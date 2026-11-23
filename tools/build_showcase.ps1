<#
.SYNOPSIS
  Build the asset showcase map /Game/Gamer/Maps/Showcase: every static mesh under /Game/Gamer in labelled rows.

.EXAMPLE
  .\tools\build_showcase.ps1
  .\tools\mp_test.ps1 -Clients 1 -Render -Map /Game/Gamer/Maps/Showcase -HoldSec 90 -Client1Args '-GamerTestScript=wait:6;bird:on;birdfocus:at:4000,0;birdzoom:3000;birdturn:0,-60;shot'
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Script   = Join-Path $RepoRoot 'tools\unreal\build_showcase.py'
$LogFile  = Join-Path $RepoRoot ("game\Saved\Logs\build_showcase_{0:yyyyMMdd-HHmmss}.log" -f (Get-Date))

# One Unreal at a time (tools/unreal_lock.ps1): waits while another run or an open editor uses Unreal.
. (Join-Path $PSScriptRoot 'unreal_lock.ps1')
$unrealLock = Enter-UnrealLock -Owner 'build_showcase.ps1'
try {& $EditorCmd "`"$Project`"" -run=pythonscript "-script=`"$Script`"" -unattended -nosplash -nullrhi "-abslog=`"$LogFile`"" | Out-Null
$code = $LASTEXITCODE
Select-String -Path $LogFile -Pattern 'GAMER_SHOWCASE|Error:' | Where-Object { $_.Line -notmatch 'LogPython: Error:.*(AgentSkill|PythonTestRunner|init_unreal|editor_toolset)' } |
    ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
Write-Host ("<< exit {0}. Log: {1}" -f $code, $LogFile)
if ($code -ne 0 -or -not (Select-String -Path $LogFile -Pattern 'GAMER_SHOWCASE: done, 0 error' -Quiet)) { throw 'Showcase build failed.' }
}
finally { Exit-UnrealLock $unrealLock }
