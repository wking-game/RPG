# 07 – Entscheidungsdelta: Asset-Plan

**Stand:** 2026-11-12  
**Zweck:** Übertragbare Zusammenfassung der Nutzerentscheidungen aus der eingereichten Waffen- und Gegenstandsliste. Dieses Blatt ist als Ergänzung für `docs/01_entscheidungen.md` gedacht.

## Festgehaltene Entscheidungen

| Bereich | Entscheidung | Konsequenz für die Asset-Planung |
|---|---|---|
| Art Direction | Klarform-Saga im mittelalterlich-nordischen Wikinger-Kontext | Klare, stilisierte Silhouetten; keine Nachahmung geschützter Spiele |
| Waffentiers | Rohstufe → Raseneisen → Schmiedeeisen → Stahl → Damast; Relikt T6 für späte Bosse vorgesehen | Tier-Looks und Materialinstanzen folgen dieser Leiter |
| Waffen-MVP | Die bisherige Siebenerliste bleibt als Kernset bestehen; Armbrust wird zusätzlich in Phase 3 geplant | `weapon_crossbow` und `ammo_bolt` sind als Zusatz aufgenommen; Reihenfolge des ersten Kampf-Meilensteins bleibt offen |
| Schildgestaltung | Farbe/Muster werden im Charakter- und Lagereditor individuell gestaltet | Keine automatische Bindung an die Koop-Spielerfarbe |
| Gewaltoptionen | Blutdarstellung ist einstellbar; Abtrennung von Körperteilen kann ein- oder ausgeschaltet werden | Für Gore-Assets und Effekte ist ein eigener späterer Design-/Technikplan nötig |
| Waffen- und Werkzeughaltbarkeit | Ja, aber einfach gehalten | Wenige Zustände; keine aufwendigen Schadensmesh-Sets im MVP |
| Nahrung | Nur rohe Nahrung verdirbt | Wie langsam und welche rohen Gegenstände betroffen sind, ist noch festzulegen |
| Handel | Händler und nächtliche Schwarzmarkthändler sind vorgesehen | Handels-/Schwarzmarkt-Assets und UI gehören in spätere Planungsphasen |
| Tierhaltung | Später; einfach gehalten und an die Tierstufen angepasst | Tiere und Erzeugnisse werden in einer späteren Phase geplant |
| Schmuck-Slot | Nein | Kein eigener Ausrüstungsslot und keine Schmuck-Assetfamilie im MVP |
| Rüstung/Kleidung | Eigene Liste gewünscht | `06_assets_ruestung_kleidung.md` erstellt |
| Phase-1-Lagerdarstellung | `item_berries` und `item_meal` haben kein eigenes Mesh | Beeren und Mahlzeiten erscheinen in `prop_basket` bzw. `prop_sack`; diese Behälter sind Phase-1-Abhängigkeiten |

## Noch offene Entscheidungen

1. Feldbau: beginnt er in Phase 2?
2. Armbrust: kommt sie in den ersten spielbaren Kampf-Meilenstein oder direkt nach dem Kernset?
3. Rüstungshaltbarkeit: bisher nicht bestätigt.
4. Wollumhang: eigener kosmetischer Slot oder Teil eines Outfit-Sets?
5. Verderb: welche rohen Nahrungstypen verderben, und in welcher Geschwindigkeit?

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `07_assetentscheidungen_2026-11-12.md` | OpenAI Codex | 2026-11-12 | Zusammenfassung der vom Nutzer eingereichten Waffen-/Gegenstandsliste; keine externen Assets verwendet |
