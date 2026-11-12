# Asset-Liste Rüstung und Kleidung mit Tier-Stufen (Planung, Stand 2026-11-12)

*Ergänzt `design/ASSET_PLAN.md`, `design/ASSET_LIST_WAFFEN.md` und `design/ASSET_LIST_GEGENSTAENDE.md`. Für ChatGPT und den Nutzer.*
*Grundkleidung der Figuren gehört zu Phase 1 (`char_player_base`, `char_villager_base`), Miliz-Ausrüstung zu Phase 2, Rüstung zu Phase 3.*
*Alle Werte sind Vorschläge von Claude. Schutzwerte, Gewicht und Haltbarkeit gehören später in `data/`.*

## 1. Grundidee
- **Ausrüstung wie in Kingdom Come (bestätigt), aber mit weniger Slots:** 7 statt rund 16. Das senkt die Zahl der Meshes und die Gefahr, dass Teile durcheinander ragen.
- **Gleiche Stufen-Leiter wie bei den Waffen (bestätigt):** T1 Rohstufe · T2 Raseneisen · T3 Schmiedeeisen · T4 Stahl · T5 Damast · T6 Relikt.
- **Formfamilien statt Einzelteile:** Pro Slot gibt es nur 3 Grundformen (leicht, mittel, schwer) plus T6. Die Stufen unterscheiden sich über Material, Farbe und kleine Zusatzteile.
- **Individuelle Gestaltung (bestätigt):** Farben von Kleidung, Umhang und Schild im **Charakter-Editor**, Farben und Wappen der Siedlung im **Lager-Editor** (Deutung von Claude). Technik: 3 Farbkanäle als Masken pro Teil.
- **Stil:** Klarform-Saga, nordisch-frühmittelalterlich. **Keine Hörnerhelme** (historisch falsch und ein Klischee) und keine Nachbildung bekannter Spielfiguren.
- **Haltbarkeit (bestätigt, einfach):** ein Wert pro Teil, Reparatur an Werkbank oder Schmiede, im MVP keine sichtbaren Schadenszustände.

## 2. Slots
| ID | Slot | Lage | Beispiele |
|---|---|---|---|
| `slot_head` | Kopf | außen | Kapuze, Fellmütze, Lederhaube, Spangenhelm, Brillenhelm |
| `slot_cloak` | Umhang | ganz außen | Wollumhang, Fellumhang, Umhang in Siedlungsfarben |
| `slot_torso_cloth` | Oberkörper – Kleidung | innen | Tunika, Gambeson (gepolsterter Waffenrock) |
| `slot_torso_armor` | Oberkörper – Rüstung | Mitte | Rohleder-Panzer, Kettenhemd, Lamellen, Schuppen |
| `slot_hands` | Hände und Unterarme | | Handschuhe, Armschienen |
| `slot_legs` | Beine | | Hose, Wickelgamaschen, Beinschienen |
| `slot_feet` | Füße | | Bundschuhe, Stiefel |

**Lagen ohne Durchragen:** Rüstungsteile werden immer über der dicksten Kleidung derselben Formfamilie modelliert. Verdeckte Körperteile werden ausgeblendet, damit nichts durch die Rüstung sticht.

## 3. Rüstung nach Stufe
| Slot | T1 Rohstufe | T2 Raseneisen | T3 Schmiedeeisen | T4 Stahl | T5 Damast | T6 Relikt |
|---|---|---|---|---|---|---|
| Kopf | Fellmütze | Lederhaube mit Eisenbändern | Spangenhelm mit Nasal | Helm mit Brillenvisier | verzierter Helm, Messingränder | eigene Form |
| Umhang | Fellumhang | Wollumhang | Wollumhang mit Fibel | gefütterter Umhang | Umhang mit Borte | eigene Form |
| Oberkörper – Kleidung | grobe Tunika | Gambeson | Gambeson | Gambeson, verstärkt | Gambeson, Borte | – |
| Oberkörper – Rüstung | Rohleder mit Knochenplättchen | Lederpanzer mit Nieten | kurzes Kettenhemd | langes Kettenhemd + Schulter-Lamellen | Schuppen/Lamellen mit Messingsaum | eigene Form |
| Hände | Fellhandschuhe | Lederhandschuhe | Lederarmschienen mit Eisen | Eisenarmschienen | Armschienen mit Messing | eigene Form |
| Beine | Wickelgamaschen | Lederhose, Gamaschen | Hose + Lederbeinlinge | Beinschienen | Beinschienen mit Messing | eigene Form |
| Füße | Bundschuhe | Lederschuhe | Stiefel | verstärkte Stiefel | verzierte Stiefel | eigene Form |

**Formfamilien:** leicht = T1 · mittel = T2–T3 · schwer = T4–T5 · T6 eigen.
Beispiel Oberkörper-Rüstung: ein Leder-Mesh (T1/T2), ein Kettenhemd-Mesh kurz (T3), ein Kettenhemd-Mesh lang mit Lamellen-Zusatzteil (T4/T5).

## 4. Kleidung (ohne Schutzwert)
| ID | Teil | Phase | Hinweis |
|---|---|:-:|---|
| `cloth_tunic` | Tunika | 1 | Grund-Outfit von Spieler und Dorfbewohnern |
| `cloth_trousers` | Hose | 1 | |
| `cloth_shoes` | Bundschuhe | 1 | |
| `cloth_hood` | Kapuze | 2 | |
| `cloth_cloak` | Umhang | 2 | Siedlungsfarben |
| `cloth_apron` | Schürze | 2 | Erkennungsteil für Handwerker-Jobs |
| `cloth_dress` | Kleid/Überkleid | 2 | zweiter Körpertyp |

Berufe der Dorfbewohner erkennt man an **Farbe + Werkzeug + höchstens einem Kleidungsteil** (z. B. Schürze), wie im Asset-Plan vorgesehen.

## 5. Miliz (Phase 2)
Dorfbewohner als Verteidiger tragen **vereinfachte Fassungen** der T1–T3-Rüstung: eigene LODs mit weniger Dreiecken, gleiche Texturen. Siedlungsfarben auf Umhang und Schild.

## 6. Budgets
Gesamtbudget aus dem Asset-Plan: Spielerfigur **40.000 Dreiecke** mit voller Ausrüstung, Dorfbewohner **20.000**.

| Teil | Spieler Ziel | Dorfbewohner/Miliz Ziel |
|---|---:|---:|
| Körper, Kopf, Haare | 14.000 | 8.000 |
| Kopf-Slot | 2.500 | 1.000 |
| Umhang | 3.000 | 1.500 |
| Oberkörper – Kleidung | 5.000 | 3.500 |
| Oberkörper – Rüstung | 6.000 | 2.500 |
| Hände | 1.500 | 500 |
| Beine | 4.000 | 2.000 |
| Füße | 2.000 | 1.000 |
| **Summe** | **38.000** | **20.000** |

- **Kleidungs-Atlas** `T_Cloth_Atlas_BC/_N/_ORM`, 2048²: Stoff, Wolle, Leder, Fell, Borten.
- **Kettengeflecht** `T_Mail_N`, 512², kachelbar (nur Normal Map, Farbe aus dem Metall-Set der Waffen).
- **Metall-Set** aus `design/ASSET_LIST_WAFFEN.md` wiederverwenden (Stufen über Materialinstanzen).
- **Farbmasken:** 3 Kanäle pro Teil für den Charakter- und Lager-Editor.
- Je Teil 3 LODs. Keine 4K-Texturen.

## 7. Weitere Assets
| Bereich | Inhalt | Prio |
|---|---|---|
| UI | Icon pro Teil und Formfamilie + Stufen-Rahmen; Ausrüstungsansicht mit Figur (wie in KCD); Haltbarkeitsanzeige | A |
| Charakter-Editor | Körpertyp, Kopf, Haare, Farben, Schild-Muster und -Wappen | A |
| Lager-Editor | Siedlungsfarben, Wappen-Bausteine, Banner | B |
| Props | `prop_banner` (Banner der Siedlung), `prop_armor_stand` (Rüstungsständer im Haus) | B |
| Physik | Umhang: einfache Physik-Knochen statt echter Stoffsimulation (16 Spieler) | A |
| Audio | Schritte je Rüstungsklasse (Stoff, Leder, Kette) | B |

## 8. Mengenabschätzung
- Rüstung: 7 Slots × 3 Formfamilien = **21 Meshes** + 7 Relikt-Meshes + Zusatzteile (ca. 8) = rund **36**.
- Kleidung: rund **7** Meshes.
- **Achtung zweiter Körpertyp:** Der Asset-Plan sieht 2 Körpertypen vor. Bekommt jeder Körpertyp eigene Meshes, verdoppelt sich das auf rund 86. Mit **einem Mesh pro Teil plus Formanpassung (Morph Target)** für den zweiten Körpertyp bleibt es bei rund 43.

## 9. Entscheidungen, die der Nutzer treffen muss
1. **7 Slots** wie vorgeschlagen, oder mehr Lagen wie in Kingdom Come?
2. **Zweiter Körpertyp:** ein Mesh mit Formanpassung (Empfehlung) oder eigene Meshes?
3. **Helm ausblendbar** (Option im Menü)?
4. **Umhang:** einfache Physik (Empfehlung) oder echte Stoffsimulation?
5. **Schutzwerte nach Schadensart** (Hieb, Stich, Wucht) wie in Kingdom Come, oder ein einziger Rüstungswert?
6. **Sichtbare Abnutzung** bei sinkender Haltbarkeit: ja (später) oder nein?
7. **Keine Hörnerhelme:** einverstanden?
