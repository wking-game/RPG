# Kreaturen-Daten (erzeugt)

> **Erzeugt** am 2026-11-22 von `tools/data/creatures_from_gemini.py` aus `design/referenz/gemini_kuestenasche_gesamtdokumentation.md`. **Nicht von Hand ändern:** Zuordnungen im Skript anpassen und neu erzeugen. Regeln und Begründungen: `docs/21_kreaturen_level_kampf.md`.
>
> Status: Einordnung, Spawn und Beute-Zuordnung sind **Vorschläge von Claude** (2026-11-22). Die Werte (LP / Haltung / Schaden), Spawn-Gewichte für Nr. 31–48 und die Beute-Chancen sind **Geminis Startwerte**.

## 1. Überblick

| Was | Anzahl |
|---|---|
| Kreaturen | 48 |
| Beute-Einträge bei Gemini | 361 |
| verschiedene Beute-Namen bei Gemini | 279 |
| davon „A & B“-Einträge, die in zwei Gegenstände geteilt werden | 15 |
| Biom-Zusatzbeute, die entfällt (Kreatur kommt in dem Biom nicht vor) | 33 |
| Rang-Beute, die auf den höchsten erreichbaren Rang vorgezogen wird | 19 |
| Beute-Einträge, die auf das Grundteil statt auf eine Güte-Variante zeigen (Entscheidung 2026-11-22) | 39 |
| **Gegenstände nach der Bereinigung** | **162** |
| davon kreaturentypische Teile | 123 |
| davon Biom-Materialien und Knorpel (feste Stufe) | 19 |
| davon allgemeine Teile je Stufe, die Kreaturen geben (Sehne, Knochen, Haut, Horn, Drachensehne; Stufe 1 kommt mit den Tieren dazu) | 20 |

Für jeden dieser Gegenstände braucht es später einen Eintrag in `items.json` und ein Icon.

## 2. Kreaturen nach Modell-Welle

Eine Kreatur kommt nur in ihrem Heimat-Biom und in späteren Biomen vor. Ihr Modell wird mit der Welle ihres Heimat-Bioms gebaut (Entscheidung „Kreaturen: Reihenfolge“, 2026-11-22).

| Welle | Heimat-Biom | Anzahl | Kreaturen |
|---|---|---|---|
| Kern | Fjordwiesen | 0 | keine (Tiere, Waldbanditen, Alter Keiler) |
| Kern (1) | Gezeitenmarsch | 4 | Thornmaw, Mirecrawler, Bramblefang, Siltstrider |
| 2 | Hügelgrab-Heide | 13 | Osskarr, Morrowear, Dustmote, Graveclaw, Hollowback, Shroudwalker, Wiedergänger, Sporeback, Skitterclaw, Gloomstalker, Charmaw, Vaultcrawler, Mossgolem |
| 3 | Basaltfjell | 14 | Vaelgorn, Murghul, Kraggar, Rimestalker, Ironhide, Cragmaw, Troll, Riese, Cragdrake, Rimehound, Slagbrute, Cragstalker, Frostmauler, Stormwyrmling |
| 4 | Glutküste | 10 | Cinderloom, Pyroclast, Dravik, Ashstalker, Cinderhound, Cinderwyrm, Ashwing, Cinderling, Ashweaver, Cindercolossus |
| 5 | Eisrand | 7 | Galeborn, Dreadcolossus, Stormmaw, Frostdrake, Dreadwyrm, Galehound, Galewyrm |

## 3. Einordnung aller 48 Kreaturen

| Nr. | ID | Name (Gemini) | Anzeigename (Vorschlag) | Rolle | MinRang | Heimat | auch in | erreichte Ränge | Welle |
|---|---|---|---|---|---|---|---|---|---|
| 1 | `creature_vaelgorn` | Vaelgorn | Krustenwandler | Krustenwesen mit Mineralplatten | 3 | Basaltfjell | Glutküste | 3–6 | 3 |
| 2 | `creature_cinderloom` | Cinderloom | Glutweber | glimmender Faserweber mit Glutknoten | 4 | Glutküste | – | 4–6 | 4 |
| 3 | `creature_osskarr` | Osskarr | Beinrassler | Knochenwesen mit Gelenkpanzer und Stofffetzen | 3 | Hügelgrab-Heide | Basaltfjell | 3–5 | 2 |
| 4 | `creature_morrowear` | Morrowear | Lauscher | Lauscher mit großem Schwingungsorgan | 3 | Hügelgrab-Heide | Basaltfjell | 3–5 | 2 |
| 5 | `creature_pyroclast` | Pyroclast | Schlackengräber | grabender Schlacke-Panzerträger | 4 | Glutküste | – | 4–6 | 4 |
| 6 | `creature_dravik` | Dravik | Panzergräber | schwer gepanzerter Gräber mit Schuppenplatten | 5 | Glutküste | – | 5–6 | 4 |
| 7 | `creature_murghul` | Murghul | Sichelkrabbler | Chitin-Jäger mit Sichelklauen | 3 | Basaltfjell | – | 3–5 | 3 |
| 8 | `creature_kraggar` | Kraggar | Felsreißer | Fels-Raubtier mit Granithaut | 4 | Basaltfjell | Eisrand | 4–6 | 3 |
| 9 | `creature_galeborn` | Galeborn | Sturmgeborener | Sturmwesen aus Faserbändern mit geladenen Kernen | 5 | Eisrand | – | 5–6 | 5 |
| 10 | `creature_ashstalker` | Ashstalker | Aschepirscher | Asche-Pirscher mit leichtem Balg | 2 | Glutküste | – | 4–6 | 4 |
| 11 | `creature_thornmaw` | Thornmaw | Dornenmaul | Dornenmaul mit verholzter Haut | 2 | Gezeitenmarsch | Hügelgrab-Heide | 2–3 | 1 |
| 12 | `creature_dustmote` | Dustmote | Staubmotte | kleiner, flinker Staubflatterer (Schwarm) | 2 | Hügelgrab-Heide | Glutküste | 2–4 | 2 |
| 13 | `creature_mirecrawler` | Mirecrawler | Schlickkriecher | Schlick-Kriecher mit breiten Greifpratzen | 2 | Gezeitenmarsch | – | 2–3 | 1 |
| 14 | `creature_cinderhound` | Cinderhound | Glutrüde | Glut-Jagdhund im Rudel | 3 | Glutküste | – | 4–6 | 4 |
| 15 | `creature_graveclaw` | Graveclaw | Grabkralle | Grabkrallen-Wühler mit Schutt-Schultern | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 16 | `creature_rimestalker` | Rimestalker | Reifpirscher | kaltgrauer Frost-Pirscher mit Hornspitzen | 4 | Basaltfjell | Eisrand | 4–6 | 3 |
| 17 | `creature_hollowback` | Hollowback | Hohlrücken | Hohlrücken, stößt Staub- und Sporenwolken aus | 4 | Hügelgrab-Heide | Glutküste | 4–6 | 2 |
| 18 | `creature_ironhide` | Ironhide | Eisenhaut | schwerer Eisenkrusten-Panzer | 4 | Basaltfjell | Glutküste | 4–6 | 3 |
| 19 | `creature_shroudwalker` | Shroudwalker | Schleierwandler | Schattenweber der Gewölbe mit glimmendem Kern | 4 | Hügelgrab-Heide | – | 4 | 2 |
| 20 | `creature_cragmaw` | Cragmaw | Felsrachen | Felskiefer-Räuber mit Steinkamm | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 21 | `creature_dreadcolossus` | Dreadcolossus | Schreckenskoloss | uralter Koloss aus Stein und Relikten, sehr selten | 6 | Eisrand | – | 6 | 5 |
| 22 | `creature_troll` | Troll | Troll | Troll mit dicker Haut | 4 | Basaltfjell | – | 4–5 | 3 |
| 23 | `creature_giant` | Riese | Riese | Riese mit Rüstungsresten | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 24 | `creature_revenant` | Wiedergänger | Draugr | Untoter aus den Grabhügeln | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 25 | `creature_cinderwyrm` | Cinderwyrm | Glutwurm | Glut-Drache | 5 | Glutküste | – | 5–6 | 4 |
| 26 | `creature_cragdrake` | Cragdrake | Felsdrache | Fels-Drache der Gipfel | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 27 | `creature_stormmaw` | Stormmaw | Sturmrachen | großer Sturm-Drache | 6 | Eisrand | – | 6 | 5 |
| 28 | `creature_ashwing` | Ashwing | Aschenschwinge | Asche-Gleitdrache | 5 | Glutküste | – | 5–6 | 4 |
| 29 | `creature_frostdrake` | Frostdrake | Frostdrache | Frost-Drache | 5 | Eisrand | – | 5–6 | 5 |
| 30 | `creature_dreadwyrm` | Dreadwyrm | Schreckenswurm | Spitzen-Drache, sehr selten | 6 | Eisrand | – | 6 | 5 |
| 31 | `creature_bramblefang` | Bramblefang | Dornenbeißer | Dornen-Raubtier, schnelle Schnappangriffe, leicht aus dem Tritt zu bringen | 2 | Gezeitenmarsch | Hügelgrab-Heide | 2–3 | 1 |
| 32 | `creature_siltstrider` | Siltstrider | Schlickschreiter | langbeiniger Senkenläufer, stolpert bei Flankentreffern | 2 | Gezeitenmarsch | Hügelgrab-Heide, Glutküste | 2–3 | 1 |
| 33 | `creature_cinderling` | Cinderling | Funkling | kleiner Funkenläufer, flankiert in Gruppen | 2 | Glutküste | – | 2–3 | 4 |
| 34 | `creature_sporeback` | Sporeback | Sporenbuckel | buckliger Schleicher, platzt bei Haltungsbruch zur Sporenwolke | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 35 | `creature_skitterclaw` | Skitterclaw | Mauerkrabbler | vielbeiniger Mauer- und Felskrabbler | 3 | Hügelgrab-Heide | Basaltfjell | 3–4 | 2 |
| 36 | `creature_gloomstalker` | Gloomstalker | Dämmerpirscher | Schattenpirscher, lauert in dunklen Ecken | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 37 | `creature_charmaw` | Charmaw | Kohlenmaul | Glut-Beißer, beißt sich fest | 3 | Hügelgrab-Heide | Glutküste | 3–6 | 2 |
| 38 | `creature_rimehound` | Rimehound | Reifhund | Frost-Rudeljäger, bremst die Ausdauer-Erholung | 4 | Basaltfjell | Eisrand | 4–5 | 3 |
| 39 | `creature_slagbrute` | Slagbrute | Schlackenbrecher | Schlacke-Schläger mit dicker Frontkruste | 4 | Basaltfjell | Glutküste | 4–6 | 3 |
| 40 | `creature_vaultcrawler` | Vaultcrawler | Gewölbekletterer | gepanzerter Gewölbekletterer mit Greifarmen | 4 | Hügelgrab-Heide | – | 4 | 2 |
| 41 | `creature_mossgolem` | Mossgolem | Mooswächter | Wächter aus Moos, Wurzeln und altem Gestein | 4 | Hügelgrab-Heide | Basaltfjell | 4–5 | 2 |
| 42 | `creature_galehound` | Galehound | Sturmhund | Sturm-Läufer, lädt vor Stößen hörbar auf | 5 | Eisrand | – | 5–6 | 5 |
| 43 | `creature_cragstalker` | Cragstalker | Gratjäger | Grat-Jäger mit weiten Sprüngen | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 44 | `creature_ashweaver` | Ashweaver | Aschespinner | Asche-Weber mit Netzen und glimmenden Schwachpunkten | 5 | Glutküste | – | 5–6 | 4 |
| 45 | `creature_frostmauler` | Frostmauler | Frostbrecher | massiger Frost-Brecher mit Bodenstößen | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 46 | `creature_stormwyrmling` | Stormwyrmling | Sturmwürmling | junger Sturm-Drache, Vorbote der Sturm-Spitzen | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 47 | `creature_cindercolossus` | Cindercolossus | Aschekoloss | glühender Schlacke-Koloss, Schwachstellen in den Gelenken | 6 | Glutküste | – | 6 | 4 |
| 48 | `creature_galewyrm` | Galewyrm | Sturmwurm | Spitzen-Sturm-Drache mit Schwingenböen | 6 | Eisrand | – | 6 | 5 |

## 4. Werte je Rang (LP / Haltung / Schaden)

Geminis Basiswerte vor Streuung (85–115 %) und vor Anführer-Stufen. Werte für Ränge, die die Kreatur in unserer Biom-Ordnung nie erreicht, stehen in Klammern.

| Nr. | Name | Rang 2 | Rang 3 | Rang 4 | Rang 5 | Rang 6 |
|---|---|---|---|---|---|---|
| 1 | Vaelgorn | – | 640 / 70 / 19 | 890 / 92 / 24 | 1.220 / 118 / 30 | 1.650 / 145 / 38 |
| 2 | Cinderloom | – | – | 940 / 88 / 25 | 1.280 / 112 / 31 | 1.720 / 138 / 39 |
| 3 | Osskarr | – | 620 / 68 / 19 | 860 / 90 / 24 | 1.180 / 114 / 30 | (1.580 / 140 / 37) |
| 4 | Morrowear | – | 660 / 75 / 18 | 910 / 96 / 23 | 1.240 / 120 / 29 | (1.660 / 148 / 36) |
| 5 | Pyroclast | – | – | 1.080 / 122 / 26 | 1.480 / 152 / 33 | 1.980 / 185 / 41 |
| 6 | Dravik | – | – | – | 1.560 / 155 / 34 | 2.120 / 192 / 43 |
| 7 | Murghul | – | 580 / 58 / 19 | 810 / 76 / 24 | 1.100 / 96 / 30 | (1.480 / 118 / 37) |
| 8 | Kraggar | – | – | 1.120 / 130 / 25 | 1.520 / 160 / 32 | 2.050 / 195 / 40 |
| 9 | Galeborn | – | – | – | 1.420 / 132 / 34 | 1.920 / 165 / 43 |
| 10 | Ashstalker | (390 / 40 / 13) | (580 / 56 / 18) | 800 / 74 / 23 | 1.080 / 94 / 29 | 1.440 / 115 / 36 |
| 11 | Thornmaw | 420 / 45 / 13 | 620 / 62 / 18 | (860 / 82 / 23) | (1.150 / 104 / 29) | (1.540 / 128 / 36) |
| 12 | Dustmote | 260 / 25 / 10 | 390 / 35 / 14 | 550 / 48 / 19 | (750 / 64 / 24) | (1.000 / 78 / 30) |
| 13 | Mirecrawler | 450 / 50 / 12 | 660 / 68 / 17 | (910 / 88 / 22) | (1.220 / 110 / 28) | (1.620 / 135 / 35) |
| 14 | Cinderhound | – | (610 / 60 / 19) | 840 / 78 / 24 | 1.140 / 98 / 30 | 1.520 / 120 / 37 |
| 15 | Graveclaw | – | 680 / 76 / 19 | 940 / 98 / 24 | (1.280 / 124 / 31) | (1.700 / 152 / 38) |
| 16 | Rimestalker | – | – | 890 / 82 / 25 | 1.220 / 104 / 32 | 1.640 / 128 / 40 |
| 17 | Hollowback | – | – | 960 / 96 / 24 | 1.320 / 120 / 30 | 1.780 / 148 / 38 |
| 18 | Ironhide | – | – | 1.150 / 135 / 26 | 1.580 / 168 / 33 | 2.150 / 205 / 41 |
| 19 | Shroudwalker | – | – | 880 / 80 / 25 | (1.200 / 102 / 32) | (1.620 / 126 / 40) |
| 20 | Cragmaw | – | – | – | 1.500 / 148 / 34 | 2.020 / 182 / 42 |
| 21 | Dreadcolossus | – | – | – | – | 3.200 / 255 / 54 |
| 22 | Troll | – | – | 1.140 / 132 / 27 | 1.560 / 164 / 34 | (2.100 / 200 / 42) |
| 23 | Riese | – | – | – | 1.780 / 180 / 37 | 2.450 / 222 / 46 |
| 24 | Wiedergänger | – | 640 / 72 / 19 | 880 / 94 / 24 | (1.200 / 118 / 30) | (1.600 / 145 / 37) |
| 25 | Cinderwyrm | – | – | – | 1.950 / 168 / 38 | 2.750 / 215 / 48 |
| 26 | Cragdrake | – | – | – | 2.050 / 182 / 37 | 2.850 / 230 / 47 |
| 27 | Stormmaw | – | – | – | – | 3.100 / 235 / 52 |
| 28 | Ashwing | – | – | – | 1.880 / 155 / 37 | 2.650 / 198 / 46 |
| 29 | Frostdrake | – | – | – | 2.000 / 175 / 37 | 2.800 / 220 / 47 |
| 30 | Dreadwyrm | – | – | – | – | 3.400 / 265 / 55 |
| 31 | Bramblefang | 410 / 42 / 13 | 610 / 60 / 18 | (840 / 78 / 23) | (1.120 / 98 / 29) | (1.480 / 120 / 36) |
| 32 | Siltstrider | 440 / 48 / 12 | 650 / 68 / 17 | (890 / 88 / 22) | (1.190 / 110 / 28) | (1.560 / 135 / 35) |
| 33 | Cinderling | 280 / 28 / 11 | 410 / 38 / 15 | (580 / 52 / 20) | (790 / 68 / 25) | (1.060 / 85 / 31) |
| 34 | Sporeback | – | 690 / 72 / 19 | 940 / 92 / 24 | (1.260 / 115 / 30) | (1.680 / 142 / 37) |
| 35 | Skitterclaw | – | 560 / 55 / 18 | 780 / 72 / 23 | (1.060 / 90 / 29) | (1.420 / 112 / 36) |
| 36 | Gloomstalker | – | 600 / 62 / 19 | 830 / 80 / 24 | (1.120 / 100 / 30) | (1.500 / 122 / 37) |
| 37 | Charmaw | – | 720 / 82 / 20 | 980 / 102 / 26 | 1.310 / 126 / 32 | 1.740 / 155 / 39 |
| 38 | Rimehound | – | – | 860 / 78 / 25 | 1.180 / 98 / 32 | (1.580 / 120 / 40) |
| 39 | Slagbrute | – | – | 1.100 / 128 / 26 | 1.500 / 158 / 33 | 2.020 / 192 / 41 |
| 40 | Vaultcrawler | – | – | 930 / 95 / 24 | (1.270 / 118 / 30) | (1.700 / 145 / 37) |
| 41 | Mossgolem | – | – | 1.160 / 135 / 25 | 1.580 / 165 / 32 | (2.120 / 200 / 40) |
| 42 | Galehound | – | – | – | 1.220 / 102 / 31 | 1.660 / 126 / 39 |
| 43 | Cragstalker | – | – | – | 1.480 / 145 / 33 | 1.980 / 178 / 41 |
| 44 | Ashweaver | – | – | – | 1.340 / 115 / 31 | 1.800 / 142 / 38 |
| 45 | Frostmauler | – | – | – | 1.650 / 170 / 35 | 2.250 / 210 / 44 |
| 46 | Stormwyrmling | – | – | – | 1.900 / 160 / 36 | 2.620 / 202 / 45 |
| 47 | Cindercolossus | – | – | – | – | 2.800 / 245 / 50 |
| 48 | Galewyrm | – | – | – | – | 2.900 / 215 / 53 |

**Mittelwerte je Rang** (über alle Kreaturen mit Werten für diesen Rang; Grundlage für Tiere und Bosse):

| Rang | LP | Haltung | Schaden | Kreaturen |
|---|---|---|---|---|
| 2 | 379 | 40 | 12 | 7 |
| 3 | 607 | 63 | 18 | 18 |
| 4 | 910 | 93 | 24 | 30 |
| 5 | 1.368 | 127 | 32 | 43 |
| 6 | 1.978 | 166 | 41 | 48 |

## 5. Spawn-Tabellen je Biom

Biom-Rang = Stufe des Bioms; an gefährlichen Orten gilt Rang + 1 (höchstens 6). „Wächst mit (85 %)“: 85 % erreichen den Rang des Ortes, der Rest einen Rang darunter (mindestens MinRang).

### Fjordwiesen (`biome_fjord_meadow`, Rang 1; gefährliche Orte Rang 2: Banditenlager, Wolfshöhlen)

Keine Kreaturen (bestätigt). Tiere: Wildschwein, Rothirsch, Hase, Fuchs, Wolf. Menschen: Waldbanditen. Boss: Der Alte Keiler (realistisch).

### Gezeitenmarsch (`biome_tidal_marsh`, Rang 2; gefährliche Orte Rang 3: Wracks, tiefe Priele)

Dazu Robbe und Seevögel (friedlich), Strandplünderer. Boss: Die Schlickmutter als Moorhexe mit Irrlichtern (`creature_will_o_wisp`).

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Bramblefang | 1,25 | wächst mit (immer) | überall | 2–3 | 1 |
| Thornmaw | 1,00 | wächst mit (85 %) | überall | 2–3 | 1 |
| Mirecrawler | 1,10 | wächst mit (85 %) | überall | 2–3 | 1 |
| Siltstrider | 0,90 | wächst mit (85 %) | überall | 2–3 | 1 |

### Hügelgrab-Heide (`biome_barrow_heath`, Rang 3; gefährliche Orte Rang 4: Grabhügel, Gewölbe, Grabfestung)

Dazu Rothirsch, Hase, Wolf, Bär, Elch; Waldbanditen, Eidbrecher. Boss: Der Hügelkönig als Draugr-Fürst (die Wiedergänger sind sein Gefolge).

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Osskarr | 1,00 | wächst mit (85 %) | überall | 3–4 | 2 |
| Morrowear | 0,80 | wächst mit (85 %) | überall | 3–4 | 2 |
| Graveclaw | 1,00 | wächst mit (85 %) | überall | 3–4 | 2 |
| Wiedergänger | 1,10 | wächst mit (85 %) | überall | 3–4 | 2 |
| Dustmote | 1,00 | wächst mit (85 %) | überall | 2–4 | 2 |
| Sporeback | 1,05 | wächst mit (85 %) | überall | 3–4 | 2 |
| Skitterclaw | 1,15 | wächst mit (85 %) | überall | 3–4 | 2 |
| Gloomstalker | 0,95 | wächst mit (80 %) | überall | 3–4 | 2 |
| Charmaw | 0,70 | wächst mit bis Rang 4 (60 %) | überall | 3–4 | 2 |
| Bramblefang | 0,60 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Siltstrider | 0,50 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Thornmaw | 0,50 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Hollowback | 0,80 | wächst mit (85 %) | nur gefährliche Orte | 4 | 2 |
| Shroudwalker | 0,80 | wächst mit (85 %) | nur gefährliche Orte | 4 | 2 |
| Vaultcrawler | 0,90 | wächst mit (85 %) | nur gefährliche Orte | 4 | 2 |
| Mossgolem | 0,60 | wächst mit (70 %) | nur gefährliche Orte | 4 | 2 |

### Basaltfjell (`biome_basalt_fell`, Rang 4; gefährliche Orte Rang 5: Gipfel, Eishöhlen)

Dazu Wolf, Bär, Rentier, Luchs; Eidbrecher. Boss: Der Felsenvater als Troll (der größte und älteste Troll).

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Vaelgorn | 0,90 | wächst mit (85 %) | überall | 3–5 | 3 |
| Murghul | 0,90 | wächst mit (85 %) | überall | 3–5 | 3 |
| Kraggar | 1,00 | wächst mit (85 %) | überall | 4–5 | 3 |
| Rimestalker | 0,90 | wächst mit (85 %) | überall | 4–5 | 3 |
| Ironhide | 0,80 | wächst mit (85 %) | überall | 4–5 | 3 |
| Troll | 0,50 | wächst mit (85 %) | überall | 4–5 | 3 |
| Rimehound | 1,05 | wächst mit (85 %) | überall | 4–5 | 3 |
| Slagbrute | 0,70 | wächst mit bis Rang 5 (65 %) | überall | 4–5 | 3 |
| Mossgolem | 0,85 | wächst mit (85 %) | überall | 4–5 | 2 |
| Osskarr | 0,60 | wächst mit (85 %) | überall | 3–5 | 2 |
| Morrowear | 0,50 | wächst mit (85 %) | überall | 3–5 | 2 |
| Skitterclaw | 0,70 | fest Rang 3 (Nachzügler) | überall | 3 | 2 |
| Cragmaw | 0,80 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Cragdrake | 0,30 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Cragstalker | 0,85 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Riese | 0,35 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Frostmauler | 0,75 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Stormwyrmling | 0,45 | wächst mit (80 %) | nur gefährliche Orte | 5 | 3 |

### Glutküste (`biome_ember_coast`, Rang 5; gefährliche Orte Rang 6: Lavafelder, Kultstätten)

Keine Tiere; Aschebund-Kult. Boss: Die Aschezunge als Lindwurm.

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Cinderloom | 0,90 | wächst mit (85 %) | überall | 4–6 | 4 |
| Pyroclast | 0,90 | wächst mit (85 %) | überall | 4–6 | 4 |
| Dravik | 0,80 | wächst mit (85 %) | überall | 5–6 | 4 |
| Ashstalker | 1,10 | wächst mit (85 %) | überall | 4–6 | 4 |
| Cinderhound | 1,00 | wächst mit (85 %) | überall | 4–6 | 4 |
| Charmaw | 1,00 | wächst mit (85 %) | überall | 4–6 | 2 |
| Slagbrute | 1,05 | wächst mit (85 %) | überall | 4–6 | 3 |
| Ashweaver | 0,75 | wächst mit (85 %) | überall | 5–6 | 4 |
| Cinderwyrm | 0,35 | wächst mit (85 %) | überall | 5–6 | 4 |
| Ashwing | 0,40 | wächst mit (85 %) | überall | 5–6 | 4 |
| Vaelgorn | 0,60 | wächst mit (85 %) | überall | 4–6 | 3 |
| Ironhide | 0,60 | wächst mit (85 %) | überall | 4–6 | 3 |
| Hollowback | 0,50 | wächst mit (85 %) | überall | 4–6 | 2 |
| Cinderling | 1,30 | wächst mit bis Rang 3 (85 %) | überall | 2–3 | 4 |
| Siltstrider | 0,60 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Dustmote | 0,60 | fest Rang 2 (Nachzügler) | überall | 2 | 2 |
| Cindercolossus | 0,35 | wächst mit (immer) | nur gefährliche Orte | 6 | 4 |

### Eisrand (`biome_ice_rim`, Rang 6; gefährliche Orte Rang 6: Gletscherspalten, gefrorene Wracks)

Dazu Rentier, Polarfuchs; Menschen laut Story. Boss: Der Sturmkoloss als Frostriese. Anführer-Stufen hier am häufigsten.

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Galeborn | 0,90 | wächst mit (85 %) | überall | 5–6 | 5 |
| Galehound | 1,05 | wächst mit (90 %) | überall | 5–6 | 5 |
| Cragstalker | 0,65 | wächst mit (80 %) | überall | 5–6 | 3 |
| Frostmauler | 0,60 | wächst mit (80 %) | überall | 5–6 | 3 |
| Stormwyrmling | 0,55 | wächst mit (85 %) | überall | 5–6 | 3 |
| Frostdrake | 0,35 | wächst mit (85 %) | überall | 5–6 | 5 |
| Stormmaw | 0,20 | wächst mit (immer) | überall | 6 | 5 |
| Galewyrm | 0,35 | wächst mit (immer) | überall | 6 | 5 |
| Dreadcolossus | 0,15 | wächst mit (immer) | überall | 6 | 5 |
| Dreadwyrm | 0,10 | wächst mit (immer) | überall | 6 | 5 |
| Riese | 0,50 | wächst mit (85 %) | überall | 5–6 | 3 |
| Kraggar | 0,50 | wächst mit (85 %) | überall | 5–6 | 3 |
| Rimestalker | 0,60 | wächst mit (85 %) | überall | 5–6 | 3 |
| Cragmaw | 0,40 | wächst mit (85 %) | überall | 5–6 | 3 |
| Cragdrake | 0,25 | wächst mit (85 %) | überall | 5–6 | 3 |
| Rimehound | 0,75 | fest Rang 4 (Nachzügler) | überall | 4 | 3 |

## 6. Beute je Kreatur (bereinigt)

Spalten: **Menge** und **Chance** wie bei Gemini; **ab Rang** = Rang-Sperre; **+Menge** = Mengenbonus (+1 je Rang über „ab Rang“, höchstens +2). Allgemeine Teile (Sehne, Knochen, Haut, Horn, Drachensehne) bekommen die Stufe des Rangs, den die Kreatur beim Tod hat. Biom-Zusatzbeute fällt nur im genannten Biom und wird bei Nachzüglern gedämpft (siehe `docs/21_kreaturen_level_kampf.md`, Abschnitt 10).

### 1. Vaelgorn (`creature_vaelgorn`, MinRang 3, erreicht 3–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Mineral-Krustensplitter | `item_vaelgorn_crust_shard` | 1–2 | 0,90 | 3 | – | Mineral-Krustensplitter |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,75 | 3 | – | Sehnenfasern |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,65 | 3 | – | Knochensplitter |
| Rang | Mineral-Krustensplitter | `item_vaelgorn_crust_shard` | 1–2 | 0,60 | 4 | Ja | Dichtere Krustenplatten |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,50 | 5 | Ja | Verstärkte Sehnenstränge |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,45 | – | – | Gesteinssplitter |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Faser-/Harzreste.

### 2. Cinderloom (`creature_cinderloom`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Verkohlte Faser | `item_charred_fiber` | 1–2 | 0,85 | 4 | – | Verkohlte Faserstränge |
| Grund | Glutknoten | `item_ember_knot` | 1–2 | 0,75 | 4 | – | Kleine Glutknoten |
| Grund | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,65 | 4 | – | Ruinen-/Asche-Reste |
| Rang | Hitzefaser | `item_heat_fiber` | 1–2 | 0,55 | 5 | Ja | Zähe Hitze-Fasern |
| Rang | Stabile Glutkapsel | `item_ember_capsule_stable` | 1 | 0,35 | 6 | Ja | Stabile Glutkapseln |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,50 | – | – | Glut-Schlacke |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Alte Stoff-/Reliktfäden; Moosgeäst: Verkohltes Holz-/Faserstück.

### 3. Osskarr (`creature_osskarr`, MinRang 3, erreicht 3–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Knochengelenk | `item_osskarr_bone_joint` | 1–2 | 0,85 | 3 | – | Knochengelenke |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,75 | 3 | – | Trockene Sehnenbänder |
| Grund | Stofffetzen | `item_cloth_scrap` | 1–2 | 0,65 | 3 | – | Grobe Gewebe-/Stofffetzen |
| Rang | Knochenstrebe | `item_osskarr_bone_strut` | 1–2 | 0,60 | 4 | Ja | Gehärtete Knochenstreben |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,45 | 5 | Ja | Lange Sehnen |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,45 | – | – | Verfall-Relikt |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,40 | – | – | Felsstaub-/Splitterreste |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Waldfaser.

### 4. Morrowear (`creature_morrowear`, MinRang 3, erreicht 3–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schwingungsorgan | `item_morrowear_ear` | 1 | 0,80 | 3 | – | Hör-/Schwingungsorgan |
| Grund | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,80 | 3 | – | Dicke stumpfe Haut |
| Grund | Knorpel | `item_cartilage` | 1–2 | 0,65 | 3 | – | Knorpelreste |
| Rang | Schwingungsorgan | `item_morrowear_ear` | 1–2 | 0,55 | 4 | Ja | Ausgeprägtere Schwingungsorgane |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,45 | 5 | Ja | Zähere Häute (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Ruinenstaub | `item_ruin_dust` | 1–2 | 0,45 | – | – | Staub-/Asche-Korn |
| Biom: Basaltfjell | Knorpel | `item_cartilage` | 1 | 0,40 | – | – | Gebirgs-Knorpel |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Feuchtes Balg-/Lederstück.

### 5. Pyroclast (`creature_pyroclast`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schlackeplatte | `item_slag_plate` | 1–2 | 0,85 | 4 | – | Schlacke-Panzerstücke |
| Grund | Glühgestein | `item_pyroclast_glowstone` | 1–2 | 0,75 | 4 | – | Glühendes Gestein |
| Grund | Grabklaue | `item_digging_claw` | 1–2 | 0,65 | 4 | – | Schwere Grabklauen |
| Rang | Schlackeplatte | `item_slag_plate` | 1–2 | 0,55 | 5 | Ja | Dichte Schlackeplatten |
| Rang | Hitzekern | `item_heat_core` | 1 | 0,35 | 6 | Ja | Hitzebeständige Kerne/Brocken |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,50 | – | – | Schlacke-Kern |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Ruinen-Gestein; Schroffen-Grate: Geröll-Erzrest.

### 6. Dravik (`creature_dravik`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Dravik-Schuppenplatte | `item_dravik_scale_plate` | 1–3 | 0,90 | 5 | – | Dicke Horn-/Schuppenplatten |
| Grund | Grabklaue | `item_digging_claw` | 1–2 | 0,75 | 5 | – | Grabklauen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 5 | – | Zähe Dravik-Sehnen |
| Rang | Dravik-Schuppenplatte | `item_dravik_scale_plate` | 1–2 | 0,50 | 6 | Ja | Schwere Spitzen-Schuppen |
| Rang | Dravik-Hornkern | `item_dravik_horn_core` | 1 | 0,35 | 6 | Ja | Seltene Hochrang-Materialien |
| Biom: Glutküste | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,50 | – | – | Asche-Horn |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein / Schroffen-Grate: Panzer-Schutt.

### 7. Murghul (`creature_murghul`, MinRang 3, erreicht 3–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sichelklaue | `item_murghul_sickle_claw` | 1–2 | 0,85 | 3 | – | Sichel-Greifklauen |
| Grund | Chitin-Plattenstück | `item_murghul_chitin` | 1–2 | 0,75 | 3 | – | Chitin-Plattenstücke |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,65 | 3 | – | Hornstacheln |
| Rang | Sichelklaue | `item_murghul_sickle_claw` | 1–2 | 0,60 | 4 | Ja | Gehärtete Sichelklauen |
| Rang | Chitin-Plattenstück | `item_murghul_chitin` | 1–2 | 0,45 | 5 | Ja | Dickere Chitinpanzer (vorgezogen von R6) |
| Biom: Basaltfjell | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,45 | – | – | Fels-Stachel |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Ruinen-Chitin.

### 8. Kraggar (`creature_kraggar`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Felsklaue | `item_kraggar_rock_claw` | 1–2 | 0,85 | 4 | – | Fels-Klauen |
| Grund | Granithaut | `item_kraggar_granite_hide` | 1–2 | 0,80 | 4 | – | Granit-/Schiefer-Hautstücke |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,65 | 4 | – | Hornkamm-Splitter |
| Rang | Felsklaue | `item_kraggar_rock_claw` | 1–2 | 0,55 | 5 | Ja | Massive Felsklauen |
| Rang | Erzader-Brocken | `item_kraggar_ore_vein` | 1 | 0,35 | 6 | Ja | Gebirgs-/Schmiede-Reste |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,55 | – | – | Hochgebirgs-Stein |
| Biom: Basaltfjell, Eisrand | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,45 | – | – | Kanten-Horn |

### 9. Galeborn (`creature_galeborn`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturmfaser | `item_storm_fiber` | 1–3 | 0,90 | 5 | – | Sturm-Faserbänder |
| Grund | Geladener Kernrest | `item_charged_core` | 1–2 | 0,70 | 5 | – | Geladene Kernreste |
| Grund | Hornspange | `item_galeborn_horn_clasp` | 1–2 | 0,70 | 5 | – | Hornspangen |
| Rang | Sturmfaser | `item_storm_fiber` | 1–2 | 0,50 | 6 | Ja | Dichte Sturm-Fasern |
| Rang | Sturmdrüse | `item_galeborn_storm_gland` | 1 | 0,35 | 6 | Ja | Hochlagen-Komponenten |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1–2 | 0,55 | – | – | Sturm-Faser |
| Biom: Eisrand | Hornspange | `item_galeborn_horn_clasp` | 1 | 0,40 | – | – | Ruinen-Spange |

### 10. Ashstalker (`creature_ashstalker`, MinRang 2, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Aschebalg | `item_ashstalker_pelt` | 1–2 | 0,85 | 2 | – | Asche-Balg/Haut |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,75 | 2 | – | Dünne Sehnen |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1 | 0,65 | 2 | – | Leichte Knochenreste |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,65 | 3 | Ja | Zähere Häute |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,45 | 5 | Ja | Stärkere Sehnen |
| Biom: Glutküste | Verkohlte Faser | `item_charred_fiber` | 1–2 | 0,50 | – | – | Asche-Faser |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Unterholz-Reste; Bruchstein: Trümmer-Staub.

### 11. Thornmaw (`creature_thornmaw`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Dornsplitter | `item_thornmaw_thorn` | 1–2 | 0,85 | 2 | – | Dornen-/Horn-Splitter |
| Grund | Verholzte Haut | `item_thornmaw_bark_hide` | 1–2 | 0,75 | 2 | – | Verholzte Hautstücke |
| Grund | Knorpel | `item_cartilage` | 1 | 0,65 | 2 | – | Kiefer-/Knorpelreste |
| Rang | Dornsplitter | `item_thornmaw_thorn` | 1–2 | 0,65 | 3 | Ja | Dickere Dorn-/Horn-Teile |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,45 | 3 | Ja | Zähere Häute (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Dornholz | `item_thorn_wood` | 1–2 | 0,50 | – | – | Dornen-Holz |

Entfällt (Kreatur kommt dort nicht vor): Schroffen-Grate: Gesteins-Horn.

### 12. Dustmote (`creature_dustmote`, MinRang 2, erreicht 2–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Staubhautlappen | `item_dustmote_skin` | 1–2 | 0,85 | 2 | – | Staubige Hautlappen |
| Grund | Fühlerfaser | `item_dustmote_feeler` | 1–2 | 0,70 | 2 | – | Feine Fühler-/Faserreste |
| Rang | Fühlerfaser | `item_dustmote_feeler` | 1–3 | 0,60 | 3 | Ja | Dichtere Faser-/Staub-Teile |
| Biom: Hügelgrab-Heide, Glutküste | Ruinenstaub | `item_ruin_dust` | 1–2 | 0,45 | – | – | Feiner Staub-/Asche-Rest |

### 13. Mirecrawler (`creature_mirecrawler`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Feuchthaut | `item_mirecrawler_hide` | 1–2 | 0,85 | 2 | – | Zähe Feuchthaut |
| Grund | Greifpratze | `item_mirecrawler_paw` | 1–2 | 0,75 | 2 | – | Breite Greifpratzen |
| Grund | Knorpel | `item_cartilage` | 1 | 0,65 | 2 | – | Knorpelreste |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,65 | 3 | Ja | Dickere Häute |
| Rang | Greifpratze | `item_mirecrawler_paw` | 1–2 | 0,45 | 3 | Ja | Kräftigere Pratzen (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Feuchtleder | `item_damp_leather` | 1–2 | 0,50 | – | – | Feucht-Leder/Schlammrest |

### 14. Cinderhound (`creature_cinderhound`, MinRang 3, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Brandfell | `item_cinderhound_pelt` | 1–2 | 0,85 | 3 | – | Verkohlte Faser-/Fellreste |
| Grund | Funkenasche | `item_spark_ash` | 1–2 | 0,70 | 3 | – | Glimmende Funken-/Glut-Reste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 3 | – | Sehnen |
| Rang | Brandfell | `item_cinderhound_pelt` | 1–2 | 0,55 | 4 | Ja | Zähere Brand-Häute |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,45 | 6 | Ja | Stärkere Sehnen & Glut-Reste (geteilt) |
| Rang | Funkenasche | `item_spark_ash` | 1 | 0,45 | 6 | Ja | Stärkere Sehnen & Glut-Reste (geteilt) |
| Biom: Glutküste | Funkenasche | `item_spark_ash` | 1–2 | 0,50 | – | – | Glut-Funke |
| Biom: Glutküste | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1 | 0,40 | – | – | Asche-Sehne |

### 15. Graveclaw (`creature_graveclaw`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Grabklaue | `item_digging_claw` | 1–2 | 0,85 | 3 | – | Grabklauen |
| Grund | Schutt-Schulterplatte | `item_graveclaw_shoulder` | 1–2 | 0,75 | 3 | – | Erd-/Schutt-Schulterstücke |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,65 | 3 | – | Knochenreste |
| Rang | Grabklaue | `item_digging_claw` | 1–2 | 0,55 | 4 | Ja | Schwere Grabklauen |
| Rang | Schutt-Schulterplatte | `item_graveclaw_shoulder` | 1–2 | 0,45 | 4 | Ja | Gehärtete Schulterplatten (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Eisenerz | `item_iron_ore` | 1–2 | 0,45 | – | – | Grab-Erz/Schuttrest |

### 16. Rimestalker (`creature_rimestalker`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Kaltgraue Haut | `item_rimestalker_hide` | 1–2 | 0,85 | 4 | – | Kalt-graue Hautstücke |
| Grund | Hornspitze | `item_rimestalker_spike` | 1–2 | 0,75 | 4 | – | Scharfe Glieder-/Hornspitzen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 4 | – | Sehnen |
| Rang | Kaltgraue Haut | `item_rimestalker_hide` | 1–2 | 0,55 | 5 | Ja | Dichte Kälte-Häute |
| Rang | Frostdorn | `item_rimestalker_frost_spike` | 1 | 0,35 | 6 | Ja | Hochlagen-Teile |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1–2 | 0,55 | – | – | Frost-Splitter |

### 17. Hollowback (`creature_hollowback`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Hohlrückenschale | `item_hollowback_shell` | 1–2 | 0,85 | 4 | – | Ausgehöhlte Rücken-/Rumpfstücke |
| Grund | Sporenstaub | `item_hollowback_spore_dust` | 1–2 | 0,75 | 4 | – | Staub-/Sporen-/Ruß-Reste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Sehnen |
| Rang | Hohlrückenschale | `item_hollowback_shell` | 1–2 | 0,55 | 5 | Ja | Gehärtete Rumpfstrukturen |
| Rang | Sporensack | `item_hollowback_spore_sac` | 1–2 | 0,40 | 6 | Ja | Dichtere Wolken-/Materialreste |
| Biom: Hügelgrab-Heide, Glutküste | Ruinenstaub | `item_ruin_dust` | 1 | 0,45 | – | – | Umgebungs-Kapsel/Rest |

### 18. Ironhide (`creature_ironhide`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Eisenkrusten-Platte | `item_ironhide_plate` | 1–2 | 0,85 | 4 | – | Eisen-/Krusten-Panzerplatten |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,75 | 4 | – | Schwere Hornreste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Dicke Sehnen |
| Rang | Eisenkrusten-Platte | `item_ironhide_plate` | 1–2 | 0,55 | 5 | Ja | Massive Panzerplatten |
| Rang | Erzkruste | `item_ironhide_ore_crust` | 1–2 | 0,35 | 6 | Ja | Metall-/Mineral-Reste |
| Biom: Basaltfjell, Glutküste | Eisenerz | `item_iron_ore` | 1–2 | 0,45 | – | – | Erz-/Eisen-Kruste |

### 19. Shroudwalker (`creature_shroudwalker`, MinRang 4, erreicht 4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schattengewebe | `item_shadow_weave` | 1–2 | 0,85 | 4 | – | Dunkle Faser-/Gewebebahnen |
| Grund | Schattenkern-Splitter | `item_shadow_core_shard` | 1 | 0,70 | 4 | – | Glimmende Kernreste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 4 | – | Lange Sehnen |
| Rang | Schattengewebe | `item_shadow_weave` | 1–2 | 0,55 | 4 | Ja | Dichtere Schatten-/Faserbahnen (vorgezogen von R5) |
| Rang | Schleier-Relikt | `item_shroud_relic` | 1 | 0,35 | 4 | Ja | Relikt-Teile (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1–2 | 0,50 | – | – | Relikt-Faser |

### 20. Cragmaw (`creature_cragmaw`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Zackenkiefer | `item_cragmaw_jaw` | 1–2 | 0,85 | 5 | – | Zackige Kiefer-/Horn-Teile |
| Grund | Steinkamm | `item_cragmaw_crest` | 1–2 | 0,80 | 5 | – | Steinkamm-Stücke |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,70 | 5 | – | Schwere Vorderlauf-Reste |
| Rang | Kiefer-Trophäe | `item_cragmaw_trophy` | 1–2 | 0,50 | 6 | Ja | Große Kiefer-/Horn-Trophäen |
| Rang | Kammkern | `item_cragmaw_crest_core` | 1 | 0,35 | 6 | Ja | Seltene Rohstoffe |
| Biom: Basaltfjell | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,50 | – | – | Fels-Kieferrest |

### 21. Dreadcolossus (`creature_dreadcolossus`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Koloss-Gestein | `item_colossus_stone` | 2–3 | 0,95 | 6 | – | Massive Stein-/Mineral-/Relikt-Teile |
| Grund | Koloss-Knochen | `item_colossus_bone` | 2–3 | 0,85 | 6 | – | Koloss-Knochen/Sehnen |
| Grund | Koloss-Herzstein | `item_colossus_heartstone` | 1–2 | 0,70 | 6 | – | Spitzen-Materialien |
| Rang | Koloss-Relikt | `item_colossus_relic` | 1–2 | 0,45 | 6 | Ja | Seltener Koloss-Spitzenrest |
| Biom: Eisrand | Koloss-Kern | `item_colossus_core` | 1–2 | 0,55 | – | – | Seltener Koloss-Kern/Rest |

### 22. Troll (`creature_troll`, MinRang 4, erreicht 4–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Trollhaut | `item_troll_hide` | 1–2 | 0,90 | 4 | – | Dicke Trollhaut |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,80 | 4 | – | Grobe Knochen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 4 | – | Zähe Sehnen |
| Rang | Trollhaut | `item_troll_hide` | 1–2 | 0,55 | 5 | Ja | Besonders dicke Häute |
| Rang | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,40 | 5 | Ja | Schwere Knochen (vorgezogen von R6) |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moos-/Erd-Balg; Glut-Fluren / Bruchstein: Grobe Schlacke-/Steinreste.

### 23. Riese (`creature_giant`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Riesenknochen | `item_giant_bone` | 2–3 | 0,90 | 5 | – | Große Knochen-/Sehnenstücke |
| Grund | Riesen-Rüstungsschrott | `item_giant_armor_scrap` | 1–2 | 0,75 | 5 | – | Schwere Rüstungs-/Reliktreste |
| Grund | Riesenzahn | `item_giant_tooth` | 1 | 0,60 | 5 | – | Seltene Rohstoffe |
| Rang | Riesenknochen | `item_giant_bone` | 1–2 | 0,50 | 6 | Ja | Kolossale Knochen-/Sehnen-Teile |
| Rang | Riesen-Relikt | `item_giant_relic` | 1 | 0,35 | 6 | Ja | Spitzen-Relikte |
| Biom: Basaltfjell, Eisrand | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,50 | – | – | Uralter Fels-/Mineralrest |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Altes Metall-/Reliktsück.

### 24. Wiedergänger (`creature_revenant`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Altes Eisenstück | `item_old_iron` | 1–2 | 0,85 | 3 | – | Alte Ausrüstungs-/Metallreste |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,75 | 3 | – | Knochen-/Sehnenmaterial |
| Grund | Verfall-Relikt | `item_decay_relic` | 1–2 | 0,65 | 3 | – | Verfall-Reste |
| Rang | Waffenbruchstück | `item_revenant_weapon_fragment` | 1–2 | 0,55 | 4 | Ja | Bessere Metall-/Waffen-/Rüstungsfragmente |
| Rang | Grabbeigabe | `item_revenant_grave_goods` | 1 | 0,35 | 4 | Ja | Seltenere Relikte (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1–2 | 0,50 | – | – | Altes Eisenstück |

Entfällt (Kreatur kommt dort nicht vor): Glut-Fluren: Asche-Relikt; Moosgeäst: Modriges Gewebe-/Relikt.

### 25. Cinderwyrm (`creature_cinderwyrm`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Glutschuppe | `item_ember_scale` | 1–3 | 0,90 | 5 | – | Glut-Schuppen |
| Grund | Schlackeplatte | `item_slag_plate` | 1–2 | 0,80 | 5 | – | Schlacke-Platten |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Zähe Drachensehnen |
| Grund | Glutdrachenhorn | `item_cinderwyrm_horn` | 1–2 | 0,65 | 5 | – | Hitzebeständige Knochen/Hörner |
| Rang | Glutschuppe | `item_ember_scale` | 1–2 | 0,50 | 6 | Ja | Hochwertige Glut-Schuppen |
| Rang | Glutdrachenherz | `item_cinderwyrm_heart` | 1 | 0,35 | 6 | Ja | Spitzen-Komponenten |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1–2 | 0,55 | – | – | Glut-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein / Schroffen-Grate: Hitze-Horn.

### 26. Cragdrake (`creature_cragdrake`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Felsdrachenschuppe | `item_cragdrake_scale` | 1–3 | 0,90 | 5 | – | Granit-/Fels-Schuppen |
| Grund | Klammerkralle | `item_cragdrake_claw` | 1–2 | 0,80 | 5 | – | Klammerkrallen |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Dicker Drachenknorpel/Sehnen |
| Rang | Felsdrachenschuppe | `item_cragdrake_scale` | 1–2 | 0,50 | 6 | Ja | Schwere Berg-Schuppen |
| Rang | Felsdrachenherz | `item_cragdrake_heart` | 1 | 0,35 | 6 | Ja | Hochgebirgs-/Drachen-Werkstoffe |
| Biom: Basaltfjell, Eisrand | Granitschuppe | `item_granite_scale` | 1–2 | 0,55 | – | – | Granit-Schuppe |
| Biom: Basaltfjell | Klammerkralle | `item_cragdrake_claw` | 1 | 0,45 | – | – | Fels-Klaue |

### 27. Stormmaw (`creature_stormmaw`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturm-Schwingenhaut | `item_storm_wing_hide` | 2–3 | 0,95 | 6 | – | Sturm-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 2–3 | 0,85 | 6 | – | Zähe Drachensehnen |
| Grund | Sturmdrachenhorn | `item_stormmaw_horn` | 1–2 | 0,75 | 6 | – | Gebogene Hörner/Zähne |
| Rang | Sturmdrachenherz | `item_stormmaw_heart` | 1–2 | 0,45 | 6 | Ja | Seltene Sturm-/Relikt-Materialien |
| Biom: Eisrand | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1–2 | 0,60 | – | – | Sturm-Schwingenhaut |
| Biom: Eisrand | Sturmdrachenhorn | `item_stormmaw_horn` | 1 | 0,45 | – | – | Sturm-Horn |

### 28. Ashwing (`creature_ashwing`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Asche-Schwingenhaut | `item_ashwing_wing_hide` | 1–3 | 0,90 | 5 | – | Asche-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Zähe Drachensehnen |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,70 | 5 | – | Staubige Horn-/Knochenreste |
| Rang | Gleit-Schwingenhaut | `item_ashwing_wing_hide_fine` | 1–2 | 0,50 | 6 | Ja | Hochwertige Gleit-Schwingenhäute |
| Rang | Aschedrachenherz | `item_ashwing_heart` | 1 | 0,35 | 6 | Ja | Asche-/Drachen-Teile |
| Biom: Glutküste | Asche-Schwingenhaut | `item_ashwing_wing_hide` | 1–2 | 0,55 | – | – | Asche-Haut |
| Biom: Glutküste | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,45 | – | – | Staub-Klaue |

### 29. Frostdrake (`creature_frostdrake`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Frostdrachenschuppe | `item_frostdrake_scale` | 1–3 | 0,90 | 5 | – | Frost-/Schiefer-Schuppen |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Drachensehnen |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,70 | 5 | – | Kalte Horn-/Knochenreste |
| Rang | Frostdrachenschuppe | `item_frostdrake_scale` | 1–2 | 0,50 | 6 | Ja | Gehärtete Frost-Schuppen |
| Rang | Frostdrachenherz | `item_frostdrake_heart` | 1 | 0,35 | 6 | Ja | Hochlagen-Komponenten |
| Biom: Eisrand | Frostdrachenschuppe | `item_frostdrake_scale` | 1–2 | 0,55 | – | – | Frost-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Schroffen-Grate / Bruchstein: Kalt-Horn.

### 30. Dreadwyrm (`creature_dreadwyrm`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Wyrmschuppe | `item_dreadwyrm_scale` | 2–3 | 0,95 | 6 | – | Spitzen-Drachenschuppen |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 2–3 | 0,85 | 6 | – | Massive Sehnen |
| Grund | Uraltes Wyrmhorn | `item_dreadwyrm_horn` | 1–2 | 0,80 | 6 | – | Uralte Knochen-/Horn- & Relikt-Teile (geteilt) |
| Grund | Wyrm-Relikt | `item_wyrm_relic` | 1 | 0,80 | 6 | – | Uralte Knochen-/Horn- & Relikt-Teile (geteilt) |
| Rang | Wyrmherz | `item_dreadwyrm_heart` | 1–2 | 0,45 | 6 | Ja | Seltene Wyrm-Spitzenrelikte |
| Biom: Eisrand | Wyrm-Relikt | `item_wyrm_relic` | 1–2 | 0,60 | – | – | Uralte Drachen-Reliktreste |

### 31. Bramblefang (`creature_bramblefang`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Dornenbalg | `item_bramblefang_pelt` | 1–2 | 0,90 | 2 | – | Dornen-Balg |
| Grund | Fangzahn | `item_bramblefang_fang` | 1–2 | 0,75 | 2 | – | Scharfe Fangzähne |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 2 | – | Dünne Sehnen |
| Rang | Dornenbalg | `item_bramblefang_pelt` | 1–2 | 0,65 | 3 | Ja | Gehärteter Dornen-Balg |
| Rang | Fangzahn | `item_bramblefang_fang` | 1–2 | 0,45 | 3 | Ja | Verstärkte Fangzähne (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Moosfaser | `item_moss_fiber` | 1–2 | 0,55 | – | – | Moosfaser |
| Biom: Gezeitenmarsch | Dornholz | `item_thorn_wood` | 1 | 0,45 | – | – | Dornen-Holz |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,35 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinenstaub | `item_ruin_dust` | 1–2 | 0,40 | – | – | Ruinen-Staubrest |

### 32. Siltstrider (`creature_siltstrider`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Laufhaut | `item_siltstrider_hide` | 1–2 | 0,85 | 2 | – | Trockene Laufhaut |
| Grund | Lange Laufsehne | `item_siltstrider_sinew` | 1–2 | 0,75 | 2 | – | Lange Laufsehnen |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1 | 0,65 | 2 | – | Leichte Knochenreste |
| Rang | Laufhaut | `item_siltstrider_hide` | 1–2 | 0,65 | 3 | Ja | Zähere Laufhaut & längere Sehnen (geteilt) |
| Rang | Lange Laufsehne | `item_siltstrider_sinew` | 1 | 0,65 | 3 | Ja | Zähere Laufhaut & längere Sehnen (geteilt) |
| Rang | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,45 | 3 | Ja | Gehärtete Laufknochen & dichtere Fasern (geteilt) (vorgezogen von R5) |
| Rang | Lange Laufsehne | `item_siltstrider_sinew` | 1 | 0,45 | 3 | Ja | Gehärtete Laufknochen & dichtere Fasern (geteilt) (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Feuchtleder | `item_damp_leather` | 1 | 0,50 | – | – | Feucht-Leder |
| Biom: Gezeitenmarsch | Pflanzenfasern | `item_fiber` | 1–2 | 0,45 | – | – | Unterholz-Faser |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1 | 0,45 | – | – | Asche-Mineral |
| Biom: Glutküste | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,40 | – | – | Ruß-Balgrest |

### 33. Cinderling (`creature_cinderling`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Funkenasche | `item_spark_ash` | 1–3 | 0,90 | 2 | – | Funken-Asche |
| Grund | Verkohlte Faser | `item_charred_fiber` | 1–2 | 0,70 | 2 | – | Verkohlte Faser |
| Grund | Glimmsplitter | `item_cinderling_ember_shard` | 1 | 0,45 | 2 | – | Glimmender Splitter |
| Rang | Funkenasche | `item_spark_ash` | 1–2 | 0,65 | 3 | Ja | Dichtere Funken-Asche & zähere Fasern (geteilt) |
| Rang | Verkohlte Faser | `item_charred_fiber` | 1 | 0,65 | 3 | Ja | Dichtere Funken-Asche & zähere Fasern (geteilt) |
| Rang | Glutkapsel | `item_ember_capsule` | 1 | 0,40 | 3 | Ja | Glimmende Glutkapsel (vorgezogen von R5) |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,40 | – | – | Schlacke-Kern |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1 | 0,30 | – | – | Glut-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Altes Eisenstück; Bruchstein: Ruinen-Rußrest.

### 34. Sporeback (`creature_sporeback`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sporenkappe | `item_sporeback_cap` | 1–2 | 0,85 | 3 | – | Sporenkappen-Rest |
| Grund | Buckelhaut | `item_sporeback_hide` | 1–2 | 0,75 | 3 | – | Buckelige Rumpfhaut |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1 | 0,65 | 3 | – | Zähe Sehnen |
| Rang | Sporenkapsel | `item_sporeback_spore_capsule` | 1–2 | 0,50 | 4 | Ja | Konzentrierte Sporenkapsel & gehärtete Rumpfhaut (geteilt) |
| Rang | Buckelhaut | `item_sporeback_hide` | 1 | 0,50 | 4 | Ja | Konzentrierte Sporenkapsel & gehärtete Rumpfhaut (geteilt) |
| Rang | Sporen-Essenz | `item_spore_essence` | 1 | 0,30 | 4 | Ja | Seltene Sporen-Essenz (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,40 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinenstaub | `item_ruin_dust` | 1 | 0,35 | – | – | Kalk-/Mauerstaub-Kapsel |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moosfaser; Moosgeäst: Feucht-Leder.

### 35. Skitterclaw (`creature_skitterclaw`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Klammer-Chitin | `item_skitterclaw_chitin` | 1–2 | 0,85 | 3 | – | Klammer-Chitin |
| Grund | Krallenspitze | `item_skitterclaw_claw` | 1–2 | 0,75 | 3 | – | Scharfe Krallenspitzen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 3 | – | Dünne Gliedsehnen |
| Rang | Klammer-Chitin | `item_skitterclaw_chitin` | 1–2 | 0,60 | 4 | Ja | Gehärtetes Klammer-Chitin & dickere Krallen (geteilt) |
| Rang | Krallenspitze | `item_skitterclaw_claw` | 1 | 0,60 | 4 | Ja | Gehärtetes Klammer-Chitin & dickere Krallen (geteilt) |
| Rang | Klammer-Chitin | `item_skitterclaw_chitin` | 1 | 0,35 | 4 | Ja | Seltene Chitin-Platte (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1–2 | 0,50 | – | – | Ruinen-Chitin |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1 | 0,35 | – | – | Altes Eisenstück |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1 | 0,45 | – | – | Hochgebirgs-Stein |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,30 | – | – | Granit-Schuppe |

### 36. Gloomstalker (`creature_gloomstalker`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schattengewebe | `item_shadow_weave` | 1–2 | 0,85 | 3 | – | Schatten-Gewebefetzen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 3 | – | Gedämpfte Sehnen |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1 | 0,60 | 3 | – | Dunkle Knochensplitter |
| Rang | Schattengewebe | `item_shadow_weave` | 1–2 | 0,60 | 4 | Ja | Dichtere Schatten-Bahnen & stärkere Sehnen (geteilt) |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1 | 0,60 | 4 | Ja | Dichtere Schatten-Bahnen & stärkere Sehnen (geteilt) |
| Rang | Schattenkern | `item_shadow_core` | 1 | 0,30 | 4 | Ja | Glimmender Schattenkern (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,45 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1 | 0,35 | – | – | Ruinen-Chitin |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moosfaser; Moosgeäst: Dornen-Holz.

### 37. Charmaw (`creature_charmaw`, MinRang 3, erreicht 3–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Verkohlter Kiefer | `item_charmaw_jaw` | 1 | 0,80 | 3 | – | Verkohlter Kieferknochen |
| Grund | Brandsehne | `item_charmaw_burn_sinew` | 1–2 | 0,75 | 3 | – | Brand-Sehnen |
| Grund | Rußzahn | `item_charmaw_tooth` | 1–2 | 0,70 | 3 | – | Rußige Kieferzähne |
| Rang | Verkohlter Kiefer | `item_charmaw_jaw` | 1–2 | 0,55 | 4 | Ja | Gehärteter Kieferknochen & glimmende Zahnreste (geteilt) |
| Rang | Rußzahn | `item_charmaw_tooth` | 1 | 0,55 | 4 | Ja | Gehärteter Kieferknochen & glimmende Zahnreste (geteilt) |
| Rang | Glutkiefer | `item_charmaw_ember_jaw` | 1 | 0,35 | 6 | Ja | Schwerer Glut-Kiefer |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1 | 0,45 | – | – | Glut-Schuppe |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1 | 0,40 | – | – | Altes Eisenstück |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,35 | – | – | Verfall-Relikt |

### 38. Rimehound (`creature_rimehound`, MinRang 4, erreicht 4–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Kaltborstenfell | `item_rimehound_fur` | 1–2 | 0,85 | 4 | – | Kalt-borstiges Fell |
| Grund | Frostsehne | `item_frost_sinew` | 1–2 | 0,75 | 4 | – | Frost-Sehnen |
| Grund | Raufangzahn | `item_rimehound_fang` | 1–2 | 0,65 | 4 | – | Raue Fangzähne |
| Rang | Kaltborstenfell | `item_rimehound_fur` | 1–2 | 0,55 | 5 | Ja | Dichtes Hochlagen-Fell & kristalline Sehnen (geteilt) |
| Rang | Frostsehne | `item_frost_sinew` | 1 | 0,55 | 5 | Ja | Dichtes Hochlagen-Fell & kristalline Sehnen (geteilt) |
| Rang | Frostkern | `item_frost_core` | 1 | 0,35 | 5 | Ja | Frost-Kernstück (vorgezogen von R6) |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1–2 | 0,55 | – | – | Frost-Splitter |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,40 | – | – | Granit-Schuppe |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1 | 0,45 | – | – | Sturm-Faser |
| Biom: Eisrand | Frostsplitter | `item_frost_shard` | 1 | 0,40 | – | – | Frost-Splitter |

### 39. Slagbrute (`creature_slagbrute`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schlackeplatte | `item_slag_plate` | 1–2 | 0,85 | 4 | – | Schlacke-Frontkruste |
| Grund | Basaltbrocken | `item_basalt_chunk` | 1–3 | 0,80 | 4 | – | Grobe Gesteinsbrocken |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Schwere Sehnen |
| Rang | Schlackeplatte | `item_slag_plate` | 1–2 | 0,50 | 5 | Ja | Dichte Schlacke-Platte & gehärtete Gesteinsbrocken (geteilt) |
| Rang | Basaltbrocken | `item_basalt_chunk` | 1 | 0,50 | 5 | Ja | Dichte Schlacke-Platte & gehärtete Gesteinsbrocken (geteilt) |
| Rang | Hitzekern | `item_heat_core` | 1 | 0,35 | 6 | Ja | Glühender Schlacke-Kern |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,50 | – | – | Schlacke-Kern |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,50 | – | – | Hochgebirgs-Stein |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,35 | – | – | Granit-Schuppe |

### 40. Vaultcrawler (`creature_vaultcrawler`, MinRang 4, erreicht 4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Gewölbeplatte | `item_vaultcrawler_plate` | 1–2 | 0,85 | 4 | – | Gewölbe-Panzerplatten |
| Grund | Greifarm-Gelenk | `item_vaultcrawler_joint` | 1–2 | 0,75 | 4 | – | Greifarm-Gelenke |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Zähe Sehnen |
| Rang | Gewölbeplatte | `item_vaultcrawler_plate` | 1–2 | 0,55 | 4 | Ja | Verstärkte Gewölbe-Platten & gehärtete Greifgelenke (geteilt) (vorgezogen von R5) |
| Rang | Greifarm-Gelenk | `item_vaultcrawler_joint` | 1 | 0,55 | 4 | Ja | Verstärkte Gewölbe-Platten & gehärtete Greifgelenke (geteilt) (vorgezogen von R5) |
| Rang | Gewölbe-Reliktplatte | `item_vaultcrawler_relic_plate` | 1 | 0,35 | 4 | Ja | Seltene Gewölbe-Reliktplatte (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1–2 | 0,55 | – | – | Ruinen-Chitin |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1 | 0,45 | – | – | Altes Eisenstück |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,40 | – | – | Verfall-Relikt |

### 41. Mossgolem (`creature_mossgolem`, MinRang 4, erreicht 4–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Wurzelstein | `item_mossgolem_rootstone` | 1–3 | 0,90 | 4 | – | Verwachsene Wurzel-/Steinbrocken |
| Grund | Moosrinde | `item_mossgolem_bark` | 1–2 | 0,80 | 4 | – | Moosdurchsetzte Rinde |
| Grund | Wurzelbast | `item_mossgolem_bast` | 1–2 | 0,65 | 4 | – | Zähes Bindematerial |
| Rang | Uralter Wurzelkern | `item_mossgolem_root_core` | 1–2 | 0,45 | 5 | Ja | Uralte Wurzel-/Steinkerne & dichtere Rindenplatten (geteilt) |
| Rang | Moosrinde | `item_mossgolem_bark` | 1 | 0,45 | 5 | Ja | Uralte Wurzel-/Steinkerne & dichtere Rindenplatten (geteilt) |
| Rang | Wächterkern | `item_mossgolem_warden_core` | 1 | 0,30 | 5 | Ja | Seltener Wächter-Kern (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,40 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1 | 0,35 | – | – | Ruinen-Chitin |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moosfaser; Moosgeäst: Dornen-Holz.

### 42. Galehound (`creature_galehound`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturmborstenfell | `item_galehound_fur` | 1–2 | 0,85 | 5 | – | Sturm-Borstenfell |
| Grund | Böensehne | `item_galehound_sinew` | 1–2 | 0,75 | 5 | – | Gespannte Böen-Sehnen |
| Grund | Geladener Kernrest | `item_charged_core` | 1 | 0,55 | 5 | – | Flackernde Funken-/Spannungsreste |
| Rang | Geladener Kernrest | `item_charged_core` | 1–2 | 0,45 | 6 | Ja | Geladener Kernrest & hochgespannte Sehnen (geteilt) |
| Rang | Böensehne | `item_galehound_sinew` | 1 | 0,45 | 6 | Ja | Geladener Kernrest & hochgespannte Sehnen (geteilt) |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1–2 | 0,55 | – | – | Sturm-Faser |
| Biom: Eisrand | Geladener Kernrest | `item_charged_core` | 1 | 0,40 | – | – | Geladener Kernrest |

Entfällt (Kreatur kommt dort nicht vor): Schroffen-Grate: Frost-Splitter; Schroffen-Grate: Hochgebirgs-Stein.

### 43. Cragstalker (`creature_cragstalker`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Gratschuppenhaut | `item_cragstalker_hide` | 1–2 | 0,85 | 5 | – | Grat-Schuppenhaut |
| Grund | Sprungsehne | `item_cragstalker_sinew` | 1–2 | 0,75 | 5 | – | Kräftige Sprungsehnen |
| Grund | Kammhorn | `item_cragstalker_horn` | 1–2 | 0,65 | 5 | – | Gebogene Kammhörner |
| Rang | Gratschuppenhaut | `item_cragstalker_hide` | 1–2 | 0,35 | 6 | Ja | Gehärtete Grat-Schuppen & seltene Hochgebirgs-Trophäe (geteilt) |
| Rang | Grat-Trophäe | `item_cragstalker_trophy` | 1 | 0,35 | 6 | Ja | Gehärtete Grat-Schuppen & seltene Hochgebirgs-Trophäe (geteilt) |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1–2 | 0,55 | – | – | Granit-Schuppe |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,50 | – | – | Hochgebirgs-Stein |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1 | 0,45 | – | – | Sturm-Faser |
| Biom: Eisrand | Granitschuppe | `item_granite_scale` | 1 | 0,40 | – | – | Granit-Schuppe |

### 44. Ashweaver (`creature_ashweaver`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Hitzefaser | `item_heat_fiber` | 2–3 | 0,90 | 5 | – | Zähe Asche-Faserstränge |
| Grund | Glutknoten | `item_ember_knot` | 1–2 | 0,70 | 5 | – | Glimmende Glutknoten |
| Grund | Verkohltes Chitin | `item_ashweaver_chitin` | 1–2 | 0,65 | 5 | – | Verkohlte Chitin-/Faserreste |
| Rang | Hitzefaser | `item_heat_fiber` | 1–2 | 0,50 | 6 | Ja | Hochfester Glut-Faserstrang |
| Rang | Stabile Glutkapsel | `item_ember_capsule_stable` | 1 | 0,35 | 6 | Ja | Seltene Glutkapsel |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1 | 0,45 | – | – | Glut-Schuppe |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,45 | – | – | Schlacke-Kern |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Verfall-Relikt; Bruchstein: Ruinen-Chitin.

### 45. Frostmauler (`creature_frostmauler`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Frostschuppenbrocken | `item_frostmauler_scale` | 1–2 | 0,85 | 5 | – | Dicke Frost-Schuppenbrocken |
| Grund | Prankenknochen | `item_frostmauler_paw_bone` | 1–2 | 0,75 | 5 | – | Massive Prankenknochen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 5 | – | Zähe Sehnen |
| Rang | Frostschuppenbrocken | `item_frostmauler_scale` | 1–2 | 0,45 | 6 | Ja | Gehärteter Frost-Panzer |
| Rang | Hochlagen-Knochen | `item_frostmauler_rare_bone` | 1 | 0,35 | 6 | Ja | Seltener Hochlagen-Knochen |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1–2 | 0,60 | – | – | Frost-Splitter |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1–2 | 0,50 | – | – | Granit-Schuppe |
| Biom: Eisrand | Frostsplitter | `item_frost_shard` | 1 | 0,45 | – | – | Frost-Splitter |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1 | 0,40 | – | – | Sturm-Faser |

### 46. Stormwyrmling (`creature_stormwyrmling`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Junge Sturm-Schwingenhaut | `item_stormwyrmling_wing_hide` | 1–2 | 0,85 | 5 | – | Junge Sturm-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,80 | 5 | – | Elastische Drachensehnen |
| Grund | Schmales Drachenhorn | `item_stormwyrmling_horn` | 1–2 | 0,65 | 5 | – | Schmale Drachenhörner |
| Rang | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1–2 | 0,50 | 6 | Ja | Verdichtete Sturm-Schwingenhaut |
| Rang | Geladener Drachenkern | `item_charged_dragon_core` | 1 | 0,35 | 6 | Ja | Geladener Drachenkernrest |
| Biom: Eisrand | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1 | 0,50 | – | – | Sturm-Schwingenhaut |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1–2 | 0,50 | – | – | Sturm-Faser |
| Biom: Eisrand | Geladener Kernrest | `item_charged_core` | 1 | 0,35 | – | – | Geladener Kernrest |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,40 | – | – | Granit-Schuppe |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1 | 0,40 | – | – | Frost-Splitter |

### 47. Cindercolossus (`creature_cindercolossus`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schlacke-Kolossplatte | `item_colossus_slag_plate` | 2–3 | 0,95 | 6 | – | Massive Schlacke-Kolossplatte |
| Grund | Glühender Kolosskern | `item_colossus_core_glowing` | 1–2 | 0,75 | 6 | – | Glühender Koloss-Kern |
| Grund | Koloss-Gestein | `item_colossus_stone` | 2–3 | 0,80 | 6 | – | Schwere Gesteins-/Knochenmasse |
| Rang | Schlacke-Relikt | `item_colossus_slag_relic` | 1–2 | 0,45 | 6 | Ja | Seltener Spitzen-Schlacke-Relikt |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1–2 | 0,60 | – | – | Schlacke-Kern |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1–2 | 0,55 | – | – | Glut-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Verfall-Relikt; Bruchstein: Altes Eisenstück.

### 48. Galewyrm (`creature_galewyrm`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturmwyrm-Schwingenhaut | `item_galewyrm_wing_hide` | 2–3 | 0,95 | 6 | – | Spitzen-Sturm-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 2–3 | 0,85 | 6 | – | Massive Drachensehnen |
| Grund | Sturmwyrm-Horn | `item_galewyrm_horn` | 1–2 | 0,75 | 6 | – | Gebogene Sturm-Drachenhörner |
| Rang | Sturmwyrm-Kern | `item_galewyrm_core` | 1–2 | 0,45 | 6 | Ja | Seltener Sturm-Wyrm-Kern |
| Biom: Eisrand | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1–2 | 0,65 | – | – | Sturm-Schwingenhaut |
| Biom: Eisrand | Geladener Kernrest | `item_charged_core` | 1–2 | 0,55 | – | – | Geladener Kernrest |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 2–3 | 0,60 | – | – | Sturm-Faser |

## 7. Gegenstands-Katalog

### 7.1 Allgemeine Teile je Stufe

Stufe = Rang der Kreatur bzw. des Tiers beim Tod. Auch Tiere geben diese Teile (Abschnitt 8 in Dokument 21).

| Teil | IDs | Namen (Stufe 1–6) | Art |
|---|---|---|---|
| Sehne | `item_sinew_t1`, `item_sinew_t2`, `item_sinew_t3`, `item_sinew_t4`, `item_sinew_t5`, `item_sinew_t6` | Dünne Sehne · Zähe Sehne · Kräftige Sehne · Schwere Sehne · Gehärtete Sehne · Uralte Sehne | Bindung |
| Knochen | `item_bone_t1`, `item_bone_t2`, `item_bone_t3`, `item_bone_t4`, `item_bone_t5`, `item_bone_t6` | Leichte Knochen · Knochen · Kräftige Knochen · Schwere Knochen · Gehärtete Knochen · Uralte Knochen | Waffe/Werkzeug |
| Haut | `item_hide_t1`, `item_hide_t2`, `item_hide_t3`, `item_hide_t4`, `item_hide_t5`, `item_hide_t6` | Dünne Haut · Zähe Haut · Dicke Haut · Schwere Haut · Gehärtete Haut · Uralte Haut | Rüstung |
| Horn | `item_horn_t1`, `item_horn_t2`, `item_horn_t3`, `item_horn_t4`, `item_horn_t5`, `item_horn_t6` | Hornsplitter · Horn · Kräftiges Horn · Schweres Horn · Gehärtetes Horn · Uraltes Horn | Waffe/Werkzeug |
| Drachensehne | `item_dragon_sinew_t5`, `item_dragon_sinew_t6` | Drachensehne · Massive Drachensehne | Bindung |

### 7.2 Biom-Materialien und Knorpel (feste Stufe)

| ID | Name | Stufe | Art | Herkunft | Kreaturen |
|---|---|---|---|---|---|
| `item_ash_mineral` | Asche-Mineral | 5 | Veredelung | Glutküste, auch abbaubar | Vaelgorn, Cinderloom, Siltstrider, Charmaw, Slagbrute, Ashweaver |
| `item_basalt_chunk` | Basaltbrocken | 4 | Bau/Relikt | Basaltfjell, auch abbaubar | Vaelgorn, Osskarr, Kraggar, Riese, Skitterclaw, Slagbrute, Cragstalker |
| `item_cartilage` | Knorpel | 1 | Essen | Zutat für Brühen | Morrowear, Thornmaw, Mirecrawler |
| `item_charged_core` | Geladener Kernrest | 6 | Veredelung | Eisrand | Galeborn, Galehound, Stormwyrmling, Galewyrm |
| `item_damp_leather` | Feuchtleder | 2 | Rüstung | Gezeitenmarsch | Mirecrawler, Siltstrider |
| `item_decay_relic` | Verfall-Relikt | 3 | Bau/Relikt | Hügelgrab-Heide, auch in Ruinen | Osskarr, Shroudwalker, Wiedergänger, Bramblefang, Sporeback, Gloomstalker, Charmaw, Vaultcrawler, Mossgolem |
| `item_ember_scale` | Glutschuppe | 5 | Rüstung | Glutküste | Cinderwyrm, Cinderling, Charmaw, Ashweaver, Cindercolossus |
| `item_fiber` | Pflanzenfasern | 1 | Bindung | gibt es schon in `items.json` | Siltstrider |
| `item_frost_shard` | Frostsplitter | 4 | Veredelung | Basaltfjell und Eisrand | Rimestalker, Rimehound, Frostmauler, Stormwyrmling |
| `item_granite_scale` | Granitschuppe | 4 | Rüstung | Basaltfjell, auch sammelbar | Cragdrake, Skitterclaw, Rimehound, Slagbrute, Cragstalker, Frostmauler, Stormwyrmling |
| `item_iron_ore` | Eisenerz | 3 | Bau/Relikt | Metall-Leiter, auch abbaubar | Graveclaw, Ironhide |
| `item_moss_fiber` | Moosfaser | 2 | Bindung | Gezeitenmarsch, auch sammelbar | Bramblefang |
| `item_old_iron` | Altes Eisenstück | 3 | Bau/Relikt | Hügelgrab-Heide, einschmelzbar | Wiedergänger, Skitterclaw, Charmaw, Vaultcrawler |
| `item_ruin_chitin` | Ruinen-Chitin | 3 | Rüstung | Hügelgrab-Heide | Skitterclaw, Gloomstalker, Vaultcrawler, Mossgolem |
| `item_ruin_dust` | Ruinenstaub | 3 | Bau/Relikt | Hügelgrab-Heide, für Mörtel und Alchemie | Morrowear, Dustmote, Hollowback, Bramblefang, Sporeback |
| `item_slag_core` | Schlackekern | 5 | Veredelung | Glutküste | Cinderloom, Pyroclast, Cinderling, Slagbrute, Ashweaver, Cindercolossus |
| `item_storm_fiber` | Sturmfaser | 6 | Bindung | Eisrand | Galeborn, Rimehound, Galehound, Cragstalker, Frostmauler, Stormwyrmling, Galewyrm |
| `item_storm_wing_hide` | Sturm-Schwingenhaut | 6 | Rüstung | Eisrand | Stormmaw, Stormwyrmling, Galewyrm |
| `item_thorn_wood` | Dornholz | 2 | Bau/Relikt | Gezeitenmarsch, auch sammelbar | Thornmaw, Bramblefang |

### 7.3 Kreaturentypische Teile

Stufe = niedrigster Rang, ab dem der Teil fallen kann.

| ID | Name | Stufe | Art | Kreaturen |
|---|---|---|---|---|
| `item_ashstalker_pelt` | Aschebalg | 2 | Rüstung | Ashstalker |
| `item_bramblefang_fang` | Fangzahn | 2 | Waffe/Werkzeug | Bramblefang |
| `item_bramblefang_pelt` | Dornenbalg | 2 | Rüstung | Bramblefang |
| `item_charred_fiber` | Verkohlte Faser | 2 | Bindung | Cinderloom, Ashstalker, Cinderling |
| `item_cinderling_ember_shard` | Glimmsplitter | 2 | Veredelung | Cinderling |
| `item_dustmote_feeler` | Fühlerfaser | 2 | Bindung | Dustmote |
| `item_dustmote_skin` | Staubhautlappen | 2 | Rüstung | Dustmote |
| `item_mirecrawler_hide` | Feuchthaut | 2 | Rüstung | Mirecrawler |
| `item_mirecrawler_paw` | Greifpratze | 2 | Waffe/Werkzeug | Mirecrawler |
| `item_siltstrider_hide` | Laufhaut | 2 | Rüstung | Siltstrider |
| `item_siltstrider_sinew` | Lange Laufsehne | 2 | Bindung | Siltstrider |
| `item_spark_ash` | Funkenasche | 2 | Veredelung | Cinderhound, Cinderling |
| `item_thornmaw_bark_hide` | Verholzte Haut | 2 | Rüstung | Thornmaw |
| `item_thornmaw_thorn` | Dornsplitter | 2 | Waffe/Werkzeug | Thornmaw |
| `item_charmaw_burn_sinew` | Brandsehne | 3 | Bindung | Charmaw |
| `item_charmaw_jaw` | Verkohlter Kiefer | 3 | Waffe/Werkzeug | Charmaw |
| `item_charmaw_tooth` | Rußzahn | 3 | Waffe/Werkzeug | Charmaw |
| `item_cinderhound_pelt` | Brandfell | 3 | Rüstung | Cinderhound |
| `item_cloth_scrap` | Stofffetzen | 3 | Bindung | Osskarr |
| `item_digging_claw` | Grabklaue | 3 | Waffe/Werkzeug | Pyroclast, Dravik, Graveclaw |
| `item_ember_capsule` | Glutkapsel | 3 | Veredelung | Cinderling |
| `item_graveclaw_shoulder` | Schutt-Schulterplatte | 3 | Rüstung | Graveclaw |
| `item_morrowear_ear` | Schwingungsorgan | 3 | Veredelung | Morrowear |
| `item_murghul_chitin` | Chitin-Plattenstück | 3 | Rüstung | Murghul |
| `item_murghul_sickle_claw` | Sichelklaue | 3 | Waffe/Werkzeug | Murghul |
| `item_osskarr_bone_joint` | Knochengelenk | 3 | Waffe/Werkzeug | Osskarr |
| `item_shadow_weave` | Schattengewebe | 3 | Rüstung | Shroudwalker, Gloomstalker |
| `item_skitterclaw_chitin` | Klammer-Chitin | 3 | Rüstung | Skitterclaw |
| `item_skitterclaw_claw` | Krallenspitze | 3 | Waffe/Werkzeug | Skitterclaw |
| `item_sporeback_cap` | Sporenkappe | 3 | Veredelung | Sporeback |
| `item_sporeback_hide` | Buckelhaut | 3 | Rüstung | Sporeback |
| `item_vaelgorn_crust_shard` | Mineral-Krustensplitter | 3 | Rüstung | Vaelgorn |
| `item_cragdrake_claw` | Klammerkralle | 4 | Waffe/Werkzeug | Cragdrake |
| `item_ember_knot` | Glutknoten | 4 | Veredelung | Cinderloom, Ashweaver |
| `item_frost_sinew` | Frostsehne | 4 | Bindung | Rimehound |
| `item_hollowback_shell` | Hohlrückenschale | 4 | Rüstung | Hollowback |
| `item_hollowback_spore_dust` | Sporenstaub | 4 | Veredelung | Hollowback |
| `item_ironhide_plate` | Eisenkrusten-Platte | 4 | Rüstung | Ironhide |
| `item_kraggar_granite_hide` | Granithaut | 4 | Rüstung | Kraggar |
| `item_kraggar_rock_claw` | Felsklaue | 4 | Waffe/Werkzeug | Kraggar |
| `item_mossgolem_bark` | Moosrinde | 4 | Rüstung | Mossgolem |
| `item_mossgolem_bast` | Wurzelbast | 4 | Bindung | Mossgolem |
| `item_mossgolem_rootstone` | Wurzelstein | 4 | Bau/Relikt | Mossgolem |
| `item_osskarr_bone_strut` | Knochenstrebe | 4 | Waffe/Werkzeug | Osskarr |
| `item_pyroclast_glowstone` | Glühgestein | 4 | Veredelung | Pyroclast |
| `item_revenant_grave_goods` | Grabbeigabe | 4 | Bau/Relikt | Wiedergänger |
| `item_revenant_weapon_fragment` | Waffenbruchstück | 4 | Bau/Relikt | Wiedergänger |
| `item_rimehound_fang` | Raufangzahn | 4 | Waffe/Werkzeug | Rimehound |
| `item_rimehound_fur` | Kaltborstenfell | 4 | Rüstung | Rimehound |
| `item_rimestalker_hide` | Kaltgraue Haut | 4 | Rüstung | Rimestalker |
| `item_rimestalker_spike` | Hornspitze | 4 | Waffe/Werkzeug | Rimestalker |
| `item_shadow_core` | Schattenkern | 4 | Veredelung | Gloomstalker |
| `item_shadow_core_shard` | Schattenkern-Splitter | 4 | Veredelung | Shroudwalker |
| `item_shroud_relic` | Schleier-Relikt | 4 | Bau/Relikt | Shroudwalker |
| `item_slag_plate` | Schlackeplatte | 4 | Rüstung | Pyroclast, Cinderwyrm, Slagbrute |
| `item_spore_essence` | Sporen-Essenz | 4 | Veredelung | Sporeback |
| `item_sporeback_spore_capsule` | Sporenkapsel | 4 | Veredelung | Sporeback |
| `item_troll_hide` | Trollhaut | 4 | Rüstung | Troll |
| `item_vaultcrawler_joint` | Greifarm-Gelenk | 4 | Waffe/Werkzeug | Vaultcrawler |
| `item_vaultcrawler_plate` | Gewölbeplatte | 4 | Rüstung | Vaultcrawler |
| `item_vaultcrawler_relic_plate` | Gewölbe-Reliktplatte | 4 | Bau/Relikt | Vaultcrawler |
| `item_ashweaver_chitin` | Verkohltes Chitin | 5 | Rüstung | Ashweaver |
| `item_ashwing_wing_hide` | Asche-Schwingenhaut | 5 | Rüstung | Ashwing |
| `item_cinderwyrm_horn` | Glutdrachenhorn | 5 | Waffe/Werkzeug | Cinderwyrm |
| `item_cragdrake_scale` | Felsdrachenschuppe | 5 | Rüstung | Cragdrake |
| `item_cragmaw_crest` | Steinkamm | 5 | Rüstung | Cragmaw |
| `item_cragmaw_jaw` | Zackenkiefer | 5 | Waffe/Werkzeug | Cragmaw |
| `item_cragstalker_hide` | Gratschuppenhaut | 5 | Rüstung | Cragstalker |
| `item_cragstalker_horn` | Kammhorn | 5 | Waffe/Werkzeug | Cragstalker |
| `item_cragstalker_sinew` | Sprungsehne | 5 | Bindung | Cragstalker |
| `item_dravik_scale_plate` | Dravik-Schuppenplatte | 5 | Rüstung | Dravik |
| `item_frost_core` | Frostkern | 5 | Veredelung | Rimehound |
| `item_frostdrake_scale` | Frostdrachenschuppe | 5 | Rüstung | Frostdrake |
| `item_frostmauler_paw_bone` | Prankenknochen | 5 | Waffe/Werkzeug | Frostmauler |
| `item_frostmauler_scale` | Frostschuppenbrocken | 5 | Rüstung | Frostmauler |
| `item_galeborn_horn_clasp` | Hornspange | 5 | Waffe/Werkzeug | Galeborn |
| `item_galehound_fur` | Sturmborstenfell | 5 | Rüstung | Galehound |
| `item_galehound_sinew` | Böensehne | 5 | Bindung | Galehound |
| `item_giant_armor_scrap` | Riesen-Rüstungsschrott | 5 | Bau/Relikt | Riese |
| `item_giant_bone` | Riesenknochen | 5 | Waffe/Werkzeug | Riese |
| `item_giant_tooth` | Riesenzahn | 5 | Waffe/Werkzeug | Riese |
| `item_heat_fiber` | Hitzefaser | 5 | Bindung | Cinderloom, Ashweaver |
| `item_mossgolem_root_core` | Uralter Wurzelkern | 5 | Veredelung | Mossgolem |
| `item_mossgolem_warden_core` | Wächterkern | 5 | Veredelung | Mossgolem |
| `item_stormwyrmling_horn` | Schmales Drachenhorn | 5 | Waffe/Werkzeug | Stormwyrmling |
| `item_stormwyrmling_wing_hide` | Junge Sturm-Schwingenhaut | 5 | Rüstung | Stormwyrmling |
| `item_ashwing_heart` | Aschedrachenherz | 6 | Veredelung | Ashwing |
| `item_ashwing_wing_hide_fine` | Gleit-Schwingenhaut | 6 | Rüstung | Ashwing |
| `item_charged_dragon_core` | Geladener Drachenkern | 6 | Veredelung | Stormwyrmling |
| `item_charmaw_ember_jaw` | Glutkiefer | 6 | Waffe/Werkzeug | Charmaw |
| `item_cinderwyrm_heart` | Glutdrachenherz | 6 | Veredelung | Cinderwyrm |
| `item_colossus_bone` | Koloss-Knochen | 6 | Waffe/Werkzeug | Dreadcolossus |
| `item_colossus_core` | Koloss-Kern | 6 | Veredelung | Dreadcolossus |
| `item_colossus_core_glowing` | Glühender Kolosskern | 6 | Veredelung | Cindercolossus |
| `item_colossus_heartstone` | Koloss-Herzstein | 6 | Veredelung | Dreadcolossus |
| `item_colossus_relic` | Koloss-Relikt | 6 | Bau/Relikt | Dreadcolossus |
| `item_colossus_slag_plate` | Schlacke-Kolossplatte | 6 | Rüstung | Cindercolossus |
| `item_colossus_slag_relic` | Schlacke-Relikt | 6 | Bau/Relikt | Cindercolossus |
| `item_colossus_stone` | Koloss-Gestein | 6 | Bau/Relikt | Dreadcolossus, Cindercolossus |
| `item_cragdrake_heart` | Felsdrachenherz | 6 | Veredelung | Cragdrake |
| `item_cragmaw_crest_core` | Kammkern | 6 | Veredelung | Cragmaw |
| `item_cragmaw_trophy` | Kiefer-Trophäe | 6 | Bau/Relikt | Cragmaw |
| `item_cragstalker_trophy` | Grat-Trophäe | 6 | Bau/Relikt | Cragstalker |
| `item_dravik_horn_core` | Dravik-Hornkern | 6 | Veredelung | Dravik |
| `item_dreadwyrm_heart` | Wyrmherz | 6 | Veredelung | Dreadwyrm |
| `item_dreadwyrm_horn` | Uraltes Wyrmhorn | 6 | Waffe/Werkzeug | Dreadwyrm |
| `item_dreadwyrm_scale` | Wyrmschuppe | 6 | Rüstung | Dreadwyrm |
| `item_ember_capsule_stable` | Stabile Glutkapsel | 6 | Veredelung | Cinderloom, Ashweaver |
| `item_frostdrake_heart` | Frostdrachenherz | 6 | Veredelung | Frostdrake |
| `item_frostmauler_rare_bone` | Hochlagen-Knochen | 6 | Waffe/Werkzeug | Frostmauler |
| `item_galeborn_storm_gland` | Sturmdrüse | 6 | Veredelung | Galeborn |
| `item_galewyrm_core` | Sturmwyrm-Kern | 6 | Veredelung | Galewyrm |
| `item_galewyrm_horn` | Sturmwyrm-Horn | 6 | Waffe/Werkzeug | Galewyrm |
| `item_galewyrm_wing_hide` | Sturmwyrm-Schwingenhaut | 6 | Rüstung | Galewyrm |
| `item_giant_relic` | Riesen-Relikt | 6 | Bau/Relikt | Riese |
| `item_heat_core` | Hitzekern | 6 | Veredelung | Pyroclast, Slagbrute |
| `item_hollowback_spore_sac` | Sporensack | 6 | Veredelung | Hollowback |
| `item_ironhide_ore_crust` | Erzkruste | 6 | Bau/Relikt | Ironhide |
| `item_kraggar_ore_vein` | Erzader-Brocken | 6 | Bau/Relikt | Kraggar |
| `item_rimestalker_frost_spike` | Frostdorn | 6 | Waffe/Werkzeug | Rimestalker |
| `item_stormmaw_heart` | Sturmdrachenherz | 6 | Veredelung | Stormmaw |
| `item_stormmaw_horn` | Sturmdrachenhorn | 6 | Waffe/Werkzeug | Stormmaw |
| `item_wyrm_relic` | Wyrm-Relikt | 6 | Bau/Relikt | Dreadwyrm |

