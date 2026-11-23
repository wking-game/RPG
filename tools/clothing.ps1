<#
.SYNOPSIS
  Own Norse clothing for the MetaHuman figures (free, own work; style: design/kleidung_kandidaten_kostenlos.md).

.DESCRIPTION
  Export - export the assembled MetaHuman bodies (Kraeftig, Schmal) as FBX to game/Saved/Clothing (Unreal, lock).
  Build  - Blender builds the garments from the body meshes (tools/blender/build_clothing.py): the garments are
           cut from the body surface and pushed outwards, so they keep the body's skin weights and follow every
           animation. Output: game/Saved/Clothing/cloth_<Body>_<Piece>.fbx
  Import - import the garments into /Game/Gamer/Characters/Clothing/<Body>/ on the body's skeleton (Unreal, lock)
  All    - all three.
#>
[CmdletBinding()]
param([ValidateSet('Export', 'Build', 'Import', 'All')][string]$Step = 'All')

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Blender = if ($env:BLENDER) { $env:BLENDER } else { 'E:\Blender\blender.exe' }
$Out = Join-Path $RepoRoot 'game\Saved\Clothing'
New-Item -ItemType Directory -Force $Out | Out-Null

function Invoke-UnrealPython([string]$Name, [string]$Rhi = "-nullrhi") {
    . (Join-Path $PSScriptRoot 'unreal_lock.ps1')
    $lock = Enter-UnrealLock -Owner "clothing.ps1 $Name"
    try {
        $script = Join-Path $RepoRoot "tools\unreal\$Name.py"
        $log = Join-Path $RepoRoot ("game\Saved\Logs\{0}_{1:yyyyMMdd-HHmmss}.log" -f $Name, (Get-Date))
        if ($Rhi -eq 'editor') {
            # Full editor session; the script quits the editor itself.
            $Editor = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
            # Wait for the editor only (-Wait would also wait for helper processes it leaves running, e.g. zenserver).
            $p = Start-Process -FilePath $Editor -PassThru -ArgumentList @("`"$Project`"", "-ExecutePythonScript=`"$script`"", '-unattended', '-nosplash', "-abslog=`"$log`"")
            $p.WaitForExit()
        } else {
            & $EditorCmd "`"$Project`"" -run=pythonscript "-script=`"$script`"" -unattended -nosplash $Rhi "-abslog=`"$log`"" | Out-Null
        }
        Select-String -Path $log -Pattern 'GAMER_CLOTH' | ForEach-Object { ($_.Line -replace '^\[[^\]]*\]\[[^\]]*\]', '') }
        if (-not (Select-String -Path $log -Pattern 'GAMER_CLOTH: done, 0 error' -Quiet)) { throw "$Name failed, see $log" }
    } finally { Exit-UnrealLock $lock }
}

# Skeletal mesh FBX export asserts in a commandlet (SkinnedMeshComponent MeshObject), so: full editor session.
if ($Step -in 'Export', 'All') { Invoke-UnrealPython 'clothing_export_bodies' 'editor' }
if ($Step -in 'Build', 'All') {
    $script = Join-Path $RepoRoot 'tools\blender\build_clothing.py'
    & $Blender --background --factory-startup --python $script -- $Out 2>&1 | Select-String 'GAMER_CLOTH|Error|Traceback' | ForEach-Object { $_.Line }
    if (-not (Test-Path (Join-Path $Out 'build_ok.txt'))) { throw 'Blender clothing build failed' }
}
if ($Step -in 'Import', 'All') { Invoke-UnrealPython 'clothing_import' }
