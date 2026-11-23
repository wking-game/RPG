<#
.SYNOPSIS
  One Unreal at a time: a shared lock for all scripts that start Unreal (editor, game, server, build, cook).

.DESCRIPTION
  Several Claude sessions and the user work in the same project folder. Two Unreal runs at the same time
  mix screenshots, logs and saved menu settings, steal each other's ports and can run out of memory.
  Every script that starts Unreal dot-sources this file and wraps its work like this:

      . (Join-Path $PSScriptRoot 'unreal_lock.ps1')
      $lock = Enter-UnrealLock -Owner 'mp_test.ps1'
      try { ... start Unreal ... } finally { Exit-UnrealLock $lock }

  Lock file: game/Saved/unreal.lock (not in the repository), JSON with owner, PID, host process, start time.
  Busy means: the lock file names a running process, OR any Unreal process (UnrealEditor*, GamerServer*,
  Gamer*, UnrealBuildTool) is running without the lock, e.g. the user's own editor. Then the script waits
  and prints who is using Unreal, up to -TimeoutMinutes (default 60, env GAMER_UNREAL_LOCK_TIMEOUT).
  A lock whose process no longer exists is taken over.
  Nested calls (a script holding the lock calls another one, e.g. build.ps1 inside a test) reuse the lock
  via the environment variable GAMER_UNREAL_LOCK_TOKEN and do not release it.

  Manual use:
      .\tools\unreal_lock.ps1 -Status     # who holds the lock, which Unreal processes run
      .\tools\unreal_lock.ps1 -Clear      # remove a lock file by hand (only if you are sure)
#>
[CmdletBinding()]
param(
    [switch]$Status,
    [switch]$Clear
)

$script:UnrealLockPath = Join-Path (Split-Path -Parent $PSScriptRoot) 'game\Saved\unreal.lock'

function Get-UnrealProcesses {
    <# Unreal-related processes on this PC (shader workers excluded: they belong to an editor). #>
    Get-Process -ErrorAction SilentlyContinue |
        Where-Object { $_.ProcessName -match '^(UnrealEditor|UnrealEditor-Cmd|GamerServer|Gamer|UnrealBuildTool|UnrealPak)' -and
                       $_.ProcessName -notmatch '^ShaderCompileWorker' }
}

function Read-UnrealLock {
    if (-not (Test-Path $script:UnrealLockPath)) { return $null }
    try { return Get-Content $script:UnrealLockPath -Raw -ErrorAction Stop | ConvertFrom-Json }
    catch { return [pscustomobject]@{ owner = '(unlesbar)'; pid = 0; token = ''; started = '' } }
}

function Test-ProcessAlive([int]$ProcessId) {
    if ($ProcessId -le 0) { return $false }
    return [bool](Get-Process -Id $ProcessId -ErrorAction SilentlyContinue)
}

function Get-UnrealBusyReason([string]$MyToken) {
    <# Returns $null if Unreal is free for us, otherwise a German sentence why not. #>
    $lock = Read-UnrealLock
    if ($lock) {
        if ($MyToken -and $lock.token -eq $MyToken) { return $null }
        if (Test-ProcessAlive ([int]$lock.pid)) {
            return "Unreal belegt durch $($lock.owner) (PID $($lock.pid), seit $($lock.started))"
        }
        # stale: owner gone. Taken over below, but only if no Unreal process is left running.
    }
    $procs = @(Get-UnrealProcesses)
    if ($procs.Count -gt 0) {
        $names = ($procs | ForEach-Object { "$($_.ProcessName) ($($_.Id))" }) -join ', '
        return "Unreal läuft ohne Sperre (offener Editor oder Reste eines Laufs): $names"
    }
    return $null
}

function Enter-UnrealLock {
    <# Waits until Unreal is free, then takes the lock. Returns a handle for Exit-UnrealLock. #>
    param(
        [Parameter(Mandatory)][string]$Owner,
        [double]$TimeoutMinutes = $(if ($env:GAMER_UNREAL_LOCK_TIMEOUT) { [double]$env:GAMER_UNREAL_LOCK_TIMEOUT } else { 60 }),
        [int]$PollSeconds = 5
    )
    # Nested call inside a script that already holds the lock.
    $inherited = $env:GAMER_UNREAL_LOCK_TOKEN
    if ($inherited) {
        $lock = Read-UnrealLock
        if ($lock -and $lock.token -eq $inherited -and (Test-ProcessAlive ([int]$lock.pid))) {
            Write-Host "Unreal-Sperre: weiter genutzt von $($lock.owner) (verschachtelter Aufruf: $Owner)" -ForegroundColor DarkCyan
            return [pscustomobject]@{ Owned = $false; Token = $inherited }
        }
    }

    $dir = Split-Path -Parent $script:UnrealLockPath
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Force $dir | Out-Null }
    $token = [guid]::NewGuid().ToString('N')
    $deadline = (Get-Date).AddMinutes($TimeoutMinutes)
    $lastMessage = [datetime]::MinValue

    while ($true) {
        $reason = Get-UnrealBusyReason -MyToken $null
        if (-not $reason) {
            # Remove a stale lock (owner process gone, no Unreal running), then create ours atomically.
            $old = Read-UnrealLock
            if ($old -and -not (Test-ProcessAlive ([int]$old.pid))) {
                Write-Host "Unreal-Sperre: verwaiste Sperre von $($old.owner) (PID $($old.pid)) übernommen" -ForegroundColor Yellow
                Remove-Item $script:UnrealLockPath -Force -ErrorAction SilentlyContinue
            }
            $info = [ordered]@{
                owner   = $Owner
                pid     = $PID
                parent  = (Get-CimInstance Win32_Process -Filter "ProcessId=$PID" -ErrorAction SilentlyContinue).ParentProcessId
                started = (Get-Date).ToString('yyyy-MM-dd HH:mm:ss')
                token   = $token
                cwd     = (Get-Location).Path
            } | ConvertTo-Json
            try {
                $stream = [IO.File]::Open($script:UnrealLockPath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
                $bytes = [Text.Encoding]::UTF8.GetBytes($info)
                $stream.Write($bytes, 0, $bytes.Length)
                $stream.Close()
                $env:GAMER_UNREAL_LOCK_TOKEN = $token
                Write-Host "Unreal-Sperre: übernommen von $Owner (PID $PID)" -ForegroundColor DarkCyan
                return [pscustomobject]@{ Owned = $true; Token = $token }
            }
            catch [System.IO.IOException] {
                # Someone else was faster; loop again.
                $reason = 'Unreal-Sperre wurde gerade von einem anderen Lauf genommen'
            }
        }
        if ((Get-Date) -gt $deadline) {
            throw "Unreal-Sperre: nach $TimeoutMinutes Minuten nicht frei. $reason"
        }
        if (((Get-Date) - $lastMessage).TotalSeconds -ge 30) {
            Write-Host "Warte auf Unreal: $reason" -ForegroundColor Yellow
            $lastMessage = Get-Date
        }
        Start-Sleep -Seconds $PollSeconds
    }
}

function Exit-UnrealLock {
    <# Releases the lock if this handle owns it. Safe to call twice and with $null. #>
    param($Handle)
    if (-not $Handle -or -not $Handle.Owned) { return }
    $lock = Read-UnrealLock
    if ($lock -and $lock.token -eq $Handle.Token) {
        Remove-Item $script:UnrealLockPath -Force -ErrorAction SilentlyContinue
        Write-Host "Unreal-Sperre: freigegeben" -ForegroundColor DarkCyan
    }
    if ($env:GAMER_UNREAL_LOCK_TOKEN -eq $Handle.Token) { Remove-Item Env:GAMER_UNREAL_LOCK_TOKEN -ErrorAction SilentlyContinue }
    $Handle.Owned = $false
}

function Copy-UnrealScreenshots {
    <# Moves the engine screenshots taken since $Since into $Destination (own folder per run, no mixing). #>
    param([Parameter(Mandatory)][datetime]$Since, [Parameter(Mandatory)][string]$Destination)
    $root = Join-Path (Split-Path -Parent $PSScriptRoot) 'game\Saved\Screenshots'
    if (-not (Test-Path $root)) { return 0 }
    $shots = @(Get-ChildItem $root -Recurse -File -Include *.png, *.jpg, *.bmp -ErrorAction SilentlyContinue |
               Where-Object { $_.LastWriteTime -ge $Since })
    if ($shots.Count -eq 0) { return 0 }
    New-Item -ItemType Directory -Force $Destination | Out-Null
    foreach ($s in $shots) { Copy-Item $s.FullName (Join-Path $Destination $s.Name) -Force }
    return $shots.Count
}

# Direct call (not dot-sourced): status or manual clear.
if ($MyInvocation.InvocationName -ne '.') {
    if ($Clear) {
        if (Test-Path $script:UnrealLockPath) { Remove-Item $script:UnrealLockPath -Force; "Sperrdatei entfernt." } else { "Keine Sperrdatei." }
        return
    }
    $lock = Read-UnrealLock
    if ($lock) {
        $alive = if (Test-ProcessAlive ([int]$lock.pid)) { 'läuft' } else { 'beendet (verwaist)' }
        "Sperre: $($lock.owner), PID $($lock.pid) ($alive), seit $($lock.started)"
    } else { "Sperre: frei" }
    $procs = @(Get-UnrealProcesses)
    if ($procs.Count) { "Unreal-Prozesse: " + (($procs | ForEach-Object { "$($_.ProcessName) ($($_.Id))" }) -join ', ') } else { "Unreal-Prozesse: keine" }
}
