<#
.SYNOPSIS
  Start the game into the start menu (no separate server): one or more windows on this PC.

.DESCRIPTION
  Each window opens the start menu. "Welt erstellen" makes that window the host (listen server) with the
  chosen world options; "Beitreten" in another window with address 127.0.0.1 joins it. Players on other
  PCs in the same network join with the host's IP address (port 7777, the firewall may ask once).
  For a quick test with a separate (dedicated) server use tools/play_local.ps1 (Spiel_testen.bat).

.EXAMPLE
  .\tools\start_menu.ps1
  .\tools\start_menu.ps1 -Windows 2
#>
[CmdletBinding()]
param(
    [ValidateRange(1, 8)]
    [int]$Windows = 1
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Project  = Join-Path $RepoRoot 'game\Gamer.uproject'
$UeRoot   = if ($env:UE_ROOT) { $env:UE_ROOT } else { 'E:\UE58' }
$Editor   = Join-Path $UeRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$LogDir   = Join-Path $RepoRoot ("game\Saved\StartMenu\{0:yyyyMMdd-HHmmss}" -f (Get-Date))
New-Item -ItemType Directory -Force $LogDir | Out-Null

$started = @()
for ($i = 0; $i -lt $Windows; $i++) {
    $x = 40 + ($i % 2) * 980
    $y = 40 + [math]::Floor($i / 2) * 580
    $log = Join-Path $LogDir ("window{0}.log" -f ($i + 1))
    # No map on the command line: the game starts with GameDefaultMap (Entry) = start menu.
    # One window: size and mode come from the player's graphics settings (menu "Grafik").
    # Several windows (local co-op test): small windows next to each other.
    $display = if ($Windows -eq 1) { '' } else { "-windowed -resx=960 -resy=540 -WinX=$x -WinY=$y" }
    $gameArgs = "`"$Project`" -game $display -nosplash -abslog=`"$log`""
    $started += Start-Process $Editor -ArgumentList $gameArgs -PassThru
}
Write-Host "$Windows window(s) started in the start menu. Logs: $LogDir" -ForegroundColor Green
Write-Host "Window 1: Welt erstellen. Another window: Beitreten, address 127.0.0.1. Close all windows to end."
$started | Wait-Process
