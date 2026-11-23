# Technical Design & Systems Engineering

[Teamablauf](README.md) · `SYS`

## Auftrag

Spielregeln für Siedlung, Handwerk, Dorfbewohner und Weltoptionen so spezifizieren, dass ENG sie umsetzen und QA sie prüfen kann.

## Ablauf

1. Ziel und kleinsten spielbaren Umfang festlegen. Pro Regel Auslöser, Voraussetzungen, Zustandswechsel, Grenzfälle, Einheit und Startwert notieren.
2. Bei Bedarf Host- und Client-Sicht, Verhalten beim Speichern/Laden und UI-Rückmeldungen definieren. Vorschlagswerte kennzeichnen. Daten-IDs in `snake_case` dauerhaft vergeben und Referenzen prüfen.
3. Entwurf in `design/inbox/` ablegen. Klio entscheidet über offene Spielregeln. Claude Code setzt freigegebene C++- und Spieldatenänderungen um.

## Übergaben und Abnahme

- **PROD/NAR/UX → SYS** – Ziel, Abhängigkeiten, Questbedingungen und Rückmeldungen.
- **SYS → ENG/UX/QA** – Spezifikationspfad, IDs, Werte, Zustandsregeln und messbare Soll-Ergebnisse. Beispiel für Handwerk: Bei fehlenden Zutaten bleibt das Inventar unverändert.
- Abnahmefähig mit einem prüfbaren Normalfall und einem relevanten Grenzfall pro geänderter Regel.

[Datenformat](../../../docs/06_datenformat.md) · [Projektentscheidungen](../../../docs/01_entscheidungen.md)
