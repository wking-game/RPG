<#
.SYNOPSIS
  Play the current build locally: one server plus N client windows on this PC.

.DESCRIPTION
  Server = UnrealEditor.exe -server (uncooked, minimized log window).
  Clients = UnrealEditor.exe -game in small windows next to each other.
  Controls: WASD move, mouse look, Space jump. Click into a window to capture the mouse,
  Shift+F1 releases it. Close all client windows to end; the server is stopped then.

  The FIRST start with graphics compiles shaders and can take 10-30 minutes
  (CPU at 100 %, windows stay black or show a progress note). Later starts are fast.

.EXAMPLE
  .\tools\play_local.ps1
  .\tools\play_local.ps1 -Clients 1
#>
[CmdletBinding()]
param(
    [ValidateRange(1, 8)]
    [int]$Clients = 2,
    [string]$Map = '/Engine/Maps/Templates/Template_Default',
    [int]$Port = 7777
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$Editor   = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$LogDir   = Join-Path $RepoRoot ("game\Saved\PlayLocal\{0:yyyyMMdd-HHmmss}" -f (Get-Date))
New-Item -ItemType Directory -Force $LogDir | Out-Null
$serverLog = Join-Path $LogDir 'server.log'

$mapUrl = "$Map" + '?game=/Script/Gamer.GamerGameMode'
Write-Host "Starting server ($mapUrl) ..." -ForegroundColor Cyan
$server = Start-Process $Editor -ArgumentList "`"$Project`" $mapUrl -server -port=$Port -log -nosplash -abslog=`"$serverLog`"" -WindowStyle Minimized -PassThru

$deadline = (Get-Date).AddMinutes(10)
$listenPattern = 'GameNetDriver.*listening on port (\d+)'
while (-not ((Test-Path $serverLog) -and (Select-String -Path $serverLog -Pattern $listenPattern -Quiet))) {
    if ($server.HasExited) { throw "Server stopped early. Log: $serverLog" }
    if ((Get-Date) -gt $deadline) { Stop-Process -Id $server.Id -Force; throw "Server did not start within 10 minutes. Log: $serverLog" }
    Start-Sleep -Seconds 2
}
# If the wanted port is busy (e.g. a leftover test server), the engine takes the next one: use the real port.
$actualPort = [int](Select-String -Path $serverLog -Pattern $listenPattern | Select-Object -First 1).Matches[0].Groups[1].Value
if ($actualPort -ne $Port) {
    Write-Warning "Port $Port was busy (leftover server? Task Manager: GamerServer / UnrealEditor). Using port $actualPort."
    $Port = $actualPort
}
Write-Host "Server is up on port $Port." -ForegroundColor Green

$clientsStarted = @()
for ($i = 0; $i -lt $Clients; $i++) {
    $x = 40 + ($i % 2) * 980
    $y = 40 + [math]::Floor($i / 2) * 580
    $log = Join-Path $LogDir ("client{0}.log" -f ($i + 1))
    $clientArgs = "`"$Project`" 127.0.0.1:$Port -game -windowed -resx=960 -resy=540 -WinX=$x -WinY=$y -nosplash -log -abslog=`"$log`""
    $clientsStarted += Start-Process $Editor -ArgumentList $clientArgs -PassThru
}
Write-Host "$Clients client window(s) started. WASD = move, mouse = look, Space = jump." -ForegroundColor Green
Write-Host "Close all client windows to end. Logs: $LogDir"

$clientsStarted | Wait-Process
if (-not $server.HasExited) { Stop-Process -Id $server.Id -Force }
Write-Host "Server stopped."
