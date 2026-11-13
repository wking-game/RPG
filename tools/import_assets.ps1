<#
.SYNOPSIS
  Import the generated assets from Assets/ into Unreal (/Game/Gamer/...), headless.

.DESCRIPTION
  Runs tools/unreal/import_assets.py in UnrealEditor-Cmd (Python commandlet). The editor and any
  game started from it must be closed (they lock the asset files). Repeatable: replaces existing assets.
  Prints the GAMER_IMPORT lines and the full log path.

.EXAMPLE
  .\tools\import_assets.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Script   = Join-Path $RepoRoot 'tools\unreal\import_assets.py'
$LogFile  = Join-Path $RepoRoot ("game\Saved\Logs\import_assets_{0:yyyyMMdd-HHmmss}.log" -f (Get-Date))

if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Unreal Editor (or a game started from it) is running. Close it first.'
}

Write-Host ">> Importing assets from $RepoRoot\Assets ..." -ForegroundColor Cyan
$start = Get-Date
& $EditorCmd "`"$Project`"" -run=pythonscript "-script=`"$Script`"" -unattended -nosplash -nullrhi "-abslog=`"$LogFile`"" | Out-Null
$code = $LASTEXITCODE

Select-String -Path $LogFile -Pattern 'GAMER_IMPORT|Error:' | Where-Object { $_.Line -notmatch 'LogPython: Error:.*(AgentSkill|PythonTestRunner|Traceback \(most recent call last\):$|init_unreal|editor_toolset|\^\^\^)' } |
    ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
Write-Host ("<< exit {0} after {1:N0} s. Log: {2}" -f $code, ((Get-Date) - $start).TotalSeconds, $LogFile)
if ($code -ne 0 -or -not (Select-String -Path $LogFile -Pattern 'GAMER_IMPORT: done, 0 error' -Quiet)) {
    throw 'Import failed, see log.'
}
