# Asset-Plan für ChatGPT (Stand 2026-11-12)

*Für ChatGPT geschrieben, ergänzt `design/CHATGPT_BRIEFING.md`. Der Nutzer kopiert dieses Dokument in den Chat.*
*Alle Zahlen sind Vorschläge von Claude und gelten, bis der Nutzer sie ändert. Status der Entscheidungen: `docs/01_entscheidungen.md`.*

## 1. Zweck
Dieser Plan legt fest, **welche Assets das Spiel zuerst braucht, in welcher Reihenfolge, mit welchen technischen Grenzen und in welchem Format du sie beschreibst.**
Zuerst wird nur der **Vertical Slice (Phase 1, „Siedlung“)** ausgestattet: Spielfigur (Koop), Bauteile, Ressourcen, 3–4 Dorfbewohner-Jobs, 3 Bedürfnisse, ein Lager. Alles andere steht in Abschnitt 8 nur als Ausblick.

## 2. Was du lieferst – und was nicht
| Du lieferst | Du lieferst nicht |
|---|---|
| Konzeptblätter pro Asset (Bild + Beschreibung, Maße, Materialien, Farbwerte) | fertige 3D-Modelle, Rigs, Animationen, Unreal-Dateien |
| 2D-UI-Elemente und Icons (Entwurf; Endfassung siehe Abschnitt 7) | Code, Spielwerte, Datentabellen in `data/` |
| Stilregeln: Formensprache, Palette, Materialliste, Abnutzungsgrad | Nachbauten geschützter Designs (Aska, Valheim, KCD, Manor Lords, Medieval Dynasty) |
| Audio-Richtung (Beschreibung, Referenzbegriffe) | Musik/Sounds als fertige Dateien |

**Wichtig:** Woher die fertigen 3D-Modelle kommen, ist **noch nicht entschieden** (siehe Abschnitt 9). Deine Konzeptblätter müssen deshalb so genau sein, dass ein 3D-Artist, ein gekauftes Asset-Pack oder ein Werkzeug danach arbeiten kann.

## 3. Voraussetzung
Die **Art Direction ist noch offen** (Briefing, Auftrag 1). Dieser Asset-Plan wird erst nach der Stilwahl des Nutzers bearbeitet. Beschreibe alle Assets im gewählten Stil.

## 4. Technische Vorgaben (für jedes Konzeptblatt)

### 4.1 Maßstab und Raster
- 1 Unreal-Einheit = 1 cm. Echte Größen verwenden. Erwachsene Figur ca. 180 cm.
- **Bauraster: 300 cm.** Wandmodul 300 × 300 cm, Fundament 300 × 300 cm, Dachneigung 45°. Alle Bauteile passen auf dieses Raster (Snapping).
- Türöffnung mindestens 120 × 220 cm (Figuren mit Traglast und Koop-Gedränge).

### 4.2 Größen- und Polygon-Budgets
Ziel: Das fertige Spiel bleibt **unter ca. 20 GB** und läuft mit **8 GB RAM** auf niedriger Stufe. Lumen/Nanite sind noch nicht entschieden, deshalb gelten klassische Budgets mit LODs.

| Asset-Art | Dreiecke LOD0 (max.) | Texturen (max.) | Hinweis |
|---|---|---|---|
| Spielerfigur inkl. Kleidung | 40.000 | 2048² | modular: Körper, Kopf, Haare, Oberteil, Hose, Schuhe |
| Dorfbewohner | 20.000 | 2048², geteilt mit anderen | viele gleichzeitig im Bild → wenige Teile, Variation über Farbe |
| Bauteil (Wand, Boden, Dach …) | 2.000 | **Trim-Sheet** 2048² pro Materialstufe | keine eigene Textur pro Bauteil |
| Gebäude-Unikat (z. B. Lager) | 15.000 | Trim-Sheet + max. 1 × 1024² | möglichst aus Bauteilen zusammensetzen |
| Baum | 15.000 | 2048² Rinde + Blatt-Atlas | gefällter Stamm und Stumpf als eigene Teile |
| Fels, Busch | 3.000 | 1024² | 3 Größen aus einer Textur |
| Kleines Objekt (Werkzeug, Item) | 1.000 | 512²–1024², Atlas bevorzugt | |
| Terrain-Schicht | – | 2048², kachelbar | höchstens 6 Schichten |
| UI-Icon | – | 256² im Spiel (Entwurf 512²) | gleicher Rahmen und Blickwinkel |

Keine 4K-Texturen. Variation über **Farbe, Kombination und Abnutzung** statt über neue Assets.

### 4.3 Vorläufige Aufteilung der 20 GB
| Bereich | Budget |
|---|---|
| Texturen | 8 GB |
| Welt- und Kartendaten | 3 GB |
| Meshes | 3 GB |
| Audio (komprimiert) | 2 GB |
| Animationen | 1 GB |
| Materialien, Shader, Effekte | 1 GB |
| Engine, Code, Rest | 1,5 GB |
| Reserve | 0,5 GB |

Claude misst die Paketgröße laufend (`tools/build.ps1 Package`). Heute: 0,96 GB ohne eigene Inhalte.

### 4.4 Figuren und Animation
- Proportionen kompatibel zum **Unreal-5-Standardskelett** (Mannequin). So lassen sich vorhandene Animationen übertragen, statt jede neu zu erstellen.
- Spieler und Dorfbewohner nutzen **dasselbe Skelett**.
- Ausrüstung später als austauschbare Teile über diesem Skelett (KCD-ähnliche Slots).

### 4.5 Benennung
Jedes Asset hat eine **stabile ID in `snake_case`** (wird nie wiederverwendet) und einen Unreal-Namen nach Typ:

| Typ | Präfix | Beispiel-ID → Unreal-Name |
|---|---|---|
| Statisches Mesh | `SM_` | `building_wall_wood` → `SM_Building_Wall_Wood` |
| Skelett-Mesh | `SK_` | `char_villager_base` → `SK_Char_Villager_Base` |
| Textur | `T_` + Suffix `_BC` / `_N` / `_ORM` | `T_Trim_Wood_BC` |
| Material / Instanz | `M_` / `MI_` | `MI_Trim_Wood_Weathered` |
| Animation | `A_` | `A_Villager_Chop_Loop` |
| Effekt (Niagara) | `NS_` | `NS_Campfire` |
| Sound | `SW_` / `MS_` (MetaSound) | `SW_Axe_Chop_01` |
| UI-Widget / Icon | `WBP_` / `T_Icon_` | `T_Icon_Res_Wood` |

## 5. Asset-Liste Phase 1 (Vertical Slice)
Priorität: **A** = ohne geht der Slice nicht · **B** = wichtig für Lesbarkeit/Stimmung · **C** = schön, notfalls Platzhalter.
Jobs und Bedürfnisse sind Vorschläge; endgültig legt sie der Nutzer fest.

### 5.1 Figuren
| ID | Beschreibung | Prio |
|---|---|---|
| `char_player_base` | Spielerfigur, modular (2 Körpertypen, 3 Köpfe, 3 Frisuren, 2 Grund-Outfits), Farbe pro Koop-Spieler unterscheidbar | A |
| `char_villager_base` | Dorfbewohner, gleiche Basis, schlichter; Beruf an **Kleidungsfarbe + Werkzeug** erkennbar | A |
| `char_villager_variants` | 6 Kombinationen aus Kopf/Haar/Kleidung, damit 10–20 Bewohner nicht gleich aussehen | B |

### 5.2 Animationen (Liste für Abschnitt 9 – Quelle offen)
| ID | Beschreibung | Prio |
|---|---|---|
| `anim_locomotion` | Stehen, Gehen, Laufen, Ducken, Springen (ohne Ausdauer-Erschöpfung!) | A |
| `anim_chop` | Holz hacken (Schleife) | A |
| `anim_mine` | Stein schlagen (Schleife) | A |
| `anim_carry` | Tragen (Gehen mit Last) | A |
| `anim_build` | Hämmern/Bauen | A |
| `anim_cook` | Kochen am Feuer | B |
| `anim_eat` / `anim_sleep` / `anim_sit` | Bedürfnisse erfüllen | B |
| `anim_idle_variants` | 2–3 Leerlauf-Varianten | C |

### 5.3 Baukasten (Materialstufe Holz)
| ID | Beschreibung | Prio |
|---|---|---|
| `building_foundation_wood` | Fundament 300 × 300 | A |
| `building_floor_wood` | Boden 300 × 300 | A |
| `building_wall_wood` | Wand 300 × 300 | A |
| `building_wall_door_wood` | Wand mit Türöffnung | A |
| `building_wall_window_wood` | Wand mit Fenster | B |
| `building_door_wood` | Tür (öffnet) | A |
| `building_pillar_wood` / `building_beam_wood` | Pfosten, Balken | A |
| `building_roof_wood` / `building_roof_ridge_wood` / `building_roof_gable_wood` | Dachfläche, First, Giebel | A |
| `building_stairs_wood` | Treppe | B |
| `building_fence_wood` / `building_gate_wood` | Zaun, Tor | C |
| `trim_wood` | **Trim-Sheet** Holz (neu, verwittert) für alle Bauteile | A |

### 5.4 Arbeitsstätten und Siedlung
| ID | Beschreibung | Prio |
|---|---|---|
| `building_storage` | **Lager** der Siedlung (ein Lager pro Siedlung), gefüllt/leer sichtbar unterscheidbar | A |
| `building_woodcutter` | Holzfäller-Platz (Hackklotz, Holzstapel) | A |
| `building_stonecutter` | Steinmetz-Platz | A |
| `building_campfire` | Lagerfeuer = Kochstelle im Slice | A |
| `building_bed` | Schlafplatz (Strohlager) | A |
| `building_workbench` | Werkbank für den Spieler | B |
| `building_totem` | Siedlungszentrum/Markierung (Anführer-Symbol, eigenes Design!) | C |

### 5.5 Ressourcen und Gegenstände
| ID | Beschreibung | Prio |
|---|---|---|
| `res_tree_pine`, `res_tree_birch` | 2 Baumarten, je stehend / gefällt / Stumpf | A |
| `res_rock_small/medium/large` | Felsen zum Abbauen, sichtbar abgebaut | A |
| `res_berry_bush` | Beerenstrauch (voll/leer) | B |
| `item_log`, `item_stone`, `item_plank`, `item_berries`, `item_meal` | Gegenstände im Lager und beim Tragen | A |
| `tool_axe`, `tool_pickaxe`, `tool_hammer` | Werkzeuge (Spieler und Dorfbewohner) | A |

### 5.6 Umgebung
| ID | Beschreibung | Prio |
|---|---|---|
| `terrain_grass`, `terrain_dirt`, `terrain_rock`, `terrain_path` | Terrain-Schichten | A |
| `foliage_grass`, `foliage_bush`, `foliage_flowers` | Bodenbewuchs, günstig | B |
| `env_sky_mood` | Licht-/Himmelsstimmung (Beschreibung; Umsetzung in der Engine) | B |

### 5.7 Effekte
| ID | Beschreibung | Prio |
|---|---|---|
| `vfx_campfire` | Feuer + Rauch | A |
| `vfx_chop_chips`, `vfx_rock_dust` | Treffer-Effekte | B |
| `vfx_build_place` | Effekt beim Platzieren eines Bauteils | C |

### 5.8 UI (Lesbarkeit hat Vorrang)
| ID | Beschreibung | Prio |
|---|---|---|
| `ui_icons_resources` | Icons für alle Items aus 5.5 | A |
| `ui_icons_jobs` | Icons für die 3–4 Jobs (Vorschlag: Holzfäller, Steinmetz, Koch, Träger) | A |
| `ui_icons_needs` | Icons für 3 Bedürfnisse (Vorschlag: Hunger, Schlaf, Wärme) mit 3 Zuständen (gut/mittel/kritisch) | A |
| `ui_hud` | HUD: Gesundheit, Schnellleiste, Koop-Spielerliste (bis 8 Namen) – **kein Ausdauerbalken** | A |
| `ui_build_menu` | Baumenü mit Kategorien | A |
| `ui_storage` | Lageransicht: Bestand auf einen Blick, wenige Klicks | A |
| `ui_villager_panel` | Dorfbewohner-Übersicht: Job, Bedürfnisse, Priorität ändern | A |
| `ui_world_marker` | Markierung über Dorfbewohnern (Job + kritisches Bedürfnis) | B |
| `ui_font` | Schrift (lizenzfrei für Spiele, z. B. SIL Open Font License) | A |

### 5.9 Audio (Richtung, keine Dateien)
| ID | Beschreibung | Prio |
|---|---|---|
| `sfx_footsteps` | Schritte auf Gras, Erde, Holz | B |
| `sfx_chop`, `sfx_mine`, `sfx_build` | Arbeitsgeräusche | B |
| `sfx_fire`, `amb_settlement`, `amb_forest` | Feuer, Siedlung, Wald | B |
| `sfx_ui` | Klick, Bestätigen, Fehler | B |
| `music_settlement` | ruhige Siedlungsmusik (kein Zeitdruck!) | C |

## 6. Format deiner Lieferung
Pro Asset (oder Gruppe) **ein Abschnitt** in einer Markdown-Datei `NN_assets_<gruppe>.md` (z. B. `03_assets_baukasten.md`) mit:

| Feld | Inhalt |
|---|---|
| ID | aus Abschnitt 5 (neue Assets: neue ID vorschlagen, `snake_case`) |
| Zweck | wofür im Spiel, wer sieht es wie oft |
| Konzept | Bild (PNG) oder genaue Beschreibung, Vorder- und Seitenansicht bei Figuren und Gebäuden |
| Maße | in cm, passend zum 300-cm-Raster |
| Materialien | welche Trim-Sheets / Atlanten, Farbwerte (Hex) |
| Zustände | z. B. voll/leer, neu/verwittert, stehend/gefällt |
| MVP / Ausbau | was zuerst reicht, was später dazukommt |
| Budget-Check | geschätzte Dreiecke und Texturgröße laut 4.2 |
| KI-Hinweis | ob Bild/Text KI-generiert ist, als Eintrag: Datei / Werkzeug / Datum / Lizenz oder Quelle |
| Prompt-Paket | **Pflicht, sobald ein Asset per KI erzeugt werden soll:** vollständiges Paket nach `design/PROMPT_STANDARD.md` (alle Pflichtfelder, feste Blöcke S1/L1/H1/N1 wörtlich, Abnahme-Kriterien). Beispiele in `design/prompts/` |

Am Ende jeder Datei: **„Entscheidungen, die der Nutzer treffen muss“**.

## 7. Regeln
- **Keine Nachahmung** von Figuren, Gebäuden, Logos oder Optik der Vorbilder. Prinzipien ja, Aussehen nein.
- **Keine Ausdauerbalken** und keine Countdown-Elemente in der UI.
- **Alles KI-Generierte** wird im Asset-Log eingetragen (Steam verlangt Angaben). KI-Bilder sind zunächst **Konzepte**; ob sie ins fertige Spiel dürfen (z. B. Icons), entscheidet der Nutzer.
- Fremde Assets (Packs, Schriften, Sounds) nur mit **Quelle und Lizenz**; ohne Lizenzangabe kein Einsatz.
- **Umfang bremsen:** Wenn ein Asset das Budget sprengt oder für den Slice nicht nötig ist, sag es und schlage eine kleinere Variante vor.

## 8. Ausblick (noch nicht bearbeiten)
Vorbereitete Detail-Listen: Waffen mit Tier-Stufen in `design/ASSET_LIST_WAFFEN.md`, Gegenstände, Werkzeuge und Behälter in `design/ASSET_LIST_GEGENSTAENDE.md`, Rüstung und Kleidung in `design/ASSET_LIST_RUESTUNG.md`.

| Phase | Assets |
|---|---|
| 2 Siedlung ausbauen | Materialstufe Stein, ca. 10 Jobs und deren Arbeitsstätten, Kommandoansicht-UI, Miliz-Ausrüstung |
| 3 Kampf/Ausrüstung | Waffen, Rüstungs-Slots mit Stufen, Schmiede, Skilltree-UI, erste Gegner, Kampf-Animationen (Parieren, Haltungsleiste statt Ausdauer) |
| 4 Welt | weitere Biome (Terrain, Vegetation, Stimmung), Bosse, Quest-Figuren |
| 5 Spätspiel | Portale, Angriffswellen-Gegner, Weltoptionen-Menü |

## 9. Offene Entscheidungen (Nutzer)
1. **Art Direction** (Auftrag 1 im Briefing) – Voraussetzung für alles hier.
2. **Herkunft der 3D-Assets:** gekaufte/freie Packs (z. B. Fab; Lizenz je Asset prüfen) · eigene Modelle in Blender · Freelancer · KI-3D-Werkzeuge (Qualität und Lizenz unsicher, Steam-Angabe nötig) · Mischung.
3. **Herkunft der Animationen:** vorhandene Unreal-Animationen übertragen · gekaufte Packs · Motion Capture · selbst.
4. **Jobs und Bedürfnisse des Slice:** Vorschlag Holzfäller, Steinmetz, Koch, Träger / Hunger, Schlaf, Wärme.
5. **Lumen/Nanite** ja oder nein (beeinflusst Budgets, siehe `docs/04_technische_ziele.md`).
6. Ob KI-generierte 2D-Inhalte (Icons, Texturen) **ins fertige Spiel** dürfen.

## Erster Auftrag an ChatGPT (nach der Stilwahl)
Liefere **Abschnitt 5.3 (Baukasten Holz)** und **5.4 (Arbeitsstätten)** im Format aus Abschnitt 6, zuerst nur Priorität A.
Danach anhalten und auf Rückmeldung warten. Stelle vorab höchstens drei Rückfragen.
