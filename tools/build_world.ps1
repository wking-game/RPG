<#
.SYNOPSIS
  Build the world map /Game/Gamer/Maps/World (copy of Template_Default + PCG vegetation), headless.

.DESCRIPTION
  Runs tools/unreal/build_world.py in UnrealEditor-Cmd (Python commandlet). The editor and any
  game started from it must be closed. Repeatable: updates the existing map and PCG assets.
  Prints the GAMER_WORLD lines and the full log path.

.EXAMPLE
  .\tools\build_world.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Script   = Join-Path $RepoRoot 'tools\unreal\build_world.py'
$LogFile  = Join-Path $RepoRoot ("game\Saved\Logs\build_world_{0:yyyyMMdd-HHmmss}.log" -f (Get-Date))

# One Unreal at a time (tools/unreal_lock.ps1): waits while another run or an open editor uses Unreal.
. (Join-Path $PSScriptRoot 'unreal_lock.ps1')
$unrealLock = Enter-UnrealLock -Owner 'build_world.ps1'
try {
Write-Host ">> Building world map ..." -ForegroundColor Cyan
$start = Get-Date
& $EditorCmd "`"$Project`"" -run=pythonscript "-script=`"$Script`"" -unattended -nosplash -nullrhi "-abslog=`"$LogFile`"" | Out-Null
$code = $LASTEXITCODE

Select-String -Path $LogFile -Pattern 'GAMER_WORLD|Error:' | Where-Object { $_.Line -notmatch 'LogPython: Error:.*(AgentSkill|PythonTestRunner|Traceback \(most recent call last\):$|init_unreal|editor_toolset|\^\^\^)' } |
    ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
Write-Host ("<< exit {0} after {1:N0} s. Log: {2}" -f $code, ((Get-Date) - $start).TotalSeconds, $LogFile)
if ($code -ne 0 -or -not (Select-String -Path $LogFile -Pattern 'GAMER_WORLD: done, 0 error' -Quiet)) {
    throw 'World build failed, see log.'
}

# Step 2: PCG cannot generate inside -run=pythonscript (it runs in the subsystem tick).
# The PCG builder commandlet ticks until all PCG components are generated and saves the map.
# Works for maps without World Partition. -GenerateComponentEditingModeNormal: our component
# is in normal editing mode; without it the builder only takes "load as preview" components.
$PcgLog = Join-Path $RepoRoot ("game\Saved\Logs\build_world_pcg_{0:yyyyMMdd-HHmmss}.log" -f (Get-Date))
Write-Host ">> Generating PCG vegetation ..." -ForegroundColor Cyan
$start = Get-Date
& $EditorCmd "`"$Project`"" /Game/Gamer/Maps/World -run=WorldPartitionBuilderCommandlet -Builder=PCGWorldPartitionBuilder -GenerateComponentEditingModeNormal -AllowCommandletRendering -unattended -nosplash "-ini:Engine:[Core.Log]:LogPCG=Verbose" "-abslog=`"$PcgLog`"" | Out-Null
$code = $LASTEXITCODE
Select-String -Path $PcgLog -Pattern 'LogPCG.*(Generat|Error|Warning)|PCGWorldPartitionBuilder|Error:' |
    Select-Object -Last 30 | ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
Write-Host ("<< exit {0} after {1:N0} s. Log: {2}" -f $code, ((Get-Date) - $start).TotalSeconds, $PcgLog)
if ($code -ne 0) {
    throw 'PCG generation failed, see log.'
}
}
finally { Exit-UnrealLock $unrealLock }
