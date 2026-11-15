# Briefing für das Design (ChatGPT): Fotorealistische Vegetation

**Stand:** 2026-11-13 · **Von:** Claude Code (Umsetzung) · **Für:** ChatGPT (Design), zur Weitergabe durch den Nutzer

## Was sich geändert hat
Der Nutzer hat am 2026-11-13 entschieden: **Die Vegetation wird fotorealistisch** (Fotos als Vorlage, Texturen aus frei nutzbaren Fotos). Gleichzeitig gilt: **kein Geld für Asset-Dienste** (kein Meshy, keine Kaufpakete). Siehe `docs/01_entscheidungen.md`, Zeilen „Vegetation: kostenlos selbst erzeugen“ und „Stil der Vegetation“.

Das weicht von der gewählten Art Direction **Klarform-Saga** ab (`design/inbox/01_art_direction.md`: stilisiert, klare Formen, kräftige Farben, Detailgrad niedrig bis mittel).

## Bitte um Vorschlag
1. **Gesamtstil:** Wie passen fotorealistische Natur und Siedlung zusammen? Möglichkeiten: (a) alles realistischer (Gebäude, Figuren, Werkzeuge, UI-Akzente), (b) realistische Natur mit leicht stilisierter Siedlung und klaren Farbakzenten, (c) andere Lösung. Mit Vor- und Nachteilen für Aufwand, Lesbarkeit in der Vogelperspektive und Leistung.
2. **Regeln für die Vegetation:** Farbstimmung (nordisch: Fjordwiesen und Birkenhaine, `biome_fjord_meadow`), Lesbarkeit der sammelbaren Pflanzen (Beeren, Äpfel, Haselnüsse, Pilze müssen auch aus einigen Metern erkennbar sein), Zustände voll / abgeerntet / Stumpf / Setzling.
3. **Holzbaukasten:** Reichen neue, realistischere Texturen auf den vorhandenen 10 Bauteilen, oder braucht es neue Formen?

## Rahmenbedingungen
- Budgets (`CLAUDE.md`): fertiges Spiel höchstens ca. 20 GB, läuft mit mindestens 8 GB RAM; Entwicklungs-PC RTX 3070 (8 GB Grafikspeicher). Realistisches Laub (viele durchsichtige Blätter) kostet deutlich mehr Leistung als stilisiertes.
- Quellen nur frei nutzbar und dokumentiert: eigene Fotos des Nutzers oder CC0 (z. B. Poly Haven, ambientCG). Alles KI-Erzeugte ins `design/asset_log.md`.
- Umsetzung (Claude): Bäume per eigenem Generator in Blender mit Fototexturen, fotogescannte CC0-Modelle (Felsen, Stümpfe, Gras), sichtbares Nachwachsen im Spiel.

## Ergebnis bitte nach
`design/inbox/08_art_direction_fotorealismus.md`
