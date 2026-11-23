# Teamarbeit · Gamer

[Projektregeln](../../../AGENTS.md) · [Rollen und Technik](../../../CLAUDE.md)

ChatGPT liefert Design in `design/inbox/`. Claude Code übernimmt Spielcode, Spieldaten, Unreal-Import, Builds und Tests. Klio entscheidet über Projektfreigaben.

| Kürzel | Teamplan |
|---|---|
| PROD | [Production & Project Management](production.md) |
| ART | [Art & Design Team](art-design.md) |
| TA | [Technical Art & Pipeline Team](technical-art.md) |
| ANIM | [Animation Team](animation.md) |
| ENG | [Developer / Engineering Team](engineering.md) |
| SYS | [Technical Design & Systems Engineering](technical-design.md) |
| NAR | [Writing & Narrative Team](narrative.md) |
| SFX | [Sound Design Team](sound.md) |
| UX | [UI/UX Design Team](ui-ux.md) |
| QA | [QA & Quality Assurance](qa.md) |

## Aufnahmen aus externen Gesprächen

[Gemini-Bestandsaufnahme vom 2026-11-19](gemini-auswertung_2026-11-19.md) ordnet die vier bereitgestellten Gespräche nach `übernommen`, `angepasst`, `später prüfen` und `veraltet/ungeeignet` ein.

| Aufgabe | Führung | Zeitpunkt |
|---|---|---|
| [G-001 Ausrüstungs-Slotvertrag klären](aufgaben/G-001-ausruestungs-slotvertrag.md) | PROD | vor weiterer Rüstungsintegration |
| [G-002 Mindestens 20 Tracht-Looks modular planen](aufgaben/G-002-modulare-tracht.md) | ART | Figuren-/Ausrüstungsphase |
| [G-003 Seed-Welt und Quest-POIs spezifizieren](aufgaben/G-003-seed-welt-poi.md) | SYS | Weltphase |
| [G-004 Regie für In-Engine-Momente](aufgaben/G-004-in-engine-regie.md) | NAR | vor Story-Implementierung |
| [G-005 Gemini-Storyideen kuratieren](aufgaben/G-005-storyideen.md) | NAR | nach Freigabe des Kapitelplans |
| [G-006 Vertonungs- und SFX-Pipeline](aufgaben/G-006-voice-sfx-pipeline.md) | SFX | vor erster Serienvertonung |

## Ablauf

1. PROD kopiert die [Vorlage](aufgaben/_vorlage.md) nach `aufgaben/G-<Nummer>.md`, setzt Soll und Zuständigkeit. Parallelaufgaben erhalten eigene IDs und getrennte Dateien.
2. Der zuständige Bearbeiter setzt `in Arbeit`, ergänzt Ergebnis, Nachweis und nächsten Schritt. Bei Stillstand setzt er `blockiert` und nennt den fehlenden Input.
3. Zur Übergabe trägt er den Empfänger als zuständig ein. Für Weiterarbeit gilt `offen`, für Abnahme `Prüfung`. Im Chat genügt `G-014 | ART>TA | Holzwand für Import prüfen`. In einem neuen Chat dazu diesen Ordner verlinken.
4. Bei Abweichung geht die Aufgabe mit Befund zurück ans Fachteam. PROD setzt `fertig`, sobald Abnahme und nötige Freigaben belegt sind.

## Kontext klein halten

- Einstieg pro Auftrag: eigener Teamplan, Aufgabendatei, darin verlinkte Abschnitte. Details bei Bedarf nachladen.
- Eine schreibende Person oder ein Agent pro Aufgabe. Gemeinsam zugängliche Pfade, Abschnitte und IDs verwenden. Bei getrennten Arbeitskopien die Revision angeben.
- Nur Änderung, Nachweis und nächste Aktion melden. Bei Fehlern den relevanten Protokollauszug samt Pfad angeben.
- [Entscheidungen](../../../docs/01_entscheidungen.md) · [Phasenplan](../../../docs/02_phasenplan.md) · [Asset-Log](../../asset_log.md)
