<#
.SYNOPSIS
  Multiplayer smoke test: one server plus N clients on this PC, checks that every client joins.

.DESCRIPTION
  Mode Editor   - server = UnrealEditor.exe -server (uncooked content), no packaging needed.
  Mode Packaged - server = GamerServer.exe from tools\build.ps1 PackageServer (cooked content).
  Clients are always UnrealEditor.exe -game. By default without rendering (-nullrhi), so no
  shader compilation is needed; use -Render to get real client windows.

  Passes when the server log shows one "Join succeeded" per client and every client log shows
  "Welcomed by server". With -ExpectedJoins lower than -Clients it checks the player limit
  instead: exactly that many joins, and every surplus client is refused with "Server full". Logs land in game\Saved\MpTest\<timestamp>\. All started processes are
  stopped at the end. Do not run while a UnrealBuildTool build is running: the editor calls
  UnrealBuildTool on startup and waits for it.

.EXAMPLE
  .\tools\mp_test.ps1
  .\tools\mp_test.ps1 -Mode Packaged -Clients 2
  .\tools\mp_test.ps1 -Render -Clients 1
  .\tools\mp_test.ps1 -Clients 9 -ExpectedJoins 8     # MaxPlayers=8: the 9th must be refused
#>
[CmdletBinding()]
param(
    [ValidateSet('Editor', 'Packaged')]
    [string]$Mode = 'Editor',

    [ValidateRange(1, 16)]
    [int]$Clients = 2,

    # How many clients must get in. Default: all of them. Lower it to test the player limit.
    [int]$ExpectedJoins = -1,

    [string]$Map = '/Engine/Maps/Entry',
    [int]$Port = 7777,

    # Seconds to wait for the server to listen and for all clients to join.
    [int]$TimeoutSec = 600,

    [switch]$Render
)

$ErrorActionPreference = 'Stop'

$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$Editor   = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$LogDir   = Join-Path $RepoRoot ("game\Saved\MpTest\{0:yyyyMMdd-HHmmss}" -f (Get-Date))
New-Item -ItemType Directory -Force $LogDir | Out-Null

if (-not (Test-Path $Editor)) { throw "Not found: $Editor (set UE_ROOT)" }

function Wait-ForLog([string]$Path, [string]$Pattern, [int]$Count, [Diagnostics.Process[]]$MustLive, [datetime]$Deadline) {
    while ((Get-Date) -lt $Deadline) {
        if (Test-Path $Path) {
            $hits = (Select-String -Path $Path -Pattern $Pattern -ErrorAction SilentlyContinue | Measure-Object).Count
            if ($hits -ge $Count) { return $hits }
        }
        foreach ($p in $MustLive) {
            if ($p.HasExited) { throw "Process $($p.Id) exited early (code $($p.ExitCode)). Log: $Path" }
        }
        Start-Sleep -Seconds 2
    }
    $hits = if (Test-Path $Path) { (Select-String -Path $Path -Pattern $Pattern | Measure-Object).Count } else { 0 }
    return $hits
}

$started = @()
$serverLog = Join-Path $LogDir 'server.log'
try {
    # --- Server ---
    if ($Mode -eq 'Editor') {
        $srvExe  = $Editor
        $srvArgs = "`"$Project`" $Map -server -port=$Port -log -unattended -nosplash -nopause -abslog=`"$serverLog`""
    } else {
        $pkg = Join-Path $RepoRoot 'game\Build\Server\Development'
        $srvExe = Get-ChildItem $pkg -Recurse -Filter 'GamerServer.exe' -ErrorAction SilentlyContinue |
                  Sort-Object { $_.FullName.Length } | Select-Object -First 1 -ExpandProperty FullName
        if (-not $srvExe) { throw "GamerServer.exe not found under $pkg. Run: .\tools\build.ps1 PackageServer" }
        $srvArgs = "$Map -port=$Port -log -unattended -nopause -abslog=`"$serverLog`""
    }
    Write-Host ">> Server: $srvExe $srvArgs" -ForegroundColor Cyan
    $server = Start-Process $srvExe -ArgumentList $srvArgs -WindowStyle Minimized -PassThru
    $started += $server

    $deadline = (Get-Date).AddSeconds($TimeoutSec)
    $listen = Wait-ForLog $serverLog "listening on port $Port" 1 @($server) $deadline
    if ($listen -lt 1) { throw "Server did not listen on port $Port within $TimeoutSec s. Log: $serverLog" }
    Write-Host "Server listens on port $Port." -ForegroundColor Green

    # --- Clients ---
    $clientLogs = @()
    for ($i = 1; $i -le $Clients; $i++) {
        $cLog = Join-Path $LogDir "client$i.log"
        $clientLogs += $cLog
        $rhi = if ($Render) { '-windowed -resx=960 -resy=540' } else { '-nullrhi -nosound' }
        $cArgs = "`"$Project`" 127.0.0.1:$Port -game $rhi -unattended -nosplash -nopause -abslog=`"$cLog`""
        Write-Host ">> Client ${i}: $cArgs" -ForegroundColor Cyan
        $started += Start-Process $Editor -ArgumentList $cArgs -WindowStyle Minimized -PassThru
    }

    if ($ExpectedJoins -lt 0) { $ExpectedJoins = $Clients }
    $refusals = $Clients - $ExpectedJoins

    # Only the server must stay alive; refused clients may quit on their own.
    $joins = Wait-ForLog $serverLog 'Join succeeded' $ExpectedJoins @($server) $deadline
    $refused = 0
    if ($refusals -gt 0) {
        while ((Get-Date) -lt $deadline) {
            $refused = 0
            foreach ($cLog in $clientLogs) {
                if ((Test-Path $cLog) -and (Select-String -Path $cLog -Pattern 'Server full' -Quiet)) { $refused++ }
            }
            if ($refused -ge $refusals) { break }
            Start-Sleep -Seconds 2
        }
        Start-Sleep -Seconds 10   # grace period: no further join may slip through
        $joins = (Select-String -Path $serverLog -Pattern 'Join succeeded' | Measure-Object).Count
    }
    $welcomed = 0
    foreach ($cLog in $clientLogs) {
        if ((Test-Path $cLog) -and (Select-String -Path $cLog -Pattern 'Welcomed by server' -Quiet)) { $welcomed++ }
    }

    Write-Host ""
    Write-Host ("Server 'Join succeeded': {0} (expected {1})" -f $joins, $ExpectedJoins)
    Write-Host ("Clients 'Welcomed by server': {0} (expected {1})" -f $welcomed, $ExpectedJoins)
    if ($refusals -gt 0) { Write-Host ("Clients refused with 'Server full': {0} (expected {1})" -f $refused, $refusals) }
    Write-Host "Logs: $LogDir"
    if ($joins -eq $ExpectedJoins -and $welcomed -eq $ExpectedJoins -and $refused -ge $refusals) {
        Write-Host "PASS" -ForegroundColor Green
        $exit = 0
    } else {
        Write-Host "FAIL" -ForegroundColor Red
        Select-String -Path (@($serverLog) + $clientLogs) -Pattern 'NetworkFailure|TravelFailure|Error:|Fatal' -ErrorAction SilentlyContinue |
            Select-Object -First 20 | ForEach-Object { Write-Host $_.Line -ForegroundColor Yellow }
        $exit = 1
    }
}
finally {
    foreach ($p in $started) {
        if ($p -and -not $p.HasExited) { Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue }
    }
}
exit $exit
