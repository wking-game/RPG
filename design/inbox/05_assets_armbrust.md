# 05 – Waffen-Ergänzung: Armbrust und Projektentscheidungen

**Status:** Planung für Phase 3; keine Mesh-, Bild- oder Audio-Produktion.  
**Stil:** Klarform-Saga im mittelalterlich-nordischen Wikingerspiel.  
**Herkunft:** ergänzt aus der eingereichten Waffen-/Gegenstandsliste und den darin dokumentierten Nutzerentscheidungen.

## `weapon_crossbow` – Armbrust

- **Zweck:** Zusätzliche Fernkampfoption mit klar lesbarem, mechanischem Ladeablauf. Die Armbrust kommt als seltene, importierte oder schwarzmarktgehandelte Waffe in die Welt.
- **Phase/MVP:** In die Phase-3-Waffenplanung aufgenommen. Sie ist der achte geplante Kampfwaffentyp nach der vorhandenen Siebenerliste. Falls die erste Kampfversion zu groß wird, kann sie nach dem Kernset folgen, ohne andere Waffen-Assets zu blockieren.
- **Slot:** Fernkampf; eigener Bolzen als Munition `ammo_bolt`.
- **Tier-Vorschlag:** Kein T1/T2-Modell. Erste Verfügbarkeit ab T4 als fremdartige oder seltene Handelsware; T5 erhält eine Materialvariante; T6 bleibt eine spätere Relikt-Ausbaustufe. Das ist eine bewusste Zeit-/Weltstil-Erweiterung gegenüber einer streng frühmittelalterlichen Ausrüstung.
- **Konzept:** Kompakter Holzschaft mit gut sichtbarer Sehnenführung, einfacher Auslösemechanik, kurzem Bügel und einer robusten Verbund-Wurfarmlösung. Die Waffe muss in der Third-Person-Ansicht sofort als Armbrust erkennbar sein. Für T4 keine große Winde; ein einfacher Spannhaken bleibt als sichtbares Detail. T5 erhält nur ein kleines Damastmuster auf Metallteilen, keine vollflächig gemusterte Wurfarmlösung.
- **Teile-Strategie:** Ein Schaft-Mesh, ein Wurfarm-Mesh, ein wiederverwendetes Sehnenmaterial und ein kleines Spannhilfe-Mesh. T4/T5 unterscheiden sich überwiegend durch Materialinstanzen; keine Einzelwaffe pro Stufe.
- **Farb-/Materialregeln:** Schaft aus dem gemeinsamen Waffen-Holzatlas; Lederwicklung aus dem gemeinsamen Lederfeld; Metallmechanik aus `T_Weapon_Metal`. Keine Leuchtfarben und keine zusätzlichen Spielerfarben auf der Waffe.
- **Zustände:** `fresh` und `worn` über Materialinstanz. Keine separate Bruchgeometrie im MVP; Haltbarkeit bleibt einfach und wird durch vorhandene Projektentscheidung geregelt.
- **MVP / Ausbau:** Ein T4-Grundmodell plus T5-Materialvariante / später T6-Reliktform, dekorative Spannhilfe und eigene Trageposition.
- **Budget-Check:** Als Fern-/Zweihandwaffe: Ziel 2.000, harte Obergrenze 4.000 Dreiecke. Geteilter Waffenatlas 2048² und Metallset 1024²; drei LODs nach Waffenplan.

| ID | Unreal-Name | Inhalt | Budget |
|---|---|---|---:|
| `weapon_crossbow` | `SM_Weapon_Crossbow_Stock`, `SM_Weapon_Crossbow_Prod`, `SM_Weapon_Crossbow_Spanning_Hook` | Armbrustfamilie aus Schaft, Wurfarm und kleiner Spannhilfe; Materialinstanzen für T4/T5 | Ziel 2.000 / max. 4.000 Dreiecke für die Waffe |
| `ammo_bolt` | `SM_Ammo_Bolt_T4` | Kurzer, kräftiger Bolzen mit kompakter Befiederung und wechselnder Metallspitze | Ziel 150 / max. 300 Dreiecke |
| `prop_bolt_case` | `SM_Prop_Bolt_Case` | Schmaler Behälter für sichtbare Bolzen am Gürtel oder an der Ausrüstung | Vorschlag: Ziel 500 / max. 1.500 Dreiecke; optional, Priorität B |
| `ui_icon_weapon_crossbow` | `T_Icon_Weapon_Crossbow` | Ein Icon; Stufenrahmen nutzt vorhandene Tierfarben | Entwurf 512², im Spiel 256² |

## Abgleich der dokumentierten Entscheidungen

- Die Waffen-Stufenleiter bleibt Rohstufe → Raseneisen → Schmiedeeisen → Stahl → Damast; Relikt T6 ist für späte Bosse vorgesehen.
- Die sieben bisherigen Phase-3-Waffentypen bleiben im Plan; die Armbrust wird als zusätzlicher Typ geführt.
- Schildfarbe wird im Charakter- und Lagereditor individuell gestaltet; sie wird nicht automatisch an die Koop-Spielerfarbe gebunden.
- Blutstärke erhält eine einstellbare Darstellung. Abtrennbare Körperteile erhalten einen eigenen Ein/Aus-Schalter.
- Waffen- und Werkzeughaltbarkeit ist vorgesehen, aber einfach gehalten. Visuelle Zustände bleiben fürs MVP bei `fresh` und `worn`.

## Historischer Stilhinweis

Die Armbrust ist in dieser nordisch-frühmittelalterlichen Welt als seltene Import- oder Schwarzmarktware zu lesen, nicht als verbreitete Standardausrüstung. Der National Museum of Denmark beschreibt die Wikingerzeit als Epoche mit überwiegend regionaler Woll- und Leinenkleidung und weist darauf hin, dass Eisen teuer war und nicht jeder Krieger eine vollständige Ausrüstung besaß. Die Royal Armouries beschreibt die Armbrust als Waffe, die sich im mittelalterlichen Europa verbreitete; das Metropolitan Museum dokumentiert frühe europäische Bauformen mit Holzschaft und späteren Verbundbögen. Diese Quellen dienen als allgemeiner Zeit-/Materialkontext, nicht als visuelle Vorlage.

- [National Museum of Denmark – Clothes and jewellery](https://en.natmus.dk/historical-knowledge/denmark/prehistoric-period-until-1050-ad/the-viking-age/the-people/clothes-and-jewellery/)
- [Royal Armouries – The Hundred Years' War, Crossbow](https://royalarmouries.org/objects-and-stories/stories/the-hundred-years-war-1337-1453)
- [The Metropolitan Museum of Art – A Deadly Art: European Crossbows 1250–1850](https://resources.metmuseum.org/resources/metpublications/pdf/A_Deadly_Art_European_Crossbows_1250_1850.pdf)

## Entscheidungen, die der Nutzer treffen muss

Die Armbrust ist als Phase-3-Waffentyp aufgenommen. Offen bleibt nur, ob sie bereits im ersten spielbaren Kampf-Meilenstein mitkommt oder direkt nach den sieben Grundtypen folgt.

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `05_assets_armbrust.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext; Quellen im Historienhinweis; keine externen Assets verwendet |
