# Developer / Engineering Team

[Teamablauf](README.md) · `ENG`

## Auftrag

Gameplay, Netzwerk, Persistenz, UI-Anbindung und Builds für Gamer in Unreal C++ umsetzen.

## Ablauf

1. Freigegebene Kriterien und Schnittstellen aus dem Auftrag lesen. Bestehende Systeme in `game/Source/Gamer/` und Daten in `game/Content/Data/` wiederverwenden.
2. Spielzustand serverseitig ändern. Solo und Koop verwenden denselben Ablauf. Bei Änderungen an Daten oder Speicherformat alte Spielstände prüfen.
3. Mit geschlossenem Editor über `tools/build.ps1 Editor` bauen. Betroffene Automation über `tools/test_data.ps1 -Filter <Suite>` prüfen, Gameplay mit Host und mindestens einem Client testen.

## Übergaben und Abnahme

- **SYS/UX/TA/ANIM/SFX → ENG** – Regeln, UI-Zustände und verknüpfbare Asset- oder Event-IDs mit Pfaden.
- **ENG → QA** – Build-Stand, betroffene Pfade, Startbedingungen, Prüfschritte und Logpfad. **ENG → PROD/SYS** – offene Entscheidungen und Schnittstellenänderungen.
- Fertig zur Prüfung bei erfolgreichem Build und dokumentierten Ergebnissen der betroffenen Checks.

[Projektregeln](../../../CLAUDE.md) · [Tests und Unreal-Sperre](../../../docs/03_setup_checkliste.md)
