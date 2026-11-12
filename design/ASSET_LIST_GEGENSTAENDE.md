# Asset-Liste Gegenstände mit Tier-Stufen (Planung, Stand 2026-11-12)

*Ergänzt `design/ASSET_PLAN.md` und `design/ASSET_LIST_WAFFEN.md`. Für ChatGPT und den Nutzer.*
*Phase-1-Gegenstände (Spalte „Phase“ = 1) gehören zum Vertical Slice, alles andere ist Vorbereitung für spätere Phasen.*
*Alle Werte sind Vorschläge von Claude. Spielwerte (Stapelgröße, Gewicht, Buff-Stärke) gehören später in `data/`.*

## 1. Grundidee
- **Nicht jeder Gegenstand braucht ein 3D-Modell.** Die meisten Gegenstände sieht man nur als **Icon** im Inventar. Ein Mesh bekommt nur, was sichtbar **getragen, gestapelt oder abgelegt** wird.
- **Sammelbehälter statt Einzelmodelle:** Im Lager und beim Tragen stehen Nahrung und Kleinkram in **Sack, Korb, Fass oder Kiste**. Fallengelassene Gegenstände erscheinen als **ein gemeinsames Bündel-Mesh** mit Icon darüber.
- **Stufen wie bei den Waffen:** T1 Rohstufe · T2 Raseneisen · T3 Schmiedeeisen · T4 Stahl · T5 Damast · T6 Relikt (Ausbau, offen). Die Rohstoffe einer Stufe schalten die Waffen und Werkzeuge dieser Stufe frei.
- **IDs:** Alle Inventar-Gegenstände beginnen mit `item_`, Werkzeuge mit `tool_`, Behälter-Props mit `prop_`. Die Phase-1-IDs aus dem Asset-Plan bleiben unverändert.

Spalten: **Mesh** = eigenes 3D-Modell nötig (✓) oder nur Icon (–) bzw. Behälter (Sack/Korb/Fass/Kiste) · **Phase** = ab wann gebraucht.

## 2. Rohstoffe
| ID | Gegenstand | Stufe | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|:-:|---|
| `item_log` | Baumstamm | T1 | ✓ | 1 | wird getragen und gestapelt |
| `item_stone` | Stein | T1 | ✓ | 1 | |
| `item_stick` | Äste | T1 | – | 2 | Bündel im Lager |
| `item_fiber` | Pflanzenfasern | T1 | – | 2 | Korb |
| `item_flint` | Feuerstein | T1 | – | 2 | für T1-Waffen und -Werkzeuge |
| `item_bone` | Knochen | T1 | – | 2 | Kiste |
| `item_hide_raw` | Rohhaut | T1 | ✓ | 2 | hängt am Trockengestell |
| `item_resin` | Harz | T1 | – | 2 | Korb |
| `item_clay` | Ton | T1 | – | 2 | Korb |
| `item_thatch` | Stroh/Schilf | T1 | ✓ | 2 | Garbe; Dach und Betten |
| `item_turf` | Grassoden | T1 | ✓ | 2 | Stapel; für Turf-Dächer |
| `item_bog_ore` | Raseneisenerz | T2 | ✓ | 3 | Haufen im Lager |
| `item_charcoal` | Holzkohle | T2 | – | 3 | Sack |
| `item_wool` | Wolle | T2 | – | 2 | Korb; braucht Schafe (Tierhaltung offen) |
| `item_flax` | Flachs | T2 | – | 2 | Bündel |
| `item_iron_ore` | Eisenerz (Fels) | T4 | ✓ | 4 | Biom 2; Grundlage für Stahl |
| `item_antler` | Geweih | T3 | – | 3 | Griffe, Bogenbeläge |
| `item_amber` | Bernstein | T4 | – | 4 | Schmuck, Handel |
| `item_boss_material_*` | Bossmaterial je Boss | T3–T6 | – | 4 | einzigartige Icons, Freischaltung der nächsten Stufe |

## 3. Zwischenprodukte (hergestellt)
| ID | Gegenstand | Stufe | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|:-:|---|
| `item_plank` | Bretter | T1 | ✓ | 1 | getragen und gestapelt |
| `item_stone_block` | behauener Stein | T1 | ✓ | 2 | für die Materialstufe Stein |
| `item_rope` | Seil | T1 | – | 2 | Korb |
| `item_leather` | Leder | T2 | – | 2 | Kiste |
| `item_linen` | Leinen | T2 | – | 3 | Kiste |
| `item_cloth_wool` | Wollstoff | T2 | – | 3 | Kiste |
| `item_tar` | Teer | T2 | – | 4 | Fass |
| `item_iron_bar_bog` | Raseneisen-Barren | T2 | ✓ | 3 | Barren-Stapel, Farbe je Stufe |
| `item_iron_bar` | Schmiedeeisen-Barren | T3 | ✓ | 3 | aus Raseneisenerz in besserer Esse; gleiches Mesh, andere Materialinstanz |
| `item_steel_bar` | Stahl-Barren | T4 | ✓ | 4 | gleiches Mesh |
| `item_damask_billet` | Damast-Paket | T5 | ✓ | 5 | gleiches Mesh, Damast-Maske |
| `item_rivets` | Nieten und Nägel | T2 | – | 3 | Kiste; Schildbeschläge, Türen |

Die vier Barren teilen sich **ein Mesh** und unterscheiden sich nur über die Metall-Materialinstanzen aus der Waffenliste.

## 4. Nahrung (Buff-System, kein Ausdauer-Ersatz)
Essen gibt Buffs auf **Gesundheit, Regeneration, Traglast, Resistenzen oder Schaden** (bestätigt). Rohes Essen gibt wenig, Gekochtes mehr, Kombinationen am meisten.

| ID | Gegenstand | Stufe | Mesh | Phase | Buff-Schwerpunkt |
|---|---|:-:|:-:|:-:|---|
| `item_berries` | Beeren | T1 | – | 1 | Regeneration (klein) |
| `item_meal` | einfache Mahlzeit | T1 | – | 1 | Gesundheit; Platzhalter für den Slice |
| `item_mushrooms` | Pilze | T1 | – | 2 | Regeneration |
| `item_meat_raw` | rohes Fleisch | T1 | – | 2 | Sack/Fass im Lager |
| `item_meat_cooked` | gebratenes Fleisch | T1 | – | 2 | Gesundheit |
| `item_fish_raw` / `item_fish_cooked` | Fisch | T1 | – | 2 | Regeneration |
| `item_grain` | Gerste | T2 | – | 2 | Sack; Feldbau |
| `item_flour` | Mehl | T2 | – | 3 | Sack |
| `item_porridge` | Grütze | T2 | – | 2 | Traglast |
| `item_bread` | Fladenbrot | T2 | – | 3 | Gesundheit + Traglast |
| `item_stew` | Eintopf | T3 | – | 3 | Gesundheit + Resistenz |
| `item_honey` | Honig | T3 | – | 3 | Zutat |
| `item_mead` | Met | T3 | – | 3 | Schaden (kurz) |
| `item_feast_*` | Festmahl je Biom | T4–T5 | – | 4 | mehrere Buffs |

**Hinweis zur Regel „kein Zeitdruck“:** Verderbendes Essen würde Zeitdruck erzeugen. Vorschlag: **kein Verderben**, oder nur rohes Fleisch und Fisch sehr langsam.

## 5. Werkzeuge mit Stufen
Gleiche Teile-Strategie wie bei den Waffen: Kopf-Mesh je Material (T1 Stein, ab T2 Metall), Stiel geteilt, Stufen über Materialinstanzen.

| ID | Werkzeug | T1 | T2 | T3 | T4 | T5 | Mesh | Phase | Wirkung der Stufe |
|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| `tool_axe` | Axt | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | 1 | schneller fällen, härtere Bäume |
| `tool_pickaxe` | Spitzhacke | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | 1 | schneller abbauen, neue Erze |
| `tool_hammer` | Bauhammer | ✓ | ✓ | ✓ | – | – | ✓ | 1 | schaltet Baustufen frei |
| `tool_knife` | Messer | ✓ | ✓ | ✓ | ✓ | – | ✓ | 2 | häuten, mehr Ausbeute |
| `tool_sickle` | Sichel | – | ✓ | ✓ | ✓ | – | ✓ | 2 | Getreide, Fasern |
| `tool_hoe` | Hacke | ✓ | ✓ | ✓ | – | – | ✓ | 2 | Feldbau |
| `tool_fishing_rod` | Angel | ✓ | ✓ | – | – | – | ✓ | 2 | Fischen |
| `tool_torch` | Fackel | ✓ | – | – | – | – | ✓ | 2 | Licht (verbraucht sich langsam, kein Zeitdruck) |
| `tool_tongs` | Schmiedezange | – | ✓ | ✓ | – | – | ✓ | 3 | gehört zur Schmiede |

Dorfbewohner nutzen dieselben Werkzeug-Meshes. Ihre Arbeitsgeschwindigkeit hängt vom Werkzeug im Lager ab (Vorschlag).

## 6. Verbrauchsgüter
| ID | Gegenstand | Stufe | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|:-:|---|
| `item_bandage` | Verband | T1 | – | 3 | Heilung über Zeit |
| `item_salve` | Kräutersalbe | T2 | – | 3 | stärkere Heilung |
| `item_antidote` | Gegengift | T3 | – | 4 | Biom mit Gift |
| `item_firestarter` | Zunder | T1 | – | 2 | Lagerfeuer entzünden |
| `item_repair_kit` | Flickzeug | T2 | – | 3 | Ausrüstung unterwegs reparieren (nur wenn es Haltbarkeit gibt) |

## 7. Wertgegenstände, Handel, Quest
| ID | Gegenstand | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|---|
| `item_hacksilver` | Hacksilber (Währung) | – | 4 | nur wenn es Handel gibt |
| `item_jewelry_*` | Schmuck (Fibel, Armring) | – | 4 | Handel, Ausrüstungs-Slot „Schmuck“ (offen) |
| `item_trophy_boss_*` | Trophäe je Boss | ✓ | 4 | **eigenes Design**, platzierbar in der Siedlung |
| `item_quest_*` | Quest-Gegenstände | – | 4 | pro Questlinie |
| `item_portal_key_*` | Portal-Fokus | ✓ | 5 | für das Portalsystem im Spätspiel |

## 8. Behälter- und Ablage-Props
| ID | Prop | Mesh | Phase | Verwendung |
|---|---|:-:|:-:|---|
| `prop_sack` | Sack | ✓ | 1 | Getreide, Mehl, Holzkohle, Fleisch |
| `prop_basket` | Korb | ✓ | 1 | Beeren, Pilze, Fasern, Harz |
| `prop_barrel` | Fass | ✓ | 2 | Met, Teer, Fisch |
| `prop_crate` | Kiste | ✓ | 2 | Leder, Stoffe, Knochen, Nieten |
| `prop_item_bundle` | Bündel für abgelegte Gegenstände | ✓ | 2 | eins für alles, Icon schwebt darüber |
| `prop_drying_rack` | Trockengestell | ✓ | 2 | Häute, Fisch |

## 9. Budgets
| Asset-Art | Dreiecke Ziel | Obergrenze | Textur |
|---|---:|---:|---|
| Gegenstand mit Mesh (Stamm, Barren, Stein …) | 300 | 1.000 | geteiltes Item-Atlas 2048² |
| Behälter-Prop | 500 | 1.500 | Item-Atlas |
| Werkzeug | 800 | 2.000 | Waffen-Atlas + Metall-Set (aus der Waffenliste) |
| Trophäe | 2.000 | 4.000 | max. 1024² eigene Textur |
| Icon | – | – | 256² im Spiel, Entwurf 512², **einheitlicher Blickwinkel und Rahmen**, Stufenfarbe im Rahmen |

## 10. Mengenabschätzung
| | Phase 1 | bis Phase 3 | gesamt (Plan) |
|---|---:|---:|---:|
| Gegenstände (Icons) | 5 | ca. 55 | ca. 75 |
| Gegenstände mit eigenem Mesh | 3 | ca. 12 | ca. 16 |
| Werkzeug-Meshes (Köpfe + Stiele) | 4 | ca. 16 | ca. 18 |
| Behälter-Props | 2 | 6 | 6 |

**Abgleich mit `design/inbox/04_assets_arbeitsstaetten.md`:** Dort zeigt das gefüllte Lager Beeren und Mahlzeiten als eigene Item-Meshes. Nach dieser Liste haben `item_berries` und `item_meal` **kein eigenes Mesh**. Vorschlag: Im Lager stehen sie in `prop_basket` und `prop_sack`. Deshalb sind diese beiden Behälter schon Teil von Phase 1. Das ist der einzige Zuwachs gegenüber dem Slice im Asset-Plan.

Der größte Aufwand sind die **Icons**, nicht die Modelle. Deshalb lohnt sich früh ein festes Icon-Raster (Blickwinkel, Licht, Rahmen, Stufenfarbe).

## 11. Entscheidungen, die der Nutzer treffen muss
1. **Verderben von Nahrung:** keins (Empfehlung, wegen „kein Zeitdruck“) oder langsam nur für Rohes?
2. **Haltbarkeit von Werkzeugen und Waffen:** ja (dann Reparatur und Flickzeug) oder nein?
3. **Handel und Währung** (Hacksilber): Gibt es Händler?
4. **Tierhaltung** (Schafe, Ziegen, Hühner) für Wolle, Milch, Eier: ja, später, nein?
5. **Feldbau** (Gerste, Flachs): ab Phase 2?
6. **Schmuck-Slot** in der Ausrüstung: ja oder nein?
7. **Rüstung und Kleidung** fehlen hier noch. Soll als Nächstes eine eigene Liste dafür entstehen?
