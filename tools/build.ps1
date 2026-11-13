<#
.SYNOPSIS
  Build pipeline for the Gamer project (Phase 0).

.DESCRIPTION
  One entry point for the common build steps:
    Editor   - build GamerEditor (Win64). Editor must be closed.
    Game     - build the Gamer game target (Win64).
    Server   - build the GamerServer dedicated server target (Win64). Source engine only.
    Package  - BuildCookRun: build, cook, stage, pak, archive to Build\Packages\<Config>,
               then report the packaged size (budget: about 20 GB, see docs/04_technische_ziele.md).
    PackageServer - BuildCookRun for the dedicated server only (no client), archive to Build\Server\<Config>.
    ProjectFiles - regenerate Visual Studio project files.

  Engine path: $env:UE_ROOT, otherwise E:\UE58 (source build, tag 5.8.4-release).
  The launcher build D:\gamer\unreal\UE_5.8 still works for Editor/Game/Package, not for Server.
  Exit code is non-zero if a step fails.

.EXAMPLE
  .\tools\build.ps1 Editor
  .\tools\build.ps1 Game -Config Shipping
  .\tools\build.ps1 Server
  .\tools\build.ps1 PackageServer -Map /Engine/Maps/Entry
  .\tools\build.ps1 Package -Config Development
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [ValidateSet('Editor', 'Game', 'Server', 'Package', 'PackageServer', 'ProjectFiles')]
    [string]$Step = 'Editor',

    [ValidateSet('Debug', 'DebugGame', 'Development', 'Shipping')]
    [string]$Config = 'Development',

    # Optional: cook only this map (e.g. /Engine/Maps/Entry) instead of the default map set.
    [string]$Map,

    # Allow building the editor target although an editor is running (not recommended).
    [switch]$Force
)

$ErrorActionPreference = 'Stop'

$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$BuildBat = Join-Path $UeRoot 'Engine\Build\BatchFiles\Build.bat'
$RunUat   = Join-Path $UeRoot 'Engine\Build\BatchFiles\RunUAT.bat'
# Launcher ("installed") builds carry this marker file; source builds do not.
$IsInstalledEngine = Test-Path (Join-Path $UeRoot 'Engine\Build\InstalledBuild.txt')

foreach ($p in @($Project, $BuildBat, $RunUat)) {
    if (-not (Test-Path $p)) { throw "Not found: $p (set UE_ROOT if the engine lives elsewhere)" }
}

function Assert-SourceEngine {
    if ($IsInstalledEngine) {
        throw "Engine at $UeRoot is a launcher build. Dedicated server targets need the source build (E:\UE58)."
    }
}

$MapArgs = if ($Map) { @("-map=$Map") } else { @() }

function Invoke-Checked([string]$Exe, [string[]]$Arguments) {
    Write-Host ">> $Exe $($Arguments -join ' ')" -ForegroundColor Cyan
    $sw = [Diagnostics.Stopwatch]::StartNew()
    & cmd /c "`"$Exe`" $($Arguments -join ' ')"
    $code = $LASTEXITCODE
    $sw.Stop()
    Write-Host ("<< exit {0} after {1:N0} s" -f $code, $sw.Elapsed.TotalSeconds) -ForegroundColor Cyan
    if ($code -ne 0) { throw "Step failed with exit code $code" }
}

function Assert-EditorClosed {
    if ($Force) { return }
    $running = Get-Process UnrealEditor -ErrorAction SilentlyContinue
    if ($running) {
        throw "Unreal Editor is running (PID $($running.Id -join ', ')). Close it first, or use -Force."
    }
}

function Get-FolderSizeGB([string]$Path) {
    $bytes = (Get-ChildItem -Path $Path -Recurse -File -ErrorAction SilentlyContinue | Measure-Object Length -Sum).Sum
    if (-not $bytes) { $bytes = 0 }
    return [math]::Round($bytes / 1GB, 2)
}

switch ($Step) {
    'ProjectFiles' {
        $pfArgs = @('-projectfiles', "-project=`"$Project`"", '-game', '-progress')
        if ($IsInstalledEngine) { $pfArgs += '-rocket' } else { $pfArgs += '-engine' }
        Invoke-Checked $BuildBat $pfArgs
    }
    'Editor' {
        Assert-EditorClosed
        Invoke-Checked $BuildBat @('GamerEditor', 'Win64', $Config, "-project=`"$Project`"", '-waitmutex')
    }
    'Game' {
        Invoke-Checked $BuildBat @('Gamer', 'Win64', $Config, "-project=`"$Project`"", '-waitmutex')
    }
    'Server' {
        Assert-SourceEngine
        Invoke-Checked $BuildBat @('GamerServer', 'Win64', $Config, "-project=`"$Project`"", '-waitmutex')
    }
    'PackageServer' {
        Assert-SourceEngine
        $archive = Join-Path $RepoRoot "game\Build\Server\$Config"
        Invoke-Checked $RunUat (@(
            'BuildCookRun', "-project=`"$Project`"", '-noP4', '-platform=Win64',
            '-server', '-serverplatform=Win64', '-noclient', "-serverconfig=$Config",
            '-build', '-cook', '-stage', '-pak', '-archive',
            "-archivedirectory=`"$archive`"", '-unattended', '-utf8output'
        ) + $MapArgs)
        Write-Host ("Server package: {0} GB in {1}" -f (Get-FolderSizeGB $archive), $archive) -ForegroundColor Green
    }
    'Package' {
        $archive = Join-Path $RepoRoot "game\Build\Packages\$Config"
        Invoke-Checked $RunUat (@(
            'BuildCookRun', "-project=`"$Project`"", '-noP4', '-platform=Win64',
            "-clientconfig=$Config", '-build', '-cook', '-stage', '-pak', '-archive',
            "-archivedirectory=`"$archive`"", '-unattended', '-utf8output'
        ) + $MapArgs)
        $size = Get-FolderSizeGB $archive
        $budget = 20
        Write-Host ("Packaged size: {0} GB in {1} (budget about {2} GB)" -f $size, $archive, $budget) -ForegroundColor Green
        if ($size -gt $budget) { Write-Warning "Packaged size exceeds the budget." }
    }
}
