<#
.SYNOPSIS
  Automated start menu test: one host creates a world from the start menu (listen server), N clients join
  it by address through the start menu. Without graphics unless -Render.

.DESCRIPTION
  The host runs -GamerMenuScript (e.g. "button:create;button:start") and then -GamerTestScript in the world;
  the clients run "button:joinpage;address:127.0.0.1:<port>;button:connect" and then their own world script.
  After -HoldSec all processes are stopped. PASS when the host log shows N "Join succeeded" and every client
  log shows "Welcomed by server". Logs: game\Saved\MenuTest\<time>\host.log, client1.log ...

.EXAMPLE
  .\tools\menu_test.ps1 -HostMenuScript "button:create;button:start" -HostScript "buildings" -HoldSec 40
#>
[CmdletBinding()]
param(
    [int]$Clients = 1,
    [string]$HostMenuScript = 'button:create;button:start',
    [string]$HostScript = '',
    [string]$ClientScript = '',
    [string]$ClientMenuScript = '',
    [int]$Port = 7777,
    [int]$HoldSec = 40,
    [switch]$Render
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$Editor   = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$LogDir   = Join-Path $RepoRoot ("game\Saved\MenuTest\{0:yyyyMMdd-HHmmss}" -f (Get-Date))
New-Item -ItemType Directory -Force $LogDir | Out-Null

if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Unreal Editor / game processes are running. Close them first (they may hold port 7777).'
}

$rhi = if ($Render) { '-windowed -resx=960 -resy=540' } else { '-nullrhi -nosound' }
$style = if ($Render) { 'Normal' } else { 'Minimized' }
function Start-Game([string]$Name, [string]$MenuScript, [string]$WorldScript) {
    $log = Join-Path $LogDir "$Name.log"
    $extra = ''
    if ($MenuScript) { $extra += " -GamerMenuScript=`"$MenuScript`"" }
    if ($WorldScript) { $extra += " -GamerTestScript=`"$WorldScript`"" }
    $cmd = "`"$Project`" -game $rhi -unattended -nosplash -nopause -abslog=`"$log`"$extra"
    Write-Host ">> ${Name}: $cmd" -ForegroundColor Cyan
    return Start-Process $Editor -ArgumentList $cmd -WindowStyle $style -PassThru
}

$all = @()
try {
    $all += Start-Game 'host' $HostMenuScript $HostScript
    $hostLog = Join-Path $LogDir 'host.log'
    $deadline = (Get-Date).AddMinutes(10)
    $listen = 'GameNetDriver.*listening on port (\d+)'
    while (-not ((Test-Path $hostLog) -and (Select-String -Path $hostLog -Pattern $listen -Quiet))) {
        if ($all[0].HasExited) { throw "Host stopped early. Log: $hostLog" }
        if ((Get-Date) -gt $deadline) { throw "Host did not open a world within 10 minutes. Log: $hostLog" }
        Start-Sleep -Seconds 2
    }
    $actualPort = [int](Select-String -Path $hostLog -Pattern $listen | Select-Object -First 1).Matches[0].Groups[1].Value
    Write-Host "Host listens on port $actualPort." -ForegroundColor Green

    $joinScript = if ($ClientMenuScript) { $ClientMenuScript } else { "button:joinpage;address:127.0.0.1:$actualPort;button:connect" }
    for ($i = 1; $i -le $Clients; $i++) {
        $all += Start-Game "client$i" $joinScript $ClientScript
    }
    Write-Host "Holding for $HoldSec s ..."
    Start-Sleep -Seconds $HoldSec
}
finally {
    foreach ($p in $all) {
        Get-CimInstance Win32_Process -Filter "ParentProcessId=$($p.Id)" -ErrorAction SilentlyContinue |
            ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue }
    }
}

$joins = @(Select-String -Path (Join-Path $LogDir 'host.log') -Pattern 'Join succeeded').Count
$welcomed = 0
for ($i = 1; $i -le $Clients; $i++) {
    $log = Join-Path $LogDir "client$i.log"
    if ((Test-Path $log) -and (Select-String -Path $log -Pattern 'Welcomed by server' -Quiet)) { $welcomed++ }
}
Write-Host ""
Write-Host "Host 'Join succeeded': $joins (expected $Clients)"
Write-Host "Clients 'Welcomed by server': $welcomed (expected $Clients)"
Write-Host "Logs: $LogDir"
if ($joins -ge $Clients -and $welcomed -eq $Clients) { Write-Host 'PASS' -ForegroundColor Green; exit 0 }
Write-Host 'FAIL' -ForegroundColor Red
exit 1
