# data/

Die Spielinhalte liegen **nicht hier**, sondern in `game/Content/Data/` (Entscheidung 2026-11-13):
Unreal packt beim Paketieren nur Dateien innerhalb des Projektordners `game/` ein.

Format und Regeln: `docs/06_datenformat.md`. Kurz: eine JSON-Datei pro Tabelle, IDs in `snake_case`,
stabil, nie wiederverwenden; wegfallende Einträge bekommen `"deprecated": true` statt gelöscht zu werden.
