@echo off
rem Startet das Spiel im Startmenue (Welt erstellen / Beitreten). Zwei Fenster: Spiel_starten.bat -Windows 2
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\start_menu.ps1" %*
pause
