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
| `tier_6` | Relikt *(bestätigt, Ausbau)* | Bossmaterialien, eigenständige Formen | späte Bosse | eigene Akzentfarbe, eigene Silhouette |

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
| `weapon_crossbow` | Armbrust | Fernkampf | | – | – | ✓ | ✓ | ✓ | ✓ |
| `ammo_bolt` | Bolzen | Munition | | – | – | ✓ | ✓ | ✓ | ✓ |

Schwert erst ab T3: Schwerter waren teuer und selten. Das gibt dem Fortschritt ein Ziel.
Armbrust: **Freischaltung über bestimmte Attribute plus eine Fähigkeit im Skilltree** (bestätigt). Ab welcher Stufe ist offen: Claude schlägt T3 vor (Mechanik braucht Schmiedeeisen), ChatGPT/Codex T4 als seltene Import- oder Schwarzmarktware (`design/inbox/05_assets_armbrust.md`). Die T4-Variante passt gut zum bestätigten Nacht-Händler. Langsamer als der Bogen, dafür mehr Haltungsschaden. Ausgestaltung der Teile siehe 05.
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
| Armbrust | Säule, Bogen, Sehne, Abzug | T3–T5 eine Säule und ein Bogen (Horn-Komposit als Zusatzteil ab T4) · T6 eigen |
| Bolzen | Schaft, Spitze | 1 Schaft, 2 Spitzen (Eisen, Stahl/Damast) |

**Schild-Gestaltung (bestätigt):** individuell gestaltbar. Spieler gestalten ihren Schild im **Charakter-Editor**, die Siedlung bekommt im **Lager-Editor** eigene Farben und ein Wappen (Deutung von Claude). Technik: 2–3 Farben als Material-Parameter plus **8–12 eigene Muster-Masken** und ein Wappen-Feld; keine historischen Wappen nachbilden. Dieselben Farben können Umhang und Banner nutzen.

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
| Editor | Muster-Masken (8–12), Wappen-Bausteine, Farbpaletten für Charakter- und Lager-Editor | B |

### Gewaltdarstellung (bestätigt: einstellbar)
Vorschlag für die Menü-Stufen: **Aus** (keine Blutspuren) · **Blut** · **Blut + abtrennbare Körperteile**. Jede Stufe braucht eigene Assets:

| Stufe | Assets | Aufwand |
|---|---|---|
| Blut | Blut-Effekt bei Treffern (Niagara), 4–6 Wund-Decals, Blutspuren am Boden | klein |
| Abtrennbare Körperteile | Schnittstellen an Figuren (Kopf, Arme, Beine), Stumpf-Kappen je Schnittstelle, abgetrennte Teile aus den vorhandenen Modulteilen der Figur, eigener Effekt | **groß**: betrifft jede Figur und jeden Gegnertyp |

Hinweise von Claude:
- Abtrennbare Körperteile sollten ein **Ausbau nach der ersten Kampf-Version** sein. Die modularen Figuren aus dem Asset-Plan helfen, weil Körperteile schon getrennt sind.
- **Altersfreigabe:** Nach meinem Wissen richtet sich die Einstufung nach dem stärksten einstellbaren Inhalt, nicht nach der Voreinstellung. Das sollte vor der Veröffentlichung für das Zielland geprüft werden (für Deutschland USK).

## 8. Mengenabschätzung (MVP Phase 3)
7 Typen im MVP · rund **25 Meshes** (Köpfe, Klingen, Stiele, Zusatzteile) · **3 Textursätze** + Masken · **7 Icons** + 6 Stufenrahmen.
Zum Vergleich: ohne Teile-Strategie wären es 7 Typen × 6 Stufen = 42 Einzelwaffen mit eigenen Texturen.

## 9. Entscheidungen
**Beantwortet am 2026-11-12** (eingetragen in `docs/01_entscheidungen.md`):
1. Stufen-Leiter: nordisch-historisch wie vorgeschlagen.
2. Relikt-Stufe T6: ja.
3. Waffentypen im MVP: die 7 Typen (Antwort „ja“, von Claude als „passend“ gedeutet).
4. Gewaltdarstellung: einstellbar, auch ob Körperteile abgetrennt werden.
5. Schild: individuell gestaltbar im Charakter- und Lager-Editor.
6. Fernkampf: Bogen und Armbrust.

**Nachtrag 2026-11-12:** Armbrust wird über Attribute plus eine Fähigkeit freigeschaltet.

**Noch offen:**
1. Armbrust ab **T3 oder T4** (siehe Abschnitt 3)?
2. Ist „Lager-Editor“ der Editor für Farben und Wappen der **Siedlung**?
3. Abtrennbare Körperteile schon in der ersten Kampf-Version oder als Ausbau danach (Empfehlung: Ausbau)?
4. Ziel-Altersfreigabe (z. B. USK 16 oder 18)?
