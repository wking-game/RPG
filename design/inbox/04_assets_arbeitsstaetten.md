# 04 – Assets: Arbeitsstätten der Siedlung (Phase 1, überarbeitet)

**Stil:** Klarform-Saga im mittelalterlich-nordischen Wikinger-Kontext.  
**Überarbeitet nach:** Review 01 vom 2026-11-12.  
**Umfang:** Abschnitt 5.4, ausschließlich Priorität A. Werkbank und Totem (Priorität B/C) bleiben außerhalb dieses MVP-Auftrags.

## Gemeinsame Regeln

- Holzdetails verwenden `trim_wood` aus `03_assets_baukasten.md`; Stein und Ressourcen verwenden ihre jeweils gemeinsamen Materialfamilien.
- Stationen sind aus der Vogelperspektive über Hauptform und ein bis zwei große Requisiten lesbar. Feine Details sind kein primärer Erkennungsträger.
- Dreiecksbudgets sind Zielwerte mit separater harter Obergrenze. Wiederverwendete Item-Meshes und das Feuer-VFX werden nicht doppelt in das Stationenbudget eingerechnet.
- Das Lager erhält eine eigene optimierte Gebäudehülle im Stil des modularen Baukastens, ohne wörtlich aus allen Einzelmodulen zusammengesetzt zu sein. Verdeckte Innenflächen werden weggelassen. Füllzustände verwenden wiederverwendete Item-Meshes.

## Arbeitsstättenregister – Priorität A

| ID | Unreal-Name | Empfohlene Maße | Dreiecks-Ziel | Harte Obergrenze |
|---|---|---|---:|---:|
| `building_storage` | `SM_Building_Storage` | 300 × 600 cm Grundfläche; Wandhöhe 300 cm; Firsthöhe 450 cm | 8.000 | 15.000 |
| `building_woodcutter` | `SM_Building_Woodcutter` | 300 × 300 cm Arbeitsfläche | 2.500 | 4.000 |
| `building_stonecutter` | `SM_Building_Stonecutter` | 300 × 300 cm Arbeitsfläche | 3.000 | 5.000 |
| `building_campfire` | `SM_Building_Campfire` | ca. 200 × 200 cm; ca. 170 cm hoch inkl. Aufhängung | 1.200 | 2.000 |
| `building_bed` | `SM_Building_Bed` | 210 × 90 × 50 cm | 700 | 1.000 |

## `building_storage` – zentrales Siedlungslager

- **Zweck:** Ein einziges, leicht auffindbares Lager pro Siedlung. Der sichtbare Bestand ergänzt die Lager-UI; die UI bleibt die genaue und schnell lesbare Quelle für Mengen.
- **Konzept:** Langhausförmiger Holzspeicher mit **300 × 600 cm** Grundfläche. Die 600-cm-Längsseite verläuft entlang des Firsts. Das Gebäude ist in zwei 300 × 300 cm Abschnitte geteilt: ein geschlossener Raum und eine offene Regalbucht zum Abladen. Der Giebel ist 300 cm breit, die Wandhöhe 300 cm und die Firsthöhe 450 cm. Dadurch nutzt das Gebäude das 45°-Dachmodul ohne neue Dachgeometrie.
- **Aufbauhinweis:** Eine optimierte, eigenständige Gebäudehülle im Stil des Baukastens. Einheitliche Holz-Trim-Sheet-Familie; keine verdeckten inneren Wandflächen und keine individuelle Gebäudetextur im MVP. Für den offenen Regalbucht-Abschnitt werden nur sichtbare Stützen und Ablageflächen modelliert.
- **Übergabehinweis:** Für die spätere Umsetzung steuert `BP_Building_Storage` die beiden Füllzustände; vorhandene Item-Meshes bilden den sichtbaren Bestand.

- **Füllzustände:**
  - **Leer:** freie, leere Regalbucht mit sichtbaren Ablageböden.
  - **Gefüllt:** gestapelte, wiederverwendete Meshes der Ressourcen `item_log`, `item_plank`, `item_stone`, `item_berries` und `item_meal`. Holz, Bretter und Stein liegen getrennt; Beeren und Mahlzeiten stehen auf einem geschützten Regal.
  - Zwei Zustände (leer/gefüllt) reichen für den MVP. Feine Füllstufen sind Ausbau.
- **Materialien:** Holz-Trim-Sheet; dunklere Holzinstanz für Sockel und Dach. Vorhandene Steinmaterialfamilie nur für wenige Bodenanker. Keine zusätzliche 1024²-Textur vorgesehen.
- **MVP / Ausbau:** optimierte Hülle, geschlossener 3 × 3-m-Raum, offene 3 × 3-m-Regalbucht, ein Türblatt und zwei Füllzustände / später abgestufte Füllstände, Materialstufen und Reparaturzustand.
- **Budget-Check:** Ziel max. 8.000 Dreiecke, harte Obergrenze 15.000 für die eigene Hülle. Item-Meshes werden wiederverwendet und separat gezählt. Holz-Trim-Sheet; keine eigene Textur.
- **Ursprung/Pivot:** Empfehlung für die Gebäudehülle: Mitte der 300 × 600-cm-Grundfläche auf Geländehöhe, Längsachse parallel zum 600-cm-Raster. Die Tür bleibt ein separates Mesh mit Scharnier-Pivot.

## `building_woodcutter` – Holzfäller-Arbeitsplatz

- **Zweck:** Zeigt, wo Holzfäller arbeiten und gefällte Stämme für die Siedlung vorbereiten.
- **Konzept:** Offene 300 × 300-cm-Arbeitsfläche. Ein breiter Hackklotz steht neben einem niedrigen Holzgestell mit zwei oder drei Stämmen. Eine wiederverwendete Axt steckt sichtbar im Klotz oder liegt in einer großen Halterung. Die Hauptsilhouette bleibt unter Augenhöhe der Dorfbewohner.
- **Materialien:** Holz-Trim-Sheet plus wiederverwendete Stamm- und Werkzeug-Meshes.
- **Zustände:** Eine feste Stationsform im MVP. Besetzung und Arbeitspriorität werden in der Dorf-UI angezeigt.
- **MVP / Ausbau:** Hackklotz, Holzgestell und kleiner Stammvorrat / später wechselnder Holzstapel und Reparaturzustand.
- **Budget-Check:** Ziel 2.500, harte Obergrenze 4.000 Dreiecke für die Stationsmeshgruppe. Axt und Holzressourcen werden aus anderen Asset-IDs wiederverwendet; keine zusätzliche Textur.

## `building_stonecutter` – Steinmetz-Arbeitsplatz

- **Zweck:** Kennzeichnet den Arbeitsplatz, an dem Dorfbewohner Steine bearbeiten.
- **Konzept:** Eine massive, helle Steinplatte liegt auf zwei breiten Holzstützen. Daneben steht ein grob behauener Werkstein mit gut sichtbarer Schnitt-/Spaltfläche; drei bis fünf größere Splitter markieren den Arbeitsbereich. Der Hammer und die Picke kommen aus den geplanten Werkzeug-Assets.
- **Materialien:** Holz-Trim-Sheet und gemeinsame Steinmaterialfamilie der Felsen. Keine individuelle Stationstextur.
- **Zustände:** Ein Stationszustand im MVP. Die arbeitende Figur und spätere Treffer-Effekte zeigen die Aktivität.
- **MVP / Ausbau:** Werkplatte, ein Werkstein und wenige Splitter / später Roh-/Bearbeitungszustand des Werksteins.
- **Budget-Check:** Ziel 3.000, harte Obergrenze 5.000 Dreiecke für die Stationsmeshgruppe. Wiederverwendung der Stein- und Werkzeug-Assets.

## `building_campfire` – Lagerfeuer und Kochstelle

- **Zweck:** Gemeinsame Kochstelle und warmer, weithin sichtbarer Treffpunkt der Siedlung.
- **Konzept:** Offener, flacher Ring aus sechs bis acht wiederverwendbaren Steinen mit drei großen Feuerhölzern in der Mitte. Ein einfacher Holzgalgen trägt einen dunklen Kochkessel; keine filigranen Ketten. Die Feuerform bleibt niedrig, damit Bewohner und Ausrüstung sichtbar bleiben.
- **Maße:** ca. 200 × 200 cm Arbeitsbereich; ca. 170 cm bis zur Kesselaufhängung.
- **Materialien:** gemeinsame Stein- und Holzmaterialien; Kessel mit einfachem dunklem Metallmaterial. Das Feuer-/Rauchsystem ist die separate Asset-ID `vfx_campfire`; es wird in dieser Datei nur als Abhängigkeit notiert.
- **Zustände:** Feuer aus/an über das separate VFX; Kessel bleibt sichtbar. Keine Countdown- oder Zeitdruckanzeige.
- **MVP / Ausbau:** ein Ring, Holzgalgen, Kessel und Feuerzustand / später Glut- und Feuerstufen sowie zusätzliche Kochgefäße.
- **Budget-Check:** Ziel 1.200, harte Obergrenze 2.000 Dreiecke für das Stationsmesh ohne VFX. Gemeinsame Stein-/Holzmaterialien, keine eigene Textur.

## `building_bed` – Strohlager

- **Zweck:** Einfacher, gut lesbarer Schlafplatz für eine Figur.
- **Konzept:** Niedriger Rahmen aus vier breiten Holzbalken, darauf eine voluminöse Strohlage. Eine ungefärbte Wolldecke liegt gefaltet am Fußende. Die Form bleibt bodennah und kann in Reihen oder entlang einer Innenwand platziert werden.
- **Maße:** 210 × 90 × 50 cm; für eine ca. 180-cm-Figur.
- **Materialien:** Holz-Trim-Sheet; einfache, matte Stroh- und Stoffmaterialien in Leinenfarbe. Keine eigene hochauflösende Textur im MVP.
- **Zustände:** Ein benutzbares Bett im MVP; Benutzung wird über Bewohneranimation/UI vermittelt, nicht über eine zweite Geometrie.
- **MVP / Ausbau:** ein Strohlager / später bessere Schlafplätze als Materialstufen und optionale Deckenfarben.
- **Budget-Check:** Ziel 700, harte Obergrenze 1.000 Dreiecke; gemeinsame Holzmaterialien und einfache Stoff-/Strohoberfläche.

## Entscheidungen, die der Nutzer treffen muss

Laut dem eingereichten Review sind folgende Entscheidungen übernommen und in die Konzepte eingearbeitet: Lager 300 × 600 cm; Holzdach mit schmaler Turf-Kante; offenes Lagerfeuer mit Holzgalgen und Kessel als Kochstelle. Für die Kochstelle sind damit keine offenen Auswahlfragen vorgesehen.

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `04_assets_arbeitsstaetten.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext auf Basis des Nutzer-Briefings und des eingereichten Reviews; keine externen Assets verwendet |

