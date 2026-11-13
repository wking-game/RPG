<#
.SYNOPSIS
  Checks the game data files (game/Content/Data/*.json) with Unreal's automation tests.

.DESCRIPTION
  Starts UnrealEditor-Cmd without graphics, runs the tests under "Gamer.Data" and prints
  each result plus every error message. Exit code 0 = all tests passed.
  Tests (game/Source/Gamer/GamerDataTests.cpp):
    Gamer.Data.ShippedDataIsValid    - the real data files load without errors
    Gamer.Data.ValidatorFindsErrors  - deliberately broken data is reported (checks the checker)
  Takes about 1-2 minutes. Do not run while UnrealBuildTool is building.

.EXAMPLE
  .\tools\test_data.ps1
  .\tools\test_data.ps1 -Filter Gamer
#>
[CmdletBinding()]
param(
    [string]$Filter = 'Gamer.Data'
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$EditorCmd = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
if (-not (Test-Path $EditorCmd)) { throw "Not found: $EditorCmd (set UE_ROOT)" }

$LogDir = Join-Path $RepoRoot 'game\Saved\Tests'
New-Item -ItemType Directory -Force $LogDir | Out-Null
$Log = Join-Path $LogDir ("data_{0:yyyyMMdd-HHmmss}.log" -f (Get-Date))

$arguments = "`"$Project`" -ExecCmds=`"Automation RunTests $Filter;Quit`" -TestExit=`"Automation Test Queue Empty`" " +
             "-unattended -nopause -nullrhi -nosplash -nosound -log -abslog=`"$Log`""
Write-Host ">> Running automation tests '$Filter' ..." -ForegroundColor Cyan
$sw = [Diagnostics.Stopwatch]::StartNew()
$proc = Start-Process $EditorCmd -ArgumentList $arguments -WindowStyle Hidden -PassThru -Wait
$sw.Stop()

if (-not (Test-Path $Log)) { throw "No log written ($Log). Exit code $($proc.ExitCode)." }

$results = Select-String -Path $Log -Pattern 'Test Completed\. Result=\{(\w+)\}.*Path=\{([^}]+)\}' | ForEach-Object {
    [pscustomobject]@{ Result = $_.Matches[0].Groups[1].Value; Test = $_.Matches[0].Groups[2].Value }
}
$dataErrors = Select-String -Path $Log -Pattern 'LogGamer: Error: Data: (.*)' | ForEach-Object { $_.Matches[0].Groups[1].Value }
$testErrors = Select-String -Path $Log -Pattern 'LogAutomationController: Error: (.*)' | ForEach-Object { $_.Matches[0].Groups[1].Value }

Write-Host ""
if (-not $results) {
    Write-Host "No test results found - did the tests run? Log: $Log" -ForegroundColor Red
    exit 1
}
foreach ($r in $results) {
    $color = if ($r.Result -eq 'Success') { 'Green' } else { 'Red' }
    Write-Host ("{0,-8} {1}" -f $r.Result, $r.Test) -ForegroundColor $color
}
foreach ($e in ($testErrors | Select-Object -Unique)) { Write-Host "  $e" -ForegroundColor Yellow }
foreach ($e in ($dataErrors | Select-Object -Unique)) { Write-Host "  Data: $e" -ForegroundColor Yellow }

$failed = @($results | Where-Object { $_.Result -ne 'Success' }).Count
Write-Host ""
Write-Host ("{0} test(s), {1} failed, {2:N0} s. Log: {3}" -f $results.Count, $failed, $sw.Elapsed.TotalSeconds, $Log)
if ($failed -gt 0) { exit 1 } else { exit 0 }
