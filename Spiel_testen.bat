@echo off
rem Doppelklick: startet einen Server und zwei Spielfenster zum Ausprobieren (Maus, Tastatur, Kamera).
rem Beim ersten Start uebersetzt Unreal die Shader: 10-30 Minuten, Fenster bleiben so lange schwarz.
rem Steuerung: WASD laufen, Maus umsehen, Leertaste springen. Ins Fenster klicken = Maus fangen, Shift+F1 = Maus freigeben.
rem Ende: alle Spielfenster schliessen, der Server wird dann automatisch beendet.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\play_local.ps1" %*
pause
