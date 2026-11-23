<#
.SYNOPSIS
  MetaHuman figures, headless (editor must be closed).

.DESCRIPTION
  Prepare - create/preset MHC_Kraeftig and MHC_Schmal in /Game/Gamer/Characters/MetaHuman (no login needed).
  Link    - after the user assembled the figures in the editor: find the body/face meshes, make their
            skeletons compatible with SK_Mannequin and write game/Content/Data/figures.json, which
            AGamerCharacter/AGamerVillager read (without it they keep Manny/Quinn).
  Guide for the user: docs/16_metahuman_anmeldung.md
#>
[CmdletBinding()]
param(
    [ValidateSet('Prepare', 'Variants', 'Assemble', 'Link')][string]$Step = 'Prepare',
    # Assemble: which figure (one per run, to keep memory low); see metahuman_variants.py for the names
    [string]$Figure = 'MHC_Kraeftig',
    # Assemble: High (player figures: High + Low) or Low only (villagers, seeress)
    [ValidateSet('Both', 'Low', 'Mid')][string]$Quality = 'Both',
    # Variants: >0 gives the copied figures their own face (noise strength) plus a new auto-rig (Epic login)
    [double]$Faces = 0
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Script   = Join-Path $RepoRoot ("tools\unreal\metahuman_{0}.py" -f $Step.ToLower())
$LogFile  = Join-Path $RepoRoot ("game\Saved\Logs\metahuman_{0}_{1:yyyyMMdd-HHmmss}.log" -f $Step.ToLower(), (Get-Date))

# One Unreal at a time (tools/unreal_lock.ps1): waits while another run or an open editor uses Unreal.
. (Join-Path $PSScriptRoot 'unreal_lock.ps1')
$unrealLock = Enter-UnrealLock -Owner 'metahuman.ps1'
try {Write-Host ">> MetaHuman $Step ..." -ForegroundColor Cyan
$env:GAMER_MH_FIGURE = $Figure
$env:GAMER_MH_QUALITY = $Quality
$env:GAMER_MH_FACES = [string]$Faces
# Variants with -Figure '' would do all figures in one editor: too much memory, so the caller loops instead.
if ($Step -in 'Assemble', 'Variants') {
    # TextureGraph (bakes the face textures) only works in a real editor session (commandlet: blocked, or crashes
    # in TextureHelper::RawFromRT with TextureGraph.AllowCommandlets=1). So: full editor, script runs after
    # startup and quits the editor itself (metahuman_assemble.py).
    $Editor = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
    $p = Start-Process -FilePath $Editor -PassThru -ArgumentList @(
        "`"$Project`"", "-ExecutePythonScript=`"$Script`"", '-unattended', '-nosplash', "-abslog=`"$LogFile`"")
    # -Wait would also wait for helper processes the editor leaves running (zenserver)
    $p.WaitForExit()
    Write-Host "Editor exit code $($p.ExitCode)"
} else {
    & $EditorCmd "`"$Project`"" -run=pythonscript "-script=`"$Script`"" -unattended -nosplash -nullrhi "-abslog=`"$LogFile`"" | Out-Null
}
Select-String -Path $LogFile -Pattern 'GAMER_MH' | ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
Write-Host "<< Log: $LogFile"
if (-not (Select-String -Path $LogFile -Pattern 'GAMER_MH: done, 0 error' -Quiet)) { throw "MetaHuman $Step failed, see log." }
}
finally { Exit-UnrealLock $unrealLock }
