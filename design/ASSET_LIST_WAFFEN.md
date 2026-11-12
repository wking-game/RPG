# Asset-Liste Waffen mit Tier-Stufen (Planung für Phase 3, Stand 2026-11-12)

*Ergänzt `design/ASSET_PLAN.md` (Abschnitt 8, Phase 3). Für ChatGPT und den Nutzer.*
*Vorbereitung, keine Produktion: Waffen werden erst in Phase 3 („Kampf und Ausrüstung“) gebaut, wenn der Vertical Slice stabil läuft (Architekturregel 9).*
*Alle Werte sind Vorschläge von Claude. Spielwerte (Schaden, Tempo, Haltbarkeit) gehören nicht hierher, sondern später in `data/`.*

## 1. Grundidee
- **Ausrüstung wie in Kingdom Come:** Slots, levelbasiert, schmiedbar (bestätigt).
- **Kampf ohne Ausdauer-Sperre:** Parierfenster, Haltungsleiste der Gegner, Abklingzeiten. Waffen unterscheiden sich in Reichweite, Tempo und Haltungsschaden, nicht in Ausdauerverbrauch.
- **Wenige Meshes, viele Stufen:** Pro Waffentyp gibt es nur **2–3 Kopf- oder Klingen-Meshes**. Die Stufen unterscheiden sich vor allem über **Material, Farbe und kleine Zusatzteile**. Das schützt das 20-GB-Budget.
- **Lesbarkeit (Klarform-Saga):** Die Stufe muss auch auf Distanz und im Koop-Gewühl erkennbar sein: über Metallfarbe, eine helle Schneide ab Stufe 4 und ein Muster bei Stufe 5.

## 2. Tier-Stufen
Vorschlag mit nordisch-historischer Abfolge statt der üblichen Bronze/Eisen/Silber-Leiter. Das grenzt das Spiel zugleich von den Vorbildern ab.

| ID | Name | Material-Look | Freischaltung (Vorschlag) | Erkennungszeichen |
|---|---|---|---|---|
| `tier_1` | Rohstufe | Holz, Feuerstein, Knochen, Sehnen | Spielstart | kein Metall, helle Knochen- und Steinfarben |
| `tier_2` | Raseneisen | grobes, dunkles Eisen aus Sumpferz, sichtbare Schmiedespuren | erstes Biom, Rennofen | dunkelgrau-rostbraun, raue Oberfläche |
| `tier_3` | Schmiedeeisen | sauber geschmiedetes Eisen | Boss 1 / Schmiede Stufe 2 | mittelgrau, glattere Flächen |
| `tier_4` | Stahl | aufgekohlte, gehärtete Schneide | Boss 2 / Schmiede Stufe 3 | **helle Schneidenkante** als Band |
| `tier_5` | Damast (Schweißverbund) | sichtbares Wellenmuster im Stahl | Boss 3 | **Wellenmuster** über Maske, Messing-Akzente |
| `tier_6` | Relikt *(Ausbau, offen)* | Bossmaterialien, eigenständige Formen | späte Bosse | eigene Akzentfarbe, eigene Silhouette |

**Stufenfarben für UI und Icons** (Rahmen, nicht die Waffe selbst): T1 Knochenweiß `#E5D8BC`, T2 Rostbraun `#8A5A3C`, T3 Schieferblau `#607C8B`, T4 Fjordgrün `#3E6655`, T5 Ocker `#D2A64F`, T6 Ziegelrot `#A95D48`.

## 3. Waffentypen und Verfügbarkeit
✓ = gibt es in dieser Stufe · – = nicht vorgesehen. MVP = erste Kampf-Version in Phase 3.

| ID | Waffe | Slot | MVP | T1 | T2 | T3 | T4 | T5 | T6 |
|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| `weapon_axe_bearded` | Bartaxt (einhändig) | Haupthand | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_seax` | Sax (Kurzklinge) | Haupt-/Nebenhand | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | – |
| `weapon_spear` | Speer | Haupthand (mit Schild) oder beidhändig | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_sword` | Schwert (einhändig) | Haupthand | ✓ | – | – | ✓ | ✓ | ✓ | ✓ |
| `shield_round` | Rundschild | Nebenhand | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_bow` | Bogen | Fernkampf | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `ammo_arrow` | Pfeile | Munition | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_club` | Keule | Haupthand | | ✓ | – | – | – | – | – |
| `weapon_axe_dane` | Breitaxt (zweihändig) | beidhändig | | – | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_spear_throw` | Wurfspeer | Fernkampf | | ✓ | ✓ | ✓ | ✓ | – | – |
| `weapon_sling` | Schleuder | Fernkampf | | ✓ | – | – | – | – | – |

Schwert erst ab T3: Schwerter waren teuer und selten. Das gibt dem Fortschritt ein Ziel.
Werkzeuge aus Phase 1 (`tool_axe`, `tool_pickaxe`) bleiben eigene Assets. Die Kampfaxt darf deren Stiel-Mesh wiederverwenden.

## 4. Mesh-Strategie pro Typ
Jede Waffe besteht aus **Teilen**, die kombiniert werden. Material-Instanzen erzeugen die Stufen.

| Typ | Teile | Kopf-/Klingen-Meshes |
|---|---|---|
| Bartaxt | Kopf, Stiel, Wicklung | T1 Stein-Kopf · T2–T5 Eisen-Kopf (ein Mesh) · T6 eigener Kopf |
| Sax | Klinge, Griff, Knauf | T1 Knochen/Feuerstein · T2–T5 Metall · – |
| Speer | Spitze, Schaft, Bindung | T1 Feuerstein/Knochen · T2–T5 Metall (T4/T5 mit Flügeln als Zusatzteil) · T6 eigen |
| Schwert | Klinge, Parierstange, Griff, Knauf | T3–T5 eine Klinge, **2 Knauf-Varianten** (einfach / gestuft) · T6 eigen |
| Rundschild | Scheibe, Buckel, Rand | T1 ohne Metallbuckel (Holzbuckel) · T2–T5 Metallbuckel und Randbeschlag ab T3 · T6 eigen |
| Bogen | Bogen, Sehne, Griffwicklung | T1 einfacher Holzbogen · T2–T5 verstärkter Bogen (Hornbeläge als Zusatzteil ab T4) · T6 eigen |
| Pfeil | Schaft, Spitze, Federn | 1 Schaft, 3 Spitzen (Stein, Eisen, Stahl/Damast) |
| Breitaxt | Kopf, langer Stiel | T2–T5 ein Kopf · T6 eigen |

**Schild-Bemalung:** Farbe als Material-Parameter plus 4–6 eigene Muster-Masken (keine historischen Wappen nachbilden). Vorschlag: Standardfarbe = **Koop-Spielerfarbe**, damit sich Spieler im Kampf auseinanderhalten lassen.

## 5. Budgets
| Asset-Art | Dreiecke Ziel | Obergrenze | Texturen |
|---|---:|---:|---|
| Einhandwaffe (Axt, Sax, Schwert, Keule) | 1.500 | 3.000 | geteiltes Waffen-Atlas |
| Zweihand- und Stangenwaffe (Breitaxt, Speer) | 2.000 | 4.000 | geteiltes Waffen-Atlas |
| Rundschild | 1.200 | 2.500 | Atlas + Muster-Masken 512² |
| Bogen | 1.200 | 2.500 | Atlas |
| Pfeil | 150 | 300 | Atlas |

- **Waffen-Atlas** `T_Weapon_Atlas_BC/_N/_ORM`, 2048²: Holz, Leder, Knochen, Stein, Sehne, Federn.
- **Metall-Set** `T_Weapon_Metal_BC/_N/_ORM`, 1024², kachelbar; Stufen T2–T5 über Material-Instanzen (Farbton, Rauheit, Schneidenband).
- **Damast-Maske** `T_Weapon_Damask_Mask`, 512², kachelbar.
- Je Waffe 3 LODs. Keine 4K-Texturen.

## 6. Benennung
- Basistyp: `weapon_axe_bearded` · Stufe: `tier_3` · Item im Spiel: `weapon_axe_bearded_t3`.
- Unreal: `SM_Weapon_AxeBearded_Head_T1`, `SM_Weapon_AxeBearded_Head_Metal`, `SM_Weapon_AxeBearded_Haft`, `MI_Weapon_Metal_T4`, `T_Icon_Weapon_AxeBearded`.

## 7. Weitere Assets, die an Waffen hängen
| Bereich | Inhalt | Prio |
|---|---|---|
| Animationen | Sets für einhändig, einhändig + Schild, zweihändig, Speer, Bogen; je leichter/schwerer Angriff, Kombo, **Parieren**, Block, Schildstoß, Haltungsbruch-Reaktion. **Keine Erschöpfungs-Animationen.** | A |
| Effekte | Funken beim Parieren, Treffer auf Holz/Metall/Schild, Haltungsbruch-Signal | A |
| Audio | Schwung, Treffer je Material (Holz, Metall, Schild, Körper), Parieren, Bogen spannen/lösen | A |
| UI | **ein Icon pro Waffentyp** plus Stufen-Rahmen in der Stufenfarbe (statt einem Icon pro Typ und Stufe), Haltungsleiste der Gegner, Ausrüstungs-Slots | A |
| Schmiede | Werkstücke (Rohling, Klinge), Amboss, Esse – gehört zur Schmiede-Station | B |
| Köcher, Scheide, Schildgurt | am Körper getragen | B |

## 8. Mengenabschätzung (MVP Phase 3)
7 Typen im MVP · rund **25 Meshes** (Köpfe, Klingen, Stiele, Zusatzteile) · **3 Textursätze** + Masken · **7 Icons** + 6 Stufenrahmen.
Zum Vergleich: ohne Teile-Strategie wären es 7 Typen × 6 Stufen = 42 Einzelwaffen mit eigenen Texturen.

## 9. Entscheidungen, die der Nutzer treffen muss
1. **Stufen-Leiter:** nordisch-historisch (Rohstufe → Raseneisen → Schmiedeeisen → Stahl → Damast) wie vorgeschlagen, oder klassisch mit Bronze?
2. **Relikt-Stufe T6** mit Bossmaterialien: ja, nein, später?
3. **Waffentypen im MVP:** Ist die Liste mit 7 Typen passend, oder kleiner anfangen (z. B. Axt, Speer, Schild, Bogen)?
4. **Gewaltdarstellung:** Blut ja/nein, wie stark? Das beeinflusst Effekte und die Altersfreigabe.
5. **Schildfarbe = Spielerfarbe** im Koop?
6. **Fernkampf:** nur Bogen, oder später auch Armbrust (eher Hochmittelalter)?
