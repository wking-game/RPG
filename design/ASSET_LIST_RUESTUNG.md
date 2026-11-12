# Asset-Liste Rüstung und Kleidung mit Tier-Stufen (Planung, Stand 2026-11-12, überarbeitet)

*Ergänzt `design/ASSET_PLAN.md`, `design/ASSET_LIST_WAFFEN.md` und `design/ASSET_LIST_GEGENSTAENDE.md`. Für ChatGPT und den Nutzer.*
*Überarbeitet nach der Slot-Entscheidung des Nutzers (2026-11-12) und abgeglichen mit `design/inbox/06_assets_ruestung_kleidung.md` (ChatGPT/Codex).*
*Grundkleidung der Figuren gehört zu Phase 1 (`char_player_base`, `char_villager_base`), Miliz-Ausrüstung zu Phase 2, Rüstung zu Phase 3.*
*Alle Werte sind Vorschläge von Claude. Schutzwerte, Gewicht und Haltbarkeit gehören später in `data/`.*

## 1. Grundidee
- **Ausrüstung wie in Kingdom Come (bestätigt), mit den vom Nutzer festgelegten Slots** (Abschnitt 2).
- **Gleiche Stufen-Leiter wie bei den Waffen (bestätigt):** T1 Rohstufe · T2 Raseneisen · T3 Schmiedeeisen · T4 Stahl · T5 Damast · T6 Relikt.
- **Formfamilien statt Einzelteile:** Pro Slot gibt es nur 3 Grundformen (leicht, mittel, schwer) plus T6. Die Stufen unterscheiden sich über Material, Farbe und kleine Zusatzteile.
- **Individuelle Gestaltung (bestätigt):** Farben im **Charakter-Editor**, Siedlungsfarben und Wappen im **Lager-Editor**. Keine automatische Bindung an die Koop-Spielerfarbe. Technik: 3 Farbkanäle als Masken pro Teil.
- **Haltbarkeit (bestätigt, auch für Rüstung):** einfach gehalten, ein Wert pro Teil, Reparatur an Werkbank oder Schmiede. Optisch nur `fresh` und `worn` über Materialinstanz.
- **Stil:** Klarform-Saga, nordisch-frühmittelalterlich. **Keine Hörnerhelme.** Kettenhemd und Helm sind seltene Fortschritts- und Statusausrüstung, nicht Standardkleidung (so auch 06).

## 2. Slots (bestätigt am 2026-11-12)
| ID | Slot | Mesh | Beispiele | Hinweis |
|---|---|:-:|---|---|
| `slot_head` | Helm | ✓ | Fellmütze, Lederhaube, Spangenhelm, Brillenhelm | |
| `slot_shoulders` | Schulter | ✓ | Fellkragen, Leder-Schulterstücke, Lamellen | neu gegenüber der ersten Fassung |
| `slot_torso` | Oberkörper | ✓ | Tunika, Gambeson, Lederpanzer, Kettenhemd | **in der Slot-Antwort nicht genannt – offen**, vermutlich vergessen. Vorschlag: ein Slot, Unterkleidung gehört zur Grundfigur (wie in 06) |
| `slot_hands` | Hände | ✓ | Handschuhe, Armschienen | ein Paar-Mesh |
| `slot_ring_1`, `slot_ring_2` | 2 × Ring | – | Ringe | nur Icon; **Widerspruch zu „kein Schmuck-Slot“ offen** |
| `slot_legs` | Beine | ✓ | Hose, Wickelgamaschen, Beinschienen | Basishose gehört zur Grundfigur |
| `slot_feet` | Füße | ✓ | Bundschuhe, Stiefel | ein Paar-Mesh |
| `slot_cloak` | Umhang | ✓ | Wollumhang, Fellumhang | **wird nach dem ersten Winter freigeschaltet** |

**Umhang und Winter:** Es gibt Jahreszeiten mit Winter; der erste Winter ist nicht hart (bestätigt). Vorschlag: Der Umhang gibt Kälteschutz (passt zum Bedürfnis „Wärme“ aus dem Slice) und ist die Belohnung für den überstandenen ersten Winter.

**Lagen ohne Durchragen:** Schulter- und Oberkörperteile werden aufeinander abgestimmt modelliert. Verdeckte Körperteile werden ausgeblendet.

## 3. Ausrüstung nach Stufe
| Slot | T1 Rohstufe | T2 Raseneisen | T3 Schmiedeeisen | T4 Stahl | T5 Damast | T6 Relikt |
|---|---|---|---|---|---|---|
| Helm | Fellmütze, Stoffhaube | Lederhaube mit groben Eisenbändern | Spangenhelm mit Nasal | Helm mit Brillenvisier, helle Stirnkante | verzierter Helm, Messingpunkte | eigene Form |
| Schulter | Fellkragen | Leder-Schulterstücke mit Nieten | Leder mit Eisenbeschlag | Stahl-Lamellen | Lamellen mit Messingsaum | eigene Form |
| Oberkörper | wattierte Tunika | Gambeson mit Lederverstärkung | kurzes Kettenhemd über Gambeson | langes Kettenhemd, helle Kanten | Kettenhemd mit Damast-Segmenten | eigene Form |
| Hände | Stoffwickel, Fellhandschuhe | Leder-Armschienen mit Nieten | Armschienen mit Eisen | Stahl-Armschienen | Armschienen mit Damastfläche | eigene Form |
| Beine | Wickelgamaschen | Leder an Knie und Schienbein | Eisenbeschläge | Stahl-Beinschienen | Damast-Akzent | eigene Form |
| Füße | Bundschuhe | geschnürte Lederstiefel | Stiefel | verstärkte Stiefel | verzierte Stiefel | eigene Form |
| Umhang | Fellumhang | Wollumhang | Wollumhang mit Fibel | gefütterter Umhang | Umhang mit Borte | eigene Form |
| Ring | Knochenring | Eisenring | Eisenring | Stahlring | Damast-/Messingring | Reliktring |

**Formfamilien:** leicht = T1 · mittel = T2–T3 · schwer = T4–T5 · T6 eigen.

## 4. Kleidung (ohne Schutzwert)
| ID | Teil | Phase | Hinweis |
|---|---|:-:|---|
| `cloth_tunic` | Tunika | 1 | Grund-Outfit von Spieler und Dorfbewohnern |
| `cloth_trousers` | Hose | 1 | |
| `cloth_shoes` | Bundschuhe | 1 | |
| `cloth_hood` | Kapuze | 2 | |
| `cloth_apron` | Schürze | 2 | Erkennungsteil für Handwerker-Jobs |
| `cloth_dress` | Kleid/Überkleid | 2 | zweiter Körpertyp |

Berufe der Dorfbewohner erkennt man an **Farbe + Werkzeug + höchstens einem Kleidungsteil**, wie im Asset-Plan vorgesehen.

## 5. Miliz (Phase 2)
Dorfbewohner als Verteidiger tragen **vereinfachte Fassungen** der T1–T3-Ausrüstung: eigene LODs mit weniger Dreiecken, gleiche Texturen, Siedlungsfarben auf Umhang und Schild.

## 6. Budgets
Gesamtbudget aus dem Asset-Plan: Spielerfigur **40.000 Dreiecke** mit voller Ausrüstung (06 nennt 32.000–36.000 als Ziel – passt), Dorfbewohner **20.000**.

| Teil | Spieler Ziel | Dorfbewohner/Miliz Ziel |
|---|---:|---:|
| Körper, Kopf, Haare, Grundkleidung | 14.000 | 9.000 |
| Helm | 2.000 | 1.000 |
| Schulter | 2.000 | 1.000 |
| Oberkörper | 6.000 | 3.500 |
| Hände | 1.500 | 500 |
| Beine | 3.500 | 2.000 |
| Füße | 1.500 | 1.000 |
| Umhang | 3.000 | 1.500 |
| Ringe | 0 (nur Icon) | 0 |
| **Summe** | **33.500** | **19.500** |

- **Kleidungs-Atlas** `T_Clothing_Atlas_BC/_N/_ORM`, 2048² (Name aus 06 übernommen): Stoff, Wolle, Leder, Fell, Borten.
- **Kettengeflecht** `T_Mail_N`, 512², kachelbar (nur Normal Map, Farbe aus dem Metall-Set der Waffen).
- **Metall-Set** und **Damast-Maske** aus `design/ASSET_LIST_WAFFEN.md` wiederverwenden.
- **Farbmasken:** 3 Kanäle pro Teil für die Editoren.
- Je Teil 3 LODs. Keine 4K-Texturen.

## 7. Weitere Assets
| Bereich | Inhalt | Prio |
|---|---|---|
| UI | Icon pro Slot-Typ und Formfamilie + Stufen-Rahmen; Ausrüstungsansicht mit Figur; Haltbarkeitsanzeige | A |
| Charakter-Editor | Körpertyp, Kopf, Haare, Farben, Schild-Muster und -Wappen | A |
| Lager-Editor | Siedlungsfarben, Wappen-Bausteine, Banner | B |
| Props | `prop_banner` (Banner der Siedlung), `prop_armor_stand` (Rüstungsständer im Haus) | B |
| Physik | Umhang mit einfacher Physik, keine Stoffsimulation im MVP (wie in 06) | A |
| Audio | Schritte je Rüstungsklasse (Stoff, Leder, Kette) | B |

## 8. Mengenabschätzung
- 7 Slots mit Mesh × 3 Formfamilien = **21 Meshes** + 7 Relikt-Meshes + Zusatzteile (ca. 8) = rund **36**.
- Ringe: nur Icons (je Stufe eins, rund 6).
- Kleidung: rund **6** Meshes.
- **Zweiter Körpertyp:** Mit eigenen Meshes pro Körpertyp verdoppelt sich die Zahl auf rund 84. Mit **einem Mesh pro Teil plus Formanpassung (Morph Target)** bleibt es bei rund 42.

## 9. Entscheidungen
**Beantwortet am 2026-11-12:** Slots (Helm, Schulter, Hände, 2 × Ring, Beine, Füße, Umhang nach dem ersten Winter) · Haltbarkeit auch für Rüstung · keine automatische Spielerfarbe.

**Noch offen:**
1. **Oberkörper-Slot:** Gibt es ihn (vermutlich vergessen)? Einer oder zwei Lagen?
2. **Ringe vs. „kein Schmuck-Slot“:** Sind Ringe die Ausnahme, und Fibeln/Armringe bleiben Handelsware?
3. **Umhang:** nur Optik oder auch Kälteschutz für den Winter?
4. **Zweiter Körpertyp:** ein Mesh mit Formanpassung (Empfehlung) oder eigene Meshes?
5. **Helm ausblendbar** (Option im Menü)?
6. **Schutzwerte nach Schadensart** (Hieb, Stich, Wucht) wie in Kingdom Come, oder ein einziger Rüstungswert?
