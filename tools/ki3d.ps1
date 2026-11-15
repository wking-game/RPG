<#
.SYNOPSIS
  AI 3D pipeline for plants and rocks: Meshy -> Blender -> Unreal.

.DESCRIPTION
  1. tools/ki3d/meshy_generate.py   generates what is still missing (Meshy API, key from MESHY_API_KEY);
                                    raw GLBs and meshy_log.json in D:\gamer\ki3d_rohdaten\meshy (outside the repository)
  2. tools/blender/process_ki3d.py  size, pivot, triangles, texture, FBX + PNG in Assets/Nature, Assets/manifest_ki3d.json
  3. tools/import_assets.ps1        imports everything into Unreal (/Game/Gamer/Nature/...); the editor must be closed
  Each step can be skipped. -DryRun only shows what step 1 would generate and the credit estimate.

.EXAMPLE
  .\tools\ki3d.ps1 -DryRun
  .\tools\ki3d.ps1 -MaxCredits 600
  .\tools\ki3d.ps1 -Only rock_small,nettle
  .\tools\ki3d.ps1 -SkipGenerate            # only process and import what is already downloaded
#>
[CmdletBinding()]
param(
    [string[]]$Only = @(),
    [int]$MaxCredits = 0,
    [switch]$DryRun,
    [switch]$SkipGenerate,
    [switch]$SkipImport
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Blender  = if ($env:BLENDER) { $env:BLENDER } else { 'E:\Blender\blender.exe' }
$OnlyArg  = ($Only -join ',')

if (-not $SkipGenerate) {
    $genArgs = @((Join-Path $RepoRoot 'tools\ki3d\meshy_generate.py'))
    if ($DryRun) { $genArgs += '--dry-run' }
    if ($OnlyArg) { $genArgs += @('--only', $OnlyArg) }
    if ($MaxCredits -gt 0) { $genArgs += @('--max-credits', "$MaxCredits") }
    Write-Host ">> Meshy ..." -ForegroundColor Cyan
    & python @genArgs
    if ($LASTEXITCODE -ne 0) { throw "Meshy step failed (exit $LASTEXITCODE)" }
    if ($DryRun) { return }
}

Write-Host ">> Blender post-processing ..." -ForegroundColor Cyan
$blendArgs = @('-b', '--factory-startup', '-P', (Join-Path $RepoRoot 'tools\blender\process_ki3d.py'), '--')
if ($OnlyArg) { $blendArgs += @('--only', $OnlyArg) }
& $Blender @blendArgs 2>&1 | Where-Object { "$_" -match 'KI3D|Traceback|Error' } | ForEach-Object { "$_" }
if ($LASTEXITCODE -ne 0) { throw "Blender step failed (exit $LASTEXITCODE)" }

if (-not $SkipImport) {
    & (Join-Path $RepoRoot 'tools\import_assets.ps1')
}
