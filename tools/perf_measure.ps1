<#
.SYNOPSIS
  Performance measurement of the packaged game (Phase 1): FPS, frame times, RAM, video memory.

.DESCRIPTION
  Starts the packaged Development game (game/Build/Packages/Development/Windows) on the map World,
  1920x1080 window, no frame limit, no V-Sync, the given quality level, and records Unreal's CSV profiler
  (FrameTime, GameThreadTime, RenderThreadTime, GPUTime ...) for -Frames frames. In parallel it samples the
  process RAM (working set, private bytes) and the video memory of the process (Windows counter
  "GPU Process Memory"). The first -WarmupSec seconds (loading, shader warm-up) are left out of the result.

  Scenarios:
    solo       - one player in the start area
    villagers  - like solo, plus -Villagers villagers (test step spawnvillager)
    hostclient - listen server (measured) plus one client window on the second screen, same PC

  Uses the Unreal lock (tools/unreal_lock.ps1). Results: game/Saved/Perf/<time>_<scenario>_q<quality>/,
  summary line in result.json.

.EXAMPLE
  .\tools\perf_measure.ps1 -Scenario solo -Quality 1
  .\tools\perf_measure.ps1 -Scenario villagers -Quality 2 -Villagers 8
#>
[CmdletBinding()]
param(
    [ValidateSet('solo', 'villagers', 'hostclient')][string]$Scenario = 'solo',
    # Unreal scalability: 0 low, 1 medium, 2 high, 3 epic
    [ValidateRange(0, 3)][int]$Quality = 1,
    [int]$Frames = 15000,
    [int]$WarmupSec = 15,
    [int]$Villagers = 8,
    [int]$TimeoutSec = 600,
    # Extra console commands for comparison runs, comma separated (e.g. 'ShowFlag.InstancedStaticMeshes 0')
    [string]$ExtraExec = '',
    # Only evaluate an existing result folder again (no new measurement)
    [string]$EvaluateDir = ''
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$PkgDir   = Join-Path $RepoRoot 'game\Build\Packages\Development\Windows'
$GameExe  = Join-Path $PkgDir 'Gamer.exe'
if (-not (Test-Path $GameExe)) { throw "Not found: $GameExe (run .\tools\build.ps1 Package)" }
$CsvDir   = Join-Path $PkgDir 'Gamer\Saved\Profiling\CSV'
$OutDir   = Join-Path $RepoRoot ("game\Saved\Perf\{0:yyyyMMdd-HHmmss}_{1}_q{2}" -f (Get-Date), $Scenario, $Quality)
if (-not $EvaluateDir) { New-Item -ItemType Directory -Force $OutDir | Out-Null }
$freeRamGb = [math]::Round((Get-CimInstance Win32_OperatingSystem).FreePhysicalMemory / 1MB, 1)

$exec = "scalability $Quality,t.MaxFPS 0,r.VSync 0," + $(if ($ExtraExec) { "$ExtraExec," } else { '' }) + "csvprofile frames=$Frames"
$common = "-windowed -ResX=1920 -ResY=1080 -nosplash -unattended -nopause"
$script = ''
if ($Scenario -eq 'villagers') { $script = "-GamerTestScript=`"wait:5;" + ((1..$Villagers | ForEach-Object { 'spawnvillager' }) -join ';') + '"' }

if (-not $EvaluateDir) {
. (Join-Path $PSScriptRoot 'unreal_lock.ps1')
$lock = Enter-UnrealLock -Owner "perf_measure.ps1 $Scenario q$Quality"
$procs = @()
$samples = @()
try {
    $before = @(Get-ChildItem $CsvDir -Filter *.csv -ErrorAction SilentlyContinue | ForEach-Object FullName)
    $map = if ($Scenario -eq 'hostclient') { '/Game/Gamer/Maps/World?listen' } else { '/Game/Gamer/Maps/World' }
    $mainArgs = "$map $common -WinX=0 -WinY=0 -ExecCmds=`"$exec`" -abslog=`"$OutDir\game.log`" -GamerPlayerId=perf_host $script"
    Write-Host ">> $GameExe $mainArgs" -ForegroundColor Cyan
    $launcher = Start-Process $GameExe -ArgumentList $mainArgs -PassThru
    $procs += $launcher
    if ($Scenario -eq 'hostclient') {
        Start-Sleep -Seconds 20
        $cArgs = "127.0.0.1 $common -WinX=1920 -WinY=0 -ExecCmds=`"scalability $Quality,t.MaxFPS 0,r.VSync 0`" -abslog=`"$OutDir\client.log`" -GamerPlayerId=perf_client"
        $procs += Start-Process $GameExe -ArgumentList $cArgs -PassThru
    }

    # The packaged Gamer.exe is a launcher; the real game is its child process.
    $deadline = (Get-Date).AddSeconds($TimeoutSec)
    $game = $null
    while (-not $game -and (Get-Date) -lt $deadline) {
        Start-Sleep -Seconds 1
        $child = Get-CimInstance Win32_Process -Filter "ParentProcessId=$($launcher.Id)" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($child) { $game = Get-Process -Id $child.ProcessId -ErrorAction SilentlyContinue }
    }
    if (-not $game) { $game = Get-Process -Id $launcher.Id }
    $start = Get-Date
    $csv = $null
    while ((Get-Date) -lt $deadline) {
        Start-Sleep -Seconds 2
        $game.Refresh()
        if ($game.HasExited) { break }
        $gpu = $null
        try {
            $c = Get-Counter "\GPU Process Memory(pid_$($game.Id)*)\Dedicated Usage" -ErrorAction Stop
            $gpu = ($c.CounterSamples | Measure-Object CookedValue -Sum).Sum
        } catch { }
        $samples += [pscustomobject]@{ t = ((Get-Date) - $start).TotalSeconds; ws = $game.WorkingSet64; priv = $game.PrivateMemorySize64; gpu = $gpu }
        $csv = Get-ChildItem $CsvDir -Filter *.csv -ErrorAction SilentlyContinue | Where-Object { $before -notcontains $_.FullName } | Select-Object -First 1
        if ($csv) {
            # Finished when the game has closed the file (it stays open and buffered while capturing).
            try { $fs = [IO.File]::Open($csv.FullName, 'Open', 'Read', 'None'); $fs.Close(); break } catch { }
        }
    }
}
finally {
    foreach ($p in $procs) {
        Get-CimInstance Win32_Process -Filter "ParentProcessId=$($p.Id)" -ErrorAction SilentlyContinue |
            ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue }
    }
    foreach ($p in $procs) { $p.WaitForExit(15000) | Out-Null }
    Exit-UnrealLock $lock
}

$samples | Export-Csv (Join-Path $OutDir 'memory.csv') -NoTypeInformation
if (-not $csv) { Write-Warning "No CSV profile written. Log: $OutDir\game.log"; exit 1 }
Copy-Item $csv.FullName (Join-Path $OutDir 'profile.csv')
}
else {
    $OutDir = $EvaluateDir
    $samples = @(Import-Csv (Join-Path $OutDir 'memory.csv') | ForEach-Object {
        [pscustomobject]@{ t = [double]$_.t; ws = [double]$_.ws; priv = [double]$_.priv; gpu = $(if ($_.gpu) { [double]$_.gpu } else { $null }) } })
    $freeRamGb = $null
}

# Evaluate: skip warm-up by accumulated frame time. The CSV can contain duplicate column names, so read
# only the columns we need, by their first index.
$lines = [IO.File]::ReadAllLines((Join-Path $OutDir 'profile.csv'))
$header = $lines[0].Split(',')
$want = 'FrameTime', 'GameThreadTime', 'RenderThreadTime', 'GPUTime', 'RHIThreadTime'
$idx = @{}; foreach ($w in $want) { $idx[$w] = [array]::IndexOf($header, $w) }
$inv = [Globalization.CultureInfo]::InvariantCulture
$rows = foreach ($l in $lines | Select-Object -Skip 1) {
    $c = $l.Split(',')
    if ($c.Count -lt $header.Count -or $c[$idx['FrameTime']] -notmatch '^[\d.]+$') { continue }
    $o = [ordered]@{}; foreach ($w in $want) { $i = $idx[$w]; $o[$w] = if ($i -ge 0 -and $c[$i] -match '^[\d.]+$') { [double]::Parse($c[$i], $inv) } else { $null } }
    [pscustomobject]$o
}
$acc = 0.0; $use = New-Object System.Collections.Generic.List[object]
foreach ($r in $rows) { $acc += $r.FrameTime / 1000.0; if ($acc -ge $WarmupSec) { $use.Add($r) } }
function Avg($col) { $v = @($use | Where-Object { $null -ne $_.$col } | ForEach-Object { $_.$col }); if ($v.Count) { [math]::Round(($v | Measure-Object -Average).Average, 2) } else { $null } }
$ft = @($use | ForEach-Object { [double]$_.FrameTime } | Sort-Object)
$p99 = if ($ft.Count) { $ft[[math]::Min($ft.Count - 1, [int]([math]::Ceiling($ft.Count * 0.99)) - 1)] } else { $null }
$memUse = @($samples | Where-Object { $_.t -ge $WarmupSec })
$result = [ordered]@{
    scenario        = $Scenario
    free_ram_gb_before = $freeRamGb
    quality         = $Quality
    frames_measured = $ft.Count
    seconds_measured = [math]::Round(($ft | Measure-Object -Sum).Sum / 1000.0, 1)
    fps_avg         = if ($ft.Count) { [math]::Round(1000.0 / (($ft | Measure-Object -Average).Average), 1) } else { $null }
    fps_1pct_low    = if ($p99) { [math]::Round(1000.0 / $p99, 1) } else { $null }
    frame_ms        = Avg 'FrameTime'
    game_ms         = Avg 'GameThreadTime'
    render_ms       = Avg 'RenderThreadTime'
    gpu_ms          = Avg 'GPUTime'
    rhi_ms          = Avg 'RHIThreadTime'
    ram_ws_gb_max   = if ($memUse.Count) { [math]::Round((($memUse | Measure-Object ws -Maximum).Maximum) / 1GB, 2) } else { $null }
    ram_priv_gb_max = if ($memUse.Count) { [math]::Round((($memUse | Measure-Object priv -Maximum).Maximum) / 1GB, 2) } else { $null }
    vram_gb_max     = if (@($memUse | Where-Object gpu).Count) { [math]::Round((($memUse | Measure-Object gpu -Maximum).Maximum) / 1GB, 2) } else { $null }
    villagers_log   = (Select-String -Path "$OutDir\game.log" -Pattern 'spawnvillager|Villager.*spawn|Neu in der Siedlung' -ErrorAction SilentlyContinue | Measure-Object).Count
    out_dir         = $OutDir
}
$result | ConvertTo-Json | Set-Content (Join-Path $OutDir 'result.json') -Encoding UTF8
[pscustomobject]$result | Format-List
