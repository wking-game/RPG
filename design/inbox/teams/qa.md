# QA & Quality Assurance

[Teamablauf](README.md) · `QA`

## Auftrag

Spielbare Builds unabhängig anhand der vereinbarten Kriterien prüfen und Fehler reproduzierbar melden.

## Ablauf

1. Build-Stand, Weltoptionen und Testdaten erfassen. Unreal-Sperre beachten. Speicherfälle mit einem separaten Testspielstand prüfen.
2. Betroffene Pfade testen: `tools/test_data.ps1` für Spieldaten, mit `-Filter Gamer.Save` für Speichern/Laden, `tools/menu_test.ps1` für Listen-Host und Beitritt, `tools/mp_test.ps1` für Dedicated-Server-Beitritt. Gameplay auf Host und Client prüfen. UI, Grafik und Audio im gerenderten Spiel prüfen.
3. Pro Fehler Build, Rolle, Schritte, Soll/Ist, Häufigkeit und Belegpfad festhalten. Nach einem Fix gezielt nachtesten und betroffene Nachbarfunktionen prüfen.

## Übergaben und Abnahme

- **ENG/Fachteams → QA** – Build, Prüfumfang, Kriterien und Startbedingungen.
- **QA → zuständiges Team/PROD** – Auftrags-ID, Schweregrad, Befund und zugänglicher Log- oder Screenshotpfad.
- Je Kriterium `bestanden`, `fehlgeschlagen` oder `offen` mit Grund melden. Freigabeentscheidung an PROD/Klio geben.

[Testumgebung und Sperre](../../../docs/03_setup_checkliste.md)
