# Gesamtliste: Gegenstände, Assets und Inhalte (Stand 2026-11-13)

*Sammelliste über das ganze Spiel. Für den Nutzer, ChatGPT (Design) und Claude (Umsetzung).*
*Details stehen in den Einzellisten; diese Liste verweist darauf, statt sie zu wiederholen:*
`design/ASSET_PLAN.md` (Phase-1-Slice) · `design/ASSET_LIST_GEGENSTAENDE.md` · `design/ASSET_LIST_WAFFEN.md` · `design/ASSET_LIST_RUESTUNG.md` · Werte: `game/Content/Data/*.json`

## 0. Legende
**Status:** ✔ bestätigt (steht in `docs/01_entscheidungen.md`) · ◐ im Design-Register vorgeschlagen, noch nicht bestätigt · ○ neuer Vorschlag aus dieser Liste
**Phase:** ab wann gebraucht (1 Vertical Slice · 2 Siedlung · 3 Kampf · 4 Welt · 5 Spätspiel)
**Art:** M = 3D-Modell · F = Figur mit Skelett · A = Animation · I = nur Icon · V = Effekt · S = Ton · U = Oberfläche
**Herkunft (Plan):** Baukasten per Blender-Skript · organische Objekte per KI-3D + Blender · Texturen/Icons per KI-Bild (vom Nutzer geprüft) · Figuren und Bosse: offen (KI, gekaufte Basis oder Freelancer). Alles KI-Erzeugte ins `design/asset_log.md`.

**ID-Präfixe** (bestehend): `item_` `tool_` `weapon_` `shield_` `ammo_` `cloth_` `res_` `building_` `prop_` `char_` `anim_` `vfx_` `ui_` `terrain_` `foliage_` `biome_` `poi_` `animal_` `job_`
**ID-Präfixe** (○ neu vorgeschlagen): `npc_` (Personen mit Rolle) · `enemy_` (menschliche Gegner) · `creature_` (Sagenwesen) · `boss_` · `deco_` (Möbel, Schmuck der Siedlung) · `sfx_` (Geräusch) · `amb_` (Umgebungston) · `music_` · `decal_` · `mat_` (Master-Material)

## 1. Überblick: Mengen (grobe Schätzung)
| Bereich | Phase 1 | bis Phase 3 | ganzes Spiel |
|---|--:|--:|--:|
| Spieler- und Bewohner-Figuren (Teile) | 2 Grundkörper | ca. 30 Teile | ca. 60 Teile |
| Tiere (wild und Nutztiere) | 0 | 6 | ca. 16 |
| Menschliche Gegner (Ausrüstungs-Sätze) | 0 | 8 | ca. 20 |
| Bosse | 0 | 1–2 | 6 (Sagen-Fassung: +5) |
| Biome (Landschaftssätze) | 1 Testgelände | 2 | 6 |
| Ressourcen-Vorkommen | 6 | ca. 20 | ca. 40 |
| Orte (POIs) | 0 | 6 | ca. 20 |
| Bauteile und Gebäude | 16 | ca. 60 | ca. 120 |
| Möbel, Requisiten, Deko | 4 | ca. 40 | ca. 90 |
| Gegenstände (Icons) | 8 | ca. 60 | ca. 110 |
| Waffen und Rüstung | 0 | ca. 30 Teile | ca. 70 Teile |
| Animationen | ca. 12 | ca. 90 | ca. 200 |
| Effekte | 3 | ca. 25 | ca. 50 |
| Geräusche und Musik | ca. 20 | ca. 150 | ca. 400 |
| Oberflächen (Bildschirme, Fenster) | 4 | ca. 15 | ca. 25 |
| Texturen (Material-, Deko-, Terrain-, UI-Texturen) | ca. 10 | ca. 80 | ca. 200 |
| Deko-Kleinteile (Schnitzwerk, Schmuck, Streudeko) | 0 | ca. 40 | ca. 120 |

Der teuerste Teil sind **Figuren, Tiere und Bosse** (Skelett, Animation). Der umfangreichste Teil sind **Icons und Geräusche**.

## 2. Figuren
### 2.1 Spielerfigur ✔ (Phase 1 Platzhalter, Phase 2 echt)
| ID | Was | Art | Status |
|---|---|:-:|:-:|
| `char_player_base` | 2 Körpertypen auf dem UE5-Standardskelett | F | ✔ |
| `char_head_*` | 3–6 Köpfe | M | ✔ (3) / ○ (mehr) |
| `char_hair_*`, `char_beard_*` | 3–6 Frisuren, 4 Bärte, Zöpfe | M | ✔ / ○ |
| `char_skin_*` | Hauttöne als Materialvarianten | – | ○ |
| `char_paint_*` | Gesichtsbemalung, Narben, Tätowierungen (Charakter-Editor) | Textur | ○ |
| Kleidung, Rüstung | siehe `ASSET_LIST_RUESTUNG.md` (Slots ✔) | M | ✔ |

### 2.2 Dorfbewohner ✔
| ID | Was | Status |
|---|---|:-:|
| `char_villager_base` | gleiche Basis wie Spieler, schlichter | ✔ |
| `char_villager_variants` | 6+ Kombinationen Kopf/Haar/Kleidung | ✔ |
| Berufskleidung | Beruf erkennbar an **Farbe + Werkzeug** (Schürze Schmied, Fellumhang Jäger …) | ✔ |
| `char_elder` | ältere Bewohner (graue Haare, gebeugte Haltung), später als günstige Variante | ✔ |
| Kinder | **keine im ersten Wurf** | ✔ |

### 2.3 Personen mit besonderer Rolle (`npc_`)
| ID | Wer | Wo | Status |
|---|---|---|:-:|
| `npc_coastal_merchant` | Küstenkaufmann (kommt mit Schiff, kauft saisonal) | Handelsposten, Fischerdorf | ◐ |
| `npc_blackmarket_trader` | Schwarzmarkt-Händler, nur nachts, Zeichen am Wegstein | einsam im Wald | ✔ (Händler bestätigt) |
| `npc_fishing_villager` | Bewohner des Fischerdorfs (Quests, Ruf) | Küste | ◐ |
| `npc_lawspeaker` | Gesetzessprecher beim Thing | Thing-Platz | ○ |
| `npc_skald` | Skalde, trägt die Saga vor (Feste, Saga-Einträge) | Siedlung | ○ |
| `npc_seeress` | Seherin (Hinweise auf Questlinien, Weltoption Sagenwelt) | Opferstein | ○ |
| `npc_prisoner` | Gefangene in Banditenlagern → werden Bewohner | Banditenlager | ◐ |
| `npc_mercenary` | anheuerbare Söldner (Ausbau) | überall | ◐ |

## 3. Tiere (`animal_`)
| ID | Tier | Verhalten | liefert | Biome | Status |
|---|---|---|---|---|:-:|
| `animal_boar` | Wildschwein | greift an, wenn bedrängt | Fleisch, Haut, Hauer | Wiesen | ○ (Grundlage für Boss 1) |
| `animal_deer` | Rothirsch | flieht | Fleisch, Haut, Geweih | Wiesen, Heide | ○ |
| `animal_hare` | Hase | flieht schnell, Fallen | Fleisch, kleines Fell | Wiesen, Heide | ○ |
| `animal_fox` | Rotfuchs | scheu, stiehlt Eier | Fell | Wiesen | ○ |
| `animal_wolf` | Wolf | Rudel, umkreist zuerst | Fell | Wiesen, Heide, Fjell | ◐ |
| `animal_bear` | Braunbär | verteidigt seine Höhle | Fleisch, großes Fell, Fett | Heide, Fjell | ◐ |
| `animal_elk` | Elch | friedlich, gefährlich wenn bedrängt | Fleisch, Haut, Geweih | Heide | ◐ |
| `animal_reindeer` | Rentier | Herden, scheu | Fleisch, Fell, Geweih | Fjell, Eisrand | ◐ |
| `animal_lynx` | Luchs | schleicht, greift Einzelne an | Fell | Fjell | ◐ |
| `animal_seal` | Robbe | flieht ins Wasser | Fleisch, Tran, Fell | Marsch, Küste | ◐ |
| `animal_seabird` | Seevögel | Vogelfelsen | Eier, Federn | Küste | ◐ |
| `animal_arctic_fox` | Polarfuchs | scheu | Fell | Eisrand | ◐ |
| `animal_raven` | Raben | Umgebung, kündigen Ereignisse an (Saga) | – | alle | ○ |
| `animal_fish_*` | Lachs, Hering, Dorsch (Angeln, Reusen) | Schwärme | Fisch | Bäche, Küste | ○ |
| `animal_eagle` | Seeadler | Umgebung, Vogelfelsen | Federn (selten) | Küste, Fjell | ○ |
| Nutztiere | Huhn, Schaf, Ziege, Rind | in der Siedlung, einfach gehalten | Eier, Wolle, Milch, Leder | Siedlung | ✔ |
| `animal_dog` | Hofhund (warnt, begleitet Jäger) | Siedlung | – | Siedlung | ○ |
| `animal_horse` | Pferd oder Ochse als Zugtier für Karren, gesteuert von Bewohnern (später); **kein Reiten im ersten Wurf** ✔ | Siedlung | Zugkraft | Siedlung | ○ |

Jedes Tier braucht mindestens: Modell, Skelett, 6–10 Animationen (Stehen, Gehen, Laufen, Angriff/Flucht, Treffer, Tod, Fressen), 3–5 Geräusche, Kadaver-Zustand.

## 4. Menschliche Gegner (`enemy_`) ◐
**Gruppen** (Register): Waldbanditen (Wiesen, Heide; halten Gefangene) · Strandplünderer (Marsch, Küste; kommen bei Ebbe/übers Wasser) · Eidbrecher (Heide, Fjell; gute Ausrüstung, Formation) · Aschebund-Kult (Glutküste; Fackeln, Brandpfeile) · Söldner (überall).
**Rollen** (Register): Plänkler (Sax) · Schildträger (Rundschild, Speer; Schildwand) · Speerträger · Schläger (Breitaxt).
○ Ergänzt: Bogenschütze · Anführer/Hauptmann (stärker, eigene Optik) · Fackelträger (Aschebund, setzt Gebäude in Brand – nur mit Weltoption Angriffe).

Assets je Gruppe: 1 Kleidungs-/Rüstungssatz in Gruppenfarbe (aus den Rüstungs-Teilen kombiniert), Banner/Erkennungszeichen, 1 Lager-Set (Zelte, Feuerstelle, Palisade, Beutekiste), 5–8 Ruf-/Kampfrufe.

## 5. Sagenwesen (`creature_`) – zurückgestellt
**Entschieden am 2026-11-13:** zuerst nur Menschen und Tiere, Bosse in der realistischen Fassung. Sagenwesen höchstens später als Weltoption „Sagenwelt“. Die Tabelle bleibt als Vorrat für diesen Fall.
| ID | Wesen | Biom | Rolle |
|---|---|---|---|
| `creature_draugr` | Draugr (Untote aus Grabhügeln) | Hügelgrab-Heide | Gegner, nachts aus Grabhügeln |
| `creature_will_o_wisp` | Irrlichter | Gezeitenmarsch | führen in die Irre, Boss-Begleiter |
| `creature_troll` | Troll | Basaltfjell | schwerer Gegner, Boss-Fassung |
| `creature_frost_giant` | Frostriese | Eisrand | Boss-Fassung |
| `creature_lindworm` | Lindwurm | Glutküste | Boss-Fassung |

Aufwand: verdoppelt die Boss-Arbeit. Gemeinfreie Sagenfiguren, aber eigenes Aussehen (keine Anlehnung an bekannte Spiele).

## 6. Bosse (`boss_`) ◐ – ein Boss pro Biom
Gebaut wird zuerst die **realistische Fassung** ✔ (2026-11-13); die Sagen-Fassung nur, falls die Weltoption „Sagenwelt“ später kommt.
| ID | Boss | Biom | Realistische Fassung | Sagen-Fassung | Belohnung |
|---|---|---|---|---|---|
| `boss_old_boar` | Der Alte Keiler | Fjordwiesen | riesiger, narbiger Keiler | (immer realistisch) | Hauer → Bauplan Rennofen (T2) |
| `boss_mud_mother` | Die Schlickmutter | Gezeitenmarsch | Plündererin auf einem Wrack | Moorhexe mit Irrlichtern | Bossmaterial → T3 |
| `boss_barrow_king` | Der Hügelkönig | Hügelgrab-Heide | Eidbrecher-Jarl in Grabfestung | Draugr-Fürst | → T4 |
| `boss_rock_father` | Der Felsenvater | Basaltfjell | Bärenkönig in Eishöhle | Troll | → T5 |
| `boss_ash_tongue` | Die Aschezunge | Glutküste | Hohepriester des Aschebunds | Lindwurm | Relikt-Material (T6) |
| `boss_storm_colossus` | Der Sturmkoloss | Eisrand | Seekönig der Plünderer mit Flotte | Frostriese | Endsieg, Saga-Finale |

Je Boss: Figur, 15–25 Animationen (3 Phasen), Arena (`poi_boss_arena`, fester Ort), Leibwache oder Begleiter, Trophäe (`item_trophy_boss_*`, platzierbar ✔), Bossmaterial-Icon, eigene Musik, Phasen-Effekte (Staubwolke, Flut, Nebel, Lawine, Feuerwalze, Eisbruch), Saga-Bild.

## 7. Landschaften (`biome_`) ◐
| ID | Biom | Stufe | Stimmung | Typische Assets |
|---|---|:-:|---|---|
| `biome_fjord_meadow` | Fjordwiesen und Birkenhaine | T1 | hell, Bäche, Startfjord | Birke, Kiefer, Erle, Wiesenblumen, Bachkiesel, Findlinge, Fjordufer |
| `biome_tidal_marsh` | Gezeitenmarsch | T2 | Watt, Priele, Morgennebel | Schilf, Weiden, Seegras, Schlick, Priele, Wracks, Pfahlstege, Torfstiche |
| `biome_barrow_heath` | Hügelgrab-Heide | T3 | Heide, Steinsetzungen | Heidekraut, Wacholder, krumme Kiefern, Grabhügel, Schiffssetzungen aus Stein, Bautasteine |
| `biome_basalt_fell` | Basaltfjell | T4 | Säulenfelsen, Wasserfälle, Schnee | Basaltsäulen, Fichten, Zwergbirken, Moos, Flechten, Wasserfälle, Eishöhlen, Schneefelder |
| `biome_ember_coast` | Glutküste | T5 | Lavafelder, schwarzer Sand | Lavagestein, verkohlte Stämme, Farne an Quellen, Dampfspalten, heiße Quellen, Obsidian |
| `biome_ice_rim` | Eisrand | T6 | Gletscher, Sturm | Gletschereis, Eisbrocken, Schneewehen, gefrorene Wracks, Eishöhlen |

Je Biom: 4–6 Terrain-Schichten (`terrain_*`), 3–5 Bäume/Großpflanzen mit LOD und Fernansicht, 6–10 Bodenbewuchs (`foliage_*`), 5–8 Felsen, 1 Wasser-Material, Himmels- und Lichtstimmung (`env_sky_*`), Wetter, Umgebungston, Musik.

## 8. Ressourcen-Vorkommen (`res_`)
| ID | Vorkommen | liefert | Biom | Status |
|---|---|---|---|:-:|
| `res_tree_pine`, `res_tree_birch` | Kiefer, Birke (stehend/gefällt/Stumpf) | Stämme | Wiesen | ✔ Slice |
| `res_tree_alder`, `res_tree_spruce`, `res_tree_oak` | Erle, Fichte, Eiche (Eiche = hartes Holz ab T2) | Stämme, Rinde | je Biom | ○ |
| `res_rock_small/medium/large` | Felsen | Stein | alle | ✔ Slice |
| `res_flint` | Feuersteinknollen am Bach | Feuerstein | Wiesen | ○ |
| `res_berry_bush` | Beerenstrauch (voll/leer) | Beeren | Wiesen | ✔ Slice |
| `res_mushrooms`, `res_herbs` | Pilze, Kräuter (Heilung, Färben) | Pilze, Kräuter | Wald, Heide | ○ |
| `res_reeds` | Schilf | Stroh/Schilf (Dach) | Marsch | ◐ |
| `res_peat` | Torfstich | Torf (Brennstoff) | Marsch, Heide | ◐ |
| `res_bog_ore` | Raseneisen im Moor | Raseneisenerz | Marsch | ◐ |
| `res_clay_pit` | Tongrube | Ton | Bachufer | ○ |
| `res_mussel_bank` | Muschelbank (nur bei Ebbe) | Muscheln | Marsch | ◐ |
| `res_driftwood` | Treibholz | Holz, selten Fundstücke | Küste | ○ |
| `res_amber` | Bernstein am Strand | Bernstein | Küste | ○ |
| `res_iron_vein` | Eisenerz-Ader | Eisenerz | Fjell | ◐ |
| `res_crystal` | Kristalle | Kristall (Schmuck, Relikte) | Fjell | ◐ |
| `res_sulfur`, `res_obsidian` | Schwefel, Obsidian | Flussmittel für Damast | Glutküste | ◐ |
| `res_relic_ice` | eingefrorene Relikte, alte Wracks | Relikt-Material, Eisen | Eisrand | ◐ |
| `res_flax_wild`, `res_wild_grain` | wilder Flachs, Wildgerste (Saatgut-Quelle) | Saatgut | Wiesen | ○ |
| `res_beehive_wild` | wildes Bienennest | Honig, Wachs | Wald | ○ |
| `res_fish_spot` | Fischgrund (Angeln, Reuse) | Fisch | Bach, Küste | ○ |

Jedes Vorkommen: Zustände voll/abgeerntet/Rest, Treffer-Effekt, 2–3 Geräusche, Icon des Ertrags.

## 9. Orte (`poi_`) ◐
Langhaus-Ruine · Grabhügel (kleiner Dungeon) · Banditenlager · verlassener Hof · Fischerdorf · Handelsposten · Opferstein · Höhle/Bärenhöhle · Turmruine (deckt Karte auf) · Schiffswrack (nur bei Ebbe) · heiße Quelle · alte Mine · Schwarzmarkt-Zeichen · Boss-Arena.
○ Ergänzt: Runensteinfeld (Saga-Fragmente) · Opferhain mit heiligem Baum · Furt und Holzbrücke · Seezeichen/Leuchtfeuer · Thingplatz einer fremden Siedlung · Köhlerlichtung (verlassen) · Fallgrube/Jagdstand · Robbenfelsen · Eishöhle mit gefrorenem Schiff.
Je Ort: eigenes Modul-Set oder Kombination aus Baukasten, Beutebehälter, 1–2 besondere Requisiten, Kartensymbol.

## 10. Siedlung: Baukasten (`building_`)
| Satz | Inhalt | Phase | Status |
|---|---|:-:|:-:|
| Holz (T1) | Fundament, Boden, Wand, Wand mit Tür/Fenster, Tür, Pfosten, Balken, Dach, First, Giebel, Treppe, Zaun, Tor | 1 | ✔ (10 Modelle in `Assets/`) |
| Flechtwerk und Lehm | Wand, Wand mit Tür, Ausbesserung | 2 | ○ |
| Grassoden (Turf) | Dachfläche, First, Kante (Turf-Kante ✔ beim Holzdach) | 2 | ✔ / ○ |
| Stein (Materialstufe 2) | Steinfundament, Trockenmauer, Herd, Stufen | 2 | ◐ |
| Palisade | Pfahlwand, Tor, Wehrgang, Wachturm | 2 | ○ |
| Hafen | Steg, Pfahl, Bootsrampe, Bootshaus | 4 | ○ |
| Langhaus-Teile | geschwungene Wand, Langdach, Firstschmuck, Türrahmen mit Schnitzwerk | 2 | ○ |

## 11. Siedlung: Gebäude und Arbeitsstätten
| ID | Gebäude | Beruf (`job_`) | Phase | Status |
|---|---|---|:-:|:-:|
| `building_storage` | Lager (eins pro Siedlung, voll/leer sichtbar) | Träger | 1 | ✔ |
| `building_woodcutter` | Holzfällerplatz | Holzfäller | 1 | ✔ |
| `building_stonecutter` | Steinmetzplatz | Steinmetz | 1 | ✔ |
| `building_campfire` | Lagerfeuer = Kochstelle | Koch | 1 | ✔ |
| `building_bed` | Strohlager | – | 1 | ✔ |
| `building_workbench` | Werkbank | Spieler | 1 | ✔ |
| `building_totem` | Siedlungszentrum (Anführer-Symbol) | – | 1 | ✔ |
| `building_sawpit` | Sägegrube | Säger | 2 | ◐ |
| `building_forager_hut` | Sammlerhütte | Sammler | 2 | ◐ |
| `building_builder_yard` | Bauhof | Baumeister | 2 | ◐ |
| `building_hunter_hut` | Jägerhütte, Trockengestell | Jäger | 2 | ◐ |
| `building_fisher_hut` | Fischerhütte, Netze, Reusen | Fischer | 2 | ◐ |
| `building_field` | Feld (Gerste, Flachs ✔), Saat-/Wachs-/Erntezustand | Bauer | 2 | ✔ |
| `building_coop`, `building_pen`, `building_byre` | Hühnerstall, Pferch, Stall | Hirte | 2–3 | ✔ |
| `building_mill` | Handmühle, Backofen | Müller/Bäcker | 3 | ◐ |
| `building_tannery` | Gerberei (Bottiche, Gestelle) | Gerber | 2 | ◐ |
| `building_weaver` | Webhaus (Webstuhl, Spinnrocken) | Weber | 3 | ◐ |
| `building_healer` | Heilerhütte (Kräuterbündel, Liegen) | Heiler | 3 | ◐ |
| `building_guardhouse` | Wachhaus, Waffenständer | Wache/Miliz | 2 | ◐ |
| `building_charcoal_kiln` | Kohlenmeiler | Köhler | 3 | ◐ |
| `building_bloomery` | Rennofen (Bauplan vom Alten Keiler) | Schmelzer | 3 | ◐ |
| `building_smithy` | Schmiede (Esse, Amboss, Blasebalg, Löschtrog), 3 Ausbaustufen | Schmied | 3 | ◐ |
| `building_longhouse` | Langhaus (Wohnen, Feste) | – | 2 | ○ |
| `building_pit_house` | Grubenhaus (einfache Werkstatt/Wohnung) | – | 2 | ○ |
| `building_smokehouse` | Räucherhaus (Fleisch, Fisch haltbar) | Koch | 2 | ○ |
| `building_well` | Brunnen | – | 2 | ○ |
| `building_bathhouse` | Badehaus (Zufriedenheit) | – | 3 | ○ |
| `building_brewery` | Brauhaus (Met, Bier) | Brauer | 3 | ○ |
| `building_apiary` | Bienenkörbe | Imker | 3 | ○ |
| `building_pottery` | Töpferei, Brennofen | Töpfer | 3 | ○ |
| `building_thing_site` | Thingplatz (Steinkreis, Sitzsteine) | – | 2 | ◐ |
| `building_jarl_hall` | Jarlssitz (Belohnung Saga-Finale) | – | 5 | ◐ |
| `building_runestone` | Runenstein, Erinnerungsstein (Prestige) | – | 4 | ◐ |
| `building_portal` | Portal (Spätspiel, verbindet Lager) | – | 5 | ✔ |
| `building_shipyard` | Bootsbauplatz | Bootsbauer | 4 | ○ offen |

## 12. Möbel, Requisiten, Deko (`prop_`, `deco_`)
- **Behälter ✔:** Sack, Korb, Fass, Kiste, Bündel für abgelegte Gegenstände, Trockengestell.
- **Werkstatt-Requisiten:** Hackklotz, Sägebock, Steinmetz-Werkbank, Amboss, Esse, Blasebalg, Löschtrog, Gerbbottich, Spannrahmen, Webstuhl, Spinnrocken, Mahlstein, Backofen, Kessel mit Galgen ✔, Bratspieß, Netz, Reuse, Fischgestell, Bienenkorb, Töpferscheibe.
- **Wohnen:** Bettgestell, Strohlager ✔, Bank, Tisch, Hocker, Truhe (Deko – Lagerung bleibt im Lager ✔), Herdstelle im Langhaus, Kochgeschirr, Trinkhorn, Tranlampe, Fackelhalter, Fellteppich, Wandbehang, Webteppich, Kräuterbündel, Schilde an der Wand.
- **Siedlung:** Wegsteine, Zaunarten, Brennholzstapel, Holzstapel ✔ (Füllstand), Steinhaufen, Erntegarben, Heuhaufen, Wassertrog, Futterkrippe, Vogelscheuche, Banner der Siedlung (Lager-Editor ✔), Festtafel, Feuerkorb, Opferschale.
- **Trophäen ✔:** je Boss eine platzierbare Trophäe, dazu Tierfelle und Geweihe als Wanddeko.

## 13. Gegenstände
Vollständig in `ASSET_LIST_GEGENSTAENDE.md` (Rohstoffe, Zwischenprodukte, Nahrung, Werkzeuge, Verbrauchsgüter, Handel, Behälter). Ergänzungen aus den Biomen (○ bzw. ◐ aus dem Register):
| ID | Gegenstand | Herkunft | Status |
|---|---|---|:-:|
| `item_peat` | Torf (Brennstoff, Köhler) | Marsch, Heide | ◐ |
| `item_mussels` | Muscheln (Nahrung, Köder) | Marsch | ◐ |
| `item_herbs` | Kräuter (Salben, Färben) | Heide | ◐ |
| `item_fur`, `item_fur_large` | Fell, großes Fell | Wolf, Bär, Luchs … | ◐ |
| `item_fat`, `item_train_oil` | Fett, Tran (Lampen, Leder) | Bär, Robbe | ◐ |
| `item_tusk_boar` | Hauer (Bauplan Rennofen) | Alter Keiler | ◐ |
| `item_crystal` | Kristall | Fjell | ◐ |
| `item_sulfur`, `item_obsidian`, `item_flux` | Schwefel, Obsidian, Flussmittel | Glutküste | ◐ |
| `item_relic_material` | Relikt-Material | Aschezunge, Eisrand | ◐ |
| `item_seed_barley`, `item_seed_flax` | Saatgut | Händler, verlassener Hof | ◐ |
| `item_wax`, `item_honey` | Wachs, Honig | Bienen | ○ / Liste |
| `item_blueprint_*` | Baupläne (Rennofen, Armbrust T4 …) | Ruinen, Bosse, Schwarzmarkt | ◐ |
| `item_saga_fragment_*` | Saga-Fragmente (Sammelobjekte) | Ruinen, Runensteine | ◐ |
| `item_logbook` | Logbuch (Questgegenstand, Schlickmutter) | Boss 2 | ◐ |
| `item_bridge_runes` | Brückenrunen (Questgegenstand) | Geschichte | ◐ |
| `item_trap_kit` | Fallen-Bausatz (Grubenfalle für den Keiler) | Werkbank | ◐ |
| `item_gift_*` | Gastgeschenke (Siedler anwerben) | hergestellt | ◐ |
| `item_beer`, `item_ale_mead` | Bier, Met | Brauhaus | ○ / Liste |
| `item_pottery` | Tonkrug, Schale (Lagerung, Handel) | Töpferei | ○ |

## 14. Waffen, Schilde, Rüstung, Kleidung
- **Waffen ✔/◐:** `ASSET_LIST_WAFFEN.md` – Bartaxt, Sax, Speer, Schwert (ab T3), Rundschild, Bogen, Pfeile (MVP ✔); Keule, Breitaxt, Wurfspeer, Schleuder, Armbrust und Bolzen; Teile-Strategie (Kopf je Material, geteilter Stiel, Stufen über Materialinstanzen).
- **Rüstung ✔:** Slots Helm, Schulter, Oberkörper (seit 2026-11-13), Hände, 2 × Ring (Ringe mit Wirkung, die einzige Ausnahme vom „kein Schmuck-Slot“), Beine, Füße, Umhang – `ASSET_LIST_RUESTUNG.md`.
- **Kleidung:** Tunika, Hose, Schuhe, Kapuze, Schürze, Kleid; ○ ergänzt: Wickelgamaschen, Fellmantel, Gürtel mit Taschen, Wollmütze, Fäustlinge (Kälte im Fjell).
- **Rucksack ◐:** Formen je Ausbaustufe (Tragegestell, Lederranzen, Fellsack), sichtbar am Rücken.
- **Qualität ◐:** grob · ordentlich · gut · meisterhaft · Meisterwerk mit eigenem Namen – sichtbar über Materialinstanz (Glanz, Gravur), kein eigenes Modell.

## 15. Transport und Fahrzeuge ○ (offen, nur Vorschlag)
Handkarren (Träger, Phase 2) · Schlitten (Winter, Phase 3) · Ruderboot (Küste, Phase 4) · Karve als Kulisse beim Küstenkaufmann (Phase 4). Ein fahrbares Schiff wäre ein großes Zusatzsystem und gehört nicht in den ersten Wurf.

## 16. Animationen (`anim_`)
| Gruppe | Inhalt | Phase |
|---|---|:-:|
| Fortbewegung ✔ | Stehen, Gehen, Laufen, Ducken, Springen, Schwimmen (○), Klettern niedrig (○) – **ohne Erschöpfung** | 1–2 |
| Arbeit ✔ | Hacken, Abbauen, Tragen, Bauen/Hämmern, Kochen; ○ Sägen, Graben, Säen, Ernten, Melken, Füttern, Schmieden, Weben, Gerben, Angeln, Netz werfen, Mahlen | 1–3 |
| Bedürfnisse ✔ | Essen, Schlafen, Sitzen, Wärmen am Feuer (○), Waschen (○) | 1–2 |
| Kampf ◐ | 3 Schlagrichtungen + Stoß je Waffengruppe, Block, Parade, Ausweichen, Schildstoß, Bogen spannen/schießen, Armbrust laden, Treffer, Haltungsbruch (Taumeln), Tod | 3 |
| Sozial ○ | Reden, Zustimmen/Ablehnen (Thing), Jubeln, Trinken, Tanzen am Fest, Trauern, Winken | 2–4 |
| Tiere | je Tier 6–10 (siehe Abschnitt 3) | 2–4 |
| Bosse | je Boss 15–25 | 3–5 |

Quelle: Skelett kompatibel zum UE5-Standardskelett ✔, damit gekaufte oder übertragene Animationen passen (Lizenz prüfen).

## 17. Effekte (`vfx_`)
- ✔ Phase 1: Lagerfeuer (Feuer, Rauch), Holzspäne, Steinstaub, Platzieren eines Bauteils.
- Arbeit: Funken an der Esse, Glut beim Schmieden („Glut und Takt“ ◐), Dampf im Löschtrog, Mehlstaub, Wasserspritzer beim Fischen.
- Kampf: Treffer auf Holz/Metall/Fleisch, Funken beim Parieren, Blut ✔ und abtrennbare Körperteile ✔ (beides schaltbar), Haltungsbruch-Hinweis.
- Welt: Regen, Schnee, Nebel (Morgennebel in der Marsch, Nachtnebel in der Heide), Sturm mit Sichtgrenze, Wasserfall-Gischt, Brandung, Gezeiten-Übergang, Lava-Glühen, Dampf heißer Quellen, Fußspuren im Schnee, Staub in Höhlen, Glühwürmchen (Wiesen, Sommer).
- Sagenwelt ◐: Irrlichter, Draugr-Nebel, Troll wird zu Stein (Sonne).
- Siedlung: Rauch aus Dächern, Feuer an Gebäuden (Weltoption Angriffe), Fest-Feuerwerk aus Funken, Saga-Ereignis-Hinweis.

## 18. Ton und Musik
- **Musik (`music_`):** Thema je Biom (6), Tag/Nacht-Variante, Kampf (leicht/schwer), je Boss ein Thema (6), Siedlung im Frieden, Fest, Thing, Saga-Finale. Instrumente passend: Leier, Knochenflöte, Trommeln, Kehlgesang. **Kein Gesang mit geschützten Texten.**
- **Umgebung (`amb_`):** je Biom Tag/Nacht/Wetter, Siedlung mit Arbeitsgeräuschen, Innenraum Langhaus, Höhle, Brandung, Wind am Gletscher.
- **Geräusche (`sfx_`):** Schritte je Untergrund (Gras, Erde, Stein, Holz, Schlick, Schnee, Kies), Arbeit je Beruf, Bauen (Platzieren, Abreißen, Einsturz), Werkzeuge, Waffen (Schwung, Treffer je Material, Parade, Schild), Bogen/Armbrust, Tierlaute, Bewohner-Rufe (kurze Laute in einer erfundenen, altnordisch klingenden Sprache ○), Oberfläche (Klick, Fehler, Saga-Eintrag, Stufenaufstieg).

## 19. Oberflächen (`ui_`)
| ID | Bildschirm/Fenster | Phase | Status |
|---|---|:-:|:-:|
| `ui_hud` | Gesundheit, Schnellleiste, Koop-Liste (bis 8), **kein Ausdauerbalken** | 1 | ✔ |
| `ui_build_menu` | Baumenü mit Kategorien | 1 | ✔ |
| `ui_inventory` | Inventar und Rucksack, Traglast | 1 | ◐ |
| `ui_crafting` | Herstellen je Station, Qualitätsanzeige | 1–3 | ◐ |
| `ui_settlement` | Bewohner, Berufe, Prioritäten, Bedürfnisse, Vorratsziele („Halte 40 Bretter vorrätig“) | 2 | ◐ |
| `ui_command_view` | Kommandoansicht (Vogelperspektive) | 2 | ✔ |
| `ui_map` | Karte mit Nebel, Orten, Markierungen | 2 | ○ |
| `ui_saga` | Saga-Buch (Einträge, Relikte, Taten) | 2 | ◐ |
| `ui_thing` | Thing-Versammlung, Abstimmungen | 2 | ◐ |
| `ui_trade` | Handel mit Hacksilber | 4 | ✔ |
| `ui_character_editor` | Charakter-Editor (Körper, Kopf, Farben) | 2 | ✔ |
| `ui_settlement_editor` | Lager-Editor (Farben, Wappen, Banner) | 2 | ✔ |
| `ui_skills` | Fähigkeitenbaum, Attribute | 3 | ✔ |
| `ui_world_setup` | Welt erstellen: Seed, Größe, Weltoptionen | 2 | ✔ |
| `ui_multiplayer` | Server beitreten/erstellen, Spielerliste | 1–2 | ○ |
| `ui_settings` | Einstellungen inkl. Blut/Körperteile schaltbar | 2 | ✔ |
| `ui_icons_*` | Icons: Gegenstände, Berufe, Bedürfnisse, Buffs, Stufenrahmen T1–T6, Kartensymbole | 1+ | ✔ |
| `ui_font_*` | Zierschrift (runenartig, nur Überschriften) + gut lesbare Fließtextschrift | 1 | ○ |
| `ui_loading` | Ladebildschirme mit Saga-Bildern | 4 | ○ |

## 20. Gestaltung im Charakter- und Lager-Editor ✔
Farbstoffe (Waid blau, Krapp rot, Wau gelb ✔) · Wappenzeichen ○ (Rabe, Wolf, Schiff, Drachenkopf, Knotenmuster, Axt, Sonne, Baum) · Schildmuster (geteilt, geviertelt, Spirale, Ring) · Bannerformen (Rechteck, Schwalbenschwanz, Dreieck) · Muster für Kleidungsborten.

## 21. Technik und Grundlagen (für alle Assets)
- **Master-Materialien (`mat_`):** Undurchsichtig, Bewuchs (mit Wind), Terrain, Wasser (Bach, Meer, Gezeiten), Figur/Haut, Haar, Metall (Stufen-Instanzen ✔), Glas/Eis, Effekte.
- **Trim-Sheets:** Holz ✔ (neu, verwittert), Stein, Metall, Leder, Stoff, Turf.
- **Decals (`decal_`):** Schmutz, Moos, Blut (schaltbar), Fußspuren, Brandflecken, Schnee auf Dächern, Runen-Gravuren.
- **Detailstufen:** LODs für alles Größere, Fernansicht für Bäume (Impostor), HLOD für die Siedlung; Budget ✔: Spiel höchstens ca. 20 GB, läuft ab 8 GB RAM.
- **Platzhalter:** einfache Formen für alles, was noch fehlt (wie die Zylinder-Spielfigur jetzt).

## 22. Texturen ○ (Vorschlag, ergänzt am 2026-11-13)
Texturen sind die Bilder, die auf Modellen liegen. Damit das Spiel unter ca. 20 GB bleibt, gilt: **lieber wenige, kachelbare Texturen und Trim-Sheets, die viele Modelle teilen**, als für jedes Modell eigene. Format wie beim Holzbaukasten ✔: Farbe (`_BC`), Normalen (`_N`), AO/Rauheit/Metall (`_ORM`).

### 22.1 Material-Texturen (kachelbar, `T_Mat_*`)
| Gruppe | Varianten |
|---|---|
| Holz | Kiefer, Birke, Eiche, Treibholz (grau, ausgewaschen), verkohlt, frisch geschnitten (Stirnholz mit Jahresringen), bemalt |
| Rinde | Kiefer, Birke (weiß mit Flecken), Fichte, Erle |
| Stein | Granit, Feldstein, Sandstein, Basalt (Säulenstruktur), Lavagestein, Schiefer, behauener Stein, Trockenmauer |
| Erde und Boden | Erde, Lehm, Schlick (nass glänzend), Torf, Kies, Sand, schwarzer Sand, Asche |
| Dach | Stroh/Schilf, Grassoden, Holzschindeln, Birkenrinde unter Soden |
| Wand | Flechtwerk, Lehmputz (glatt und rissig), gekalkt |
| Metall | Eisen roh, Eisen geschmiedet, Stahl, Damast-Maske, Bronze, Silber/Hacksilber, Rost, Hammerschläge |
| Stoff | Wolle grob und fein, Leinen, Filz, Fischgrät- und Rautenköper, Brettchenborten |
| Leder und Fell | Rohhaut, Leder hell/dunkel, genietet, Fell (Wolf, Bär, Schaf, Rentier, Robbe), Pelzrand |
| Natur | Eis (klar, trüb, Gletscherblau), Schnee (frisch, verharscht), Moos, Flechten, Laubstreu, Nadelstreu |

### 22.2 Terrain-Texturen (`T_Terrain_*`)
Je Biom 4–6 Schichten (Abschnitt 7): Gras kurz/lang, Waldboden, Erde, Fels, Weg (festgetreten), Bachkiesel, Schlick, Heideboden, Schnee, Lava, schwarzer Sand. Dazu **Jahreszeiten-Varianten** über Farbverschiebung statt neuer Texturen: Frühling frisch, Sommer satt, Herbst braun-gold, Winter mit Schneedecke.

### 22.3 Deko-Texturen und Muster (`T_Deco_*`)
| Gruppe | Beispiele | Wo benutzt |
|---|---|---|
| Schnitzmuster | Flechtband, Tierornamente in frühmittelalterlichen Stilen (Borre, Jellinge, Mammen, Urnes – historisch, gemeinfrei, eigene Umsetzung), Drachenköpfe, Ranken | Türrahmen, Firstbalken, Pfosten, Truhen, Waffengriffe |
| Runen | Runeninschriften (jüngeres Futhark), Runenstein-Bänder mit Schlangenleib, eingeritzte Zeichen | Runensteine, Gravuren, Meisterwerk-Waffen |
| Webmuster | Wandbehänge mit Szenen (Schiffe, Reiter, Jagd), Teppichmuster, Brettchenborten | Langhaus, Kleidung, Festtafel |
| Malerei | Schildbemalungen, bemalte Balken, Bannermotive, Wappenzeichen (Abschnitt 20) | Schilde, Banner, Gebäude |
| Abnutzung | Ruß, Wasserflecken, Kerben, Moosbewuchs, ausgeblichene Farbe | über Masken auf allen Bauteilen |
| Zustand Siedlung | gepflegt / vernachlässigt / beschädigt (Angriffe) | Bauteile, Zäune |

### 22.4 Decals und Masken
Siehe auch Abschnitt 21. Zusätzlich: Brandspuren, Pfotenabdrücke im Schnee, Wagenspuren, Blätter auf Wegen (Herbst), Pfützen nach Regen, Lagerfeuer-Asche, Teppichkanten, Blutspuren (schaltbar ✔), Schneeauflage oben auf Modellen (eine Maske für alles statt eigener Winter-Modelle).

### 22.5 Himmel, Wasser, Effekte
Wolkenschichten (Schönwetter, Regen, Sturm, Polarlicht im Winter ○), Sternhimmel, Mond; Wasser-Normalen (Bach, See, Meer, Brandung), Schaum, Gischt; Rauch-, Funken- und Nebel-Texturen für Effekte; Rauschen und Verlaufs-Masken (technisch, selbst erzeugt).

### 22.6 Oberflächen-Texturen (`T_UI_*`)
Pergament- oder Leinen-Hintergrund, Holzrahmen mit Schnitzkante, Eisenbeschläge an Fensterecken, Stufenrahmen T1–T6 für Icons ✔, Kartenpapier mit Tusche-Stil, Saga-Buch-Seiten, Runen-Zierleisten, Schaltflächen (normal/aktiv/gesperrt). Lesbarkeit hat Vorrang ✔: Zierde nur am Rand.

## 23. Deko-Modelle und Kleinkram ○ (ergänzt am 2026-11-13)
Kleine Modelle, die Orte lebendig machen. Ohne Spielfunktion, aber wichtig für Stimmung und Wiedererkennung.
- **Siedlungs-Kleinkram:** Eimer, Schöpfkelle, Holzschalen, Löffel, Seilrollen, aufgehängte Netze, Werkzeuge an der Wand, Späne-Haufen, Sägemehl, Brennholzkorb, Wassereimer am Brunnen, Spinnwirtel, Webgewichte, Mahlsteine, Kesselhaken, Räucherfisch an Stangen.
- **Schnitzwerk als Modell:** Drachenkopf am Giebel, geschnitzte Türpfosten, Hochsitzpfeiler im Langhaus, Wetterfahne, Stuhllehnen, Truhenbeschläge.
- **Fest- und Jahreszeiten-Deko:** Erntegarben, Kränze, Laternen, Festbanner, Julfeuer, Tannengrün im Winter, Fackelreihen beim Thing, Opfergaben am Opferstein.
- **Natur-Streudeko:** Äste, Zapfen, Pilzgruppen, Farne, Steinchen, Muscheln am Strand, Tierknochen, Geweihabwürfe, umgestürzte Stämme, Wurzelteller, Bachsteine mit Moos, Eiszapfen.
- **Ruinen-Teile:** eingestürzte Wände, verkohlte Balken, umgestürzte Runensteine, zerbrochene Schilde, rostige Klingen, Knochenhaufen (für Grabhügel und Banditenlager).
- **Licht als Deko:** Fackeln, Feuerkörbe, Tranlampen, Talglichter, Glut in Herdstellen – mit Effekt und Licht, sparsam wegen Leistung.
- **Bewegliche Deko:** Wäsche und Felle auf Leinen, Banner im Wind, Rauchfahnen, Windspiel aus Knochen – bewegen sich über Material (Wind), nicht über Animation.

## 24. Was als Nächstes sinnvoll ist
1. **Biome bestätigen** (Abschnitt 7) – davon hängen Bäume, Felsen, Tiere, Gegner und Bosse ab.
2. ~~Sagenwelt ja/nein~~ – entschieden am 2026-11-13: zuerst nein, später höchstens Weltoption.
3. **Herkunft der Figuren entscheiden** (KI, gekaufte Basis, Freelancer) – größter Kostenblock.
4. **Icon-Raster festlegen** (Blickwinkel, Licht, Rahmen, Stufenfarbe) – größter Mengenblock.
5. Die ○-Vorschläge dieser Liste durchgehen: übernehmen, streichen oder später.

Figuren- und Kampf-Entscheidungen vom 2026-11-13 (Körperbau frei kombinierbar, keine Sprachausgabe, Namensschild + Spielerfarbe, Richtungsangriffe, Zielerfassung, Ausweichschritt, Trefferzonen u. a.) stehen in `docs/01_entscheidungen.md`.
