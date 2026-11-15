<#
.SYNOPSIS
  Import the character figure and animations and retarget them to the UE5 skeleton, headless.

.DESCRIPTION
  Copies the Epic UE5 Mannequin (Manny/Quinn + basic animations) from the engine templates into
  game/Content/Characters/Mannequins (only if missing), then runs tools/unreal/import_characters.py
  in UnrealEditor-Cmd. Editor and game must be closed. Prints the GAMER_CHAR lines and the log path.
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Script   = Join-Path $RepoRoot 'tools\unreal\import_characters.py'
$LogFile  = Join-Path $RepoRoot ("game\Saved\Logs\import_characters_{0:yyyyMMdd-HHmmss}.log" -f (Get-Date))

if (Get-Process UnrealEditor*, UnrealBuildTool -ErrorAction SilentlyContinue) {
    throw 'Unreal Editor, a game started from it or UnrealBuildTool is running. Close it / wait first.'
}

$Template = Join-Path $UeRoot 'Templates\TemplateResources\High\Characters\Content\Mannequins'
$Target   = Join-Path $RepoRoot 'game\Content\Characters\Mannequins'
if (-not (Test-Path $Target)) {
    New-Item -ItemType Directory -Force (Join-Path $Target 'Rigs'), (Join-Path $Target 'Anims\Unarmed') | Out-Null
    foreach ($d in 'Meshes', 'Materials', 'Textures') { Copy-Item -Recurse (Join-Path $Template $d) $Target }
    Copy-Item (Join-Path $Template 'Rigs\*.uasset') (Join-Path $Target 'Rigs')
    Copy-Item (Join-Path $Template 'Anims\Unarmed\MM_Idle.uasset') (Join-Path $Target 'Anims\Unarmed')
    foreach ($d in 'Jump', 'Walk', 'Jog') { Copy-Item -Recurse (Join-Path $Template "Anims\Unarmed\$d") (Join-Path $Target 'Anims\Unarmed') }
}

# Output of this script only: the retargeted animations are rebuilt each run (the batch retargeter
# cannot overwrite existing assets in the commandlet).
$AnimsOut = Join-Path $RepoRoot 'game\Content\Gamer\Characters\Anims'
if (Test-Path $AnimsOut) { Remove-Item -Recurse -Force $AnimsOut }

Write-Host '>> Importing characters ...' -ForegroundColor Cyan
$start = Get-Date
& $EditorCmd "`"$Project`"" -run=pythonscript "-script=`"$Script`"" -unattended -nosplash -nullrhi "-abslog=`"$LogFile`"" | Out-Null
$code = $LASTEXITCODE
Select-String -Path $LogFile -Pattern 'GAMER_CHAR' | ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
Write-Host ("<< exit {0} after {1:N0} s. Log: {2}" -f $code, ((Get-Date) - $start).TotalSeconds, $LogFile)
if (-not (Select-String -Path $LogFile -Pattern 'GAMER_CHAR: done, 0 error' -Quiet)) { throw 'Character import failed, see log.' }
