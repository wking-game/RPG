# Gamer – Open-World-Koop-Siedlungs-RPG (Arbeitstitel offen)

Stand: 2026-11-12 · Engine: Unreal Engine 5.8 (geplant, Installation noch nicht bestätigt)

Wikinger-/Mittelalter-Spiel mit Aska als Leitspiel: Du führst eine Siedlung, Dorfbewohner erledigen die Routinearbeit,
du erkundest, kämpfst, baust und besiegst Bosse – allein oder im Koop.

## Ordner

| Ordner | Inhalt |
|---|---|
| `CLAUDE.md` | Regeln für Claude Code (Rollen, Architektur, Bestätigungspflicht) |
| `AGENTS.md` | Kurzfassung für ChatGPT/Codex |
| `docs/` | Vision, Entscheidungslog, Phasenplan, Setup-Checkliste, technische Ziele |
| `design/` | Design-Briefing für ChatGPT, `inbox/` für dessen Ergebnisse, `asset_log.md` |
| `data/` | Datengetriebene Spielinhalte (Items, Jobs, Skills …) – kommt ab Phase 1 |
| `reference/` | Heruntergeladene Fremd-Projekte, nur zum Nachlesen, nicht Teil des Spiels |
| `game/` | Hier kommt das Unreal-Projekt hin (noch nicht angelegt) |

## Arbeitsteilung

- **Claude Code:** Code, Systeme, Datenformate, Tools, Struktur, Build und Tests.
- **ChatGPT:** Design (Art Direction, UI/HUD, Looks, Konzepte).
- **Du:** alle Entscheidungen; Installation der PC-Software (siehe `docs/03_setup_checkliste.md`).

## Wo wir stehen

- [x] Konzept mit Aska als Leitspiel (`docs/00_vision.md`)
- [x] Entscheidungslog und Phasenplan
- [x] Design-Briefing für ChatGPT
- [ ] Unreal Engine 5.8, Visual Studio (C++), Git installieren bzw. prüfen
- [ ] Unreal-Projekt in `game/` anlegen
- [ ] Phase 1: Vertical Slice
