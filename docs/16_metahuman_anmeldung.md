# MetaHuman-Figuren: Was nach der Anmeldung zu tun ist

Stand 2026-11-16. Alles, was ohne Epic-Anmeldung geht, ist erledigt. Diese Anleitung beschreibt die restlichen Klicks.
Bezeichnungen im Editor können leicht abweichen (Unreal 5.8, Sprache Deutsch/Englisch gemischt).

## Was schon vorbereitet ist

- Zwei Figuren im Inhaltsbrowser unter **Gamer → Characters → MetaHuman**:
  - `MHC_Kraeftig` (Körperbau „kräftig“, 182 cm, breite Schultern) – ersetzt später Manny
  - `MHC_Schmal` (Körperbau „schmal“, 170 cm) – ersetzt später Quinn
  - Körpermaße und Hautton sind nach der Stilvorgabe (`design/stil_norden_gezeitenhall.md`) vorbelegt: wettergegerbt, etwas rauere Haut. Körperbau ist nicht an ein Geschlecht gebunden.
- Projekteinstellungen für MetaHumans sind aktiv (hast du am 15.11. selbst bestätigt).
- Das Spiel benutzt die MetaHumans automatisch, sobald sie zusammengebaut und verknüpft sind; bis dahin bleiben Manny und Quinn. Animationen bleiben dieselben.
- `MHC_Grundfigur` (Ordner Characters) ist dein erster Versuch und wird nicht gebraucht.

## Schritt für Schritt

**Vorher:** Editor nur über `Gamer.uproject` oder `E:\UE58\Engine\Binaries\Win64\UnrealEditor.exe` öffnen, **nie über den Epic Launcher**.

### 1. Anmelden (einmal)
1. Inhaltsbrowser → Gamer → Characters → MetaHuman → **`MHC_Kraeftig` doppelklicken**. Der MetaHuman Creator öffnet sich.
2. In der Werkzeugleiste oben die **Anmelde-Schaltfläche** (Epic-Konto) klicken.
3. Im Browser mit deinem Epic-Konto anmelden und bestätigen. Der Editor wartet nur ca. 90 Sekunden darauf.
4. Erfolg: Die Schaltfläche zeigt deinen Namen bzw. „angemeldet“.

### 2. Aussehen anpassen (freiwillig, ohne Anmeldung möglich)
Nach Geschmack, passend zum Stil: Kopf/Gesicht wählen, **Haare: Zöpfe oder lang**, **Bart: Vollbart oder geflochten**, Haarfarbe blond, rot, braun oder grau. Körperbau bitte nicht stark ändern (das Spiel ordnet „kräftig/schmal“ zu).

### 3. Rig erzeugen (braucht Anmeldung)
Bereich **„Rig“** (bzw. „Auto-Rig“) → **„Create Full Rig“ / „Vollständiges Rig erstellen“**. Dauert 1–3 Minuten. Erfolg: Meldung „rigged“, kein Fehler im Nachrichten-Log.

### 4. Texturen holen (braucht Anmeldung)
Bereich **„Textures“** → **„Download Texture Sources“**, Auflösung **2k** (reicht, spart Speicher). Dauert einige Minuten.

### 5. Zusammenbauen – zweimal je Figur
Bereich **„Assembly“ / „Zusammenbauen“**:

| Durchgang | Pipeline / Qualität | Zielordner („Root Directory“) |
|---|---|---|
| Spieler | **UE Optimized, High** | `/Game/Gamer/Characters/MetaHuman/Kraeftig` |
| Bewohner, Entfernung | **UE Optimized, Low** | `/Game/Gamer/Characters/MetaHuman/Kraeftig_Low` |

Jeweils **„Assemble“** klicken und warten. Danach **Strg+S** (alles speichern).

**Wenn sich der Zielordner nicht einstellen lässt:** Der Creator schreibt dann nach `/Game/MetaHumans/MHC_Kraeftig` (bzw. `MHC_Schmal`). Das ist in Ordnung, mein Verknüpfungs-Skript findet auch diesen Ordner, aber nur für die **High**-Fassung (Spieler). Den Low-Durchgang dann weglassen; Bewohner nehmen die High-Figur und überspringen deren feinste Detailstufen.

**Speicher (Absturz „Nicht genügend Arbeitsspeicher“, 16.11.):** Das Zusammenbauen braucht viel RAM.
- Auslagerungsdatei vergrößern (fest, 32–48 GB auf D:, danach Neustart).
- Vor dem Zusammenbau alle anderen Programme schließen (Browser, Spiele-Tests).
- **Eine Figur pro Editor-Sitzung:** Kraeftig zusammenbauen, speichern, Editor schließen und neu öffnen, dann Schmal.
- Reicht es weiterhin nicht: Texturen mit **1k** statt 2k holen (Schritt 4 wiederholen).

### 6. Dasselbe für `MHC_Schmal`
Schritte 3–5 mit `MHC_Schmal`, Zielordner `…/MetaHuman/Schmal` und `…/MetaHuman/Schmal_Low`.

### 7. Editor schließen und Bescheid geben
Editor zu, dann sag mir Bescheid. Ich führe aus:
```
tools\metahuman.ps1 Link
```
Das findet die zusammengebauten Körper und Köpfe, macht ihr Skelett mit den vorhandenen Animationen verträglich und schreibt `game/Content/Data/figures.json`. Danach baue und teste ich.

## Stand 16.11.: Zusammenbau per Skript erledigt

Nach Anmeldung und Auto-Rig (vom Nutzer im Creator) läuft der Rest ohne Klicks:
```
tools\metahuman.ps1 Assemble -Figure MHC_Kraeftig
tools\metahuman.ps1 Assemble -Figure MHC_Schmal
tools\metahuman.ps1 Link
```
- `Assemble` holt fehlende Textur-Quellen, baut High und Low und speichert. Es startet einen **normalen Editor** (Fenster erscheint und schließt sich selbst), weil TextureGraph (backt die Gesichtstexturen) im Hintergrund-Commandlet gesperrt ist bzw. mit `TextureGraph.AllowCommandlets=1` abstürzt.
- Ergebnis: Kraeftig 205 MB / Kraeftig_Low 116 MB (Schmal ähnlich), `figures.json` mit 4 Figuren.
- Grafiktest (1 Spieler, Bewohnerin Astrid): Spieler = MetaHuman Kraeftig, Bewohnerin = Schmal_Low; Animationen laufen, Log `Figure strong: body ok, face ok`. 140 fps bei Mittel, 960×540.
- **Offen:** Spielerfigur hat keine Haare, beide tragen nur die Creator-Unterwäsche (die Tönung der Zwischenfigur wirkt auf MetaHumans nicht). Haare/Kleidung im Creator wählen und `Assemble` erneut laufen lassen.
- **VRAM-Messung 16.11.** (Spieler + 15 Bewohner im Bild, Mittel, 960×540, nvidia-smi alle 2 s): Grafikkarte gesamt höchstens 1562 MiB, davon ca. 580 MiB Desktop ohne Spiel → Spiel (inkl. lokalem Server) ca. 1,0 GB. 144 fps (Begrenzung erreicht). Ziel 8 GB weit unterschritten; bei Vollbild/Hoch und größerer Welt nachmessen.
- Alle Bewohner eines Körperbaus sehen gleich aus (ein Gesicht je Figur). Mehr Abwechslung bräuchte weitere MetaHumans oder Variationen (Haare, Hautton) – noch nicht umgesetzt.

## So prüfst du, ob es geklappt hat

- In den vier Zielordnern liegen Skeletal Meshes mit „Body“ und „Face“ im Namen.
- Nach meinem `Link`-Schritt steht im Spiel-Log: `Figures: 4 MetaHuman figure(s)` und `Figure strong: body ok, face ok`.
- Im Spiel (`Spiel_starten.bat`) ist deine Figur ein MetaHuman statt der grauen Puppe; Bewohner ebenfalls.

## Detailstufen (Ziel 8 GB Grafikspeicher)

- **Spieler:** „High“-Zusammenbau (feine Detailstufen nah, gröbere automatisch auf Entfernung).
- **Bewohner:** „Low“-Zusammenbau; fehlt er, nimmt das Spiel den Spieler-Zusammenbau und überspringt dessen zwei feinsten Detailstufen.
- Skin Cache ist an (für MetaHuman-Köpfe nötig) und kostet etwas Grafikspeicher; nach dem Zusammenbau messe ich mit 10–20 Bewohnern nach.

## Was nur mit Anmeldung geht / was offen ist

- Rig, Texturen und Zusammenbau gehen nur angemeldet (Epics Cloud-Dienst).
- Kleidung: Für diesen Schritt reicht die Standardkleidung des Creators in gedämpften Farben; eigene Wikinger-Kleidung kommt später.
- Gesichtsanimation (Mimik) ist noch nicht angebunden: Der Kopf folgt dem Körper, das Gesicht bleibt neutral.
- Ungeprüft, bis der Zusammenbau da ist: ob die Ordner- und Dateinamen des Zusammenbaus zu `tools/unreal/metahuman_link.py` passen (sucht „Body“/„Face“ im Namen) und wie gut die Animationen auf den MetaHuman-Proportionen sitzen.

## Kleidung (eigene Umsetzung, Stand 21.11.)

`tools\clothing.ps1 All` (Export → Build → Import, Unreal-Schritte mit Sperre):
- **Export:** Körper Kraeftig/Schmal als FBX nach `game/Saved/Clothing` (läuft im normalen Editor-Fenster; als Commandlet stürzt der Skeletal-Mesh-Export ab).
- **Build:** `tools/blender/build_clothing.py` schneidet die Kleidung aus der Körperoberfläche (behält die Skin-Gewichte), glättet sie, damit sie locker sitzt, und ergänzt Röcke und Gürtel als Ellipsen-Röhren; deren Gewichte kommen nur von Rumpf und Beinen (sonst folgen sie den Händen). Vorschau: `tools/blender/preview_clothing.py`.
- **Import:** `/Game/Gamer/Characters/Clothing/<Körper>/SKM_Cloth_<Körper>_<Tunika|Kleid>`, Material `M_Cloth` (Parameter `Tint`, `Roughness`), Slots Wool/Trim/Linen/Leather.
- **Spiel:** `GamerFigures::ApplyClothing` (Leader Pose auf dem Körper). Spieler: Tunika in der gewählten Kleiderfarbe. Bewohner: Farben aus der Stilpalette nach Name, schmale Figuren zur Hälfte im langen Kleid.
- Dazu (21.11.): **Schürzenkleid** (Kleid + Schürze vorn/hinten, zwei ovale Fibeln, Perlenbogen) und **Umhang** als eigene Schicht (Schulterteil, Rückenbahn bis Knie, Fibel). Spieler: Tunika + Umhang. Bewohner: schmale Figuren halb Kleid (davon halb mit Schürze), ein Drittel mit Umhang; Seherin immer Schürzenkleid + Umhang.
- Haare (gelöst 22.11.): Die Gegenprüfung mit den unveränderten MetaHuman-Blueprints (`tools/unreal/hair_check.py`, Szenen-Kamera, Bilder in `game/Saved/HairCheck`) zeigte: Low-Blueprints haben eine `LODSyncComponent`, die Haare/Bärte fest auf die Haarhauben-LODs 5 (nah) bis 7 (fern) legt; Stufe 0 ist in der Low-Fassung leer. `GamerFigures` erzwingt deshalb für Low-Figuren Groom-LOD 5 (`FFigure::GroomLOD`). Seitdem haben alle Bewohner und die Spieler Haare. Die High-Fassung zeigt die lange Frisur `Hair_M_Layered` selbst im Blueprint nicht (Bart schon) – offen; Spieler nutzen bis dahin die Low-Fassung. Strähnen nah (Generalanweisung) brauchen die funktionierende High-Fassung bzw. einen Mittel-Zusammenbau; noch nicht umgesetzt, ebenso Haarsimulation/Wind.
- Hinweis Werkzeug: `-ExecutePythonScript` beendet den Editor nach dem Skript; für Skripte mit Tick-Rückruf `-ExecCmds="py <datei>"` nehmen.
- Messung 21.11. (1920×1080, Qualität Episch, Spieler + 15 Bewohner mit Kleidung): Grafikspeicher Spiel ca. 2,45 GB (nvidia-smi 4,25 GB gesamt minus 1,8 GB andere Programme), 60 fps mit V-Sync. 8 GB sind absolutes Maximum.
- Offen: Fellkragen, Lederweste (Referenzbilder `design/referenz/figuren`); Stoffstruktur nur als feines Rauschen; Bewohner-Haare noch unsichtbar.

## Spielerfiguren in Mittel-Qualität (Haarbewegung), Stand 2026-11-22
- `tools/metahuman.ps1 Assemble -Figure MHC_Kraeftig -Quality Mid` (ebenso MHC_Schmal) baut „UE Optimized Medium“ nach `.../Kraeftig_Mid` bzw. `.../Schmal_Mid`; `Link` trägt sie als `strong_mid`/`slim_mid` ein (figures.json: 18 Figuren). Die Spielerfigur nimmt zuerst `*_mid`, dann `*_low`, dann High.
- Die Medium-Blueprints ordnen die Haar-LODs per LODSync auf 3 (nah), 5 und 7 (fern) zu. In diesem Build zeigen die Stufen 0 bis 2 kein Haar; getestet: Bei Stufe 2 ist die Figur kahl. `GamerFigures` erzwingt für `*_mid` deshalb Groom-LOD 3 und schaltet die Simulation ein (`bSimulateHair`, Wind über `SetHairWind`). Die Bewohner bleiben bei LOD 5 (Helm, unbewegt).
- Epic liefert die Kartenstufen 3 und 4 mit ausgeschalteter Simulation aus (`Simulation = Disable`). `tools/unreal/metahuman_hairsim.py` schaltet sie für Haar und Bart auf `Enable`; die Augenbrauen bleiben ohne Physik. `metahuman_assemble.py` ruft das nach jedem Mid-Build auf, es lässt sich aber auch einzeln starten.
- Prüfung im Spiel: Beim Erzeugen schreibt `GamerFigures` je Haargruppe die Zeile „Hair <Asset> group n: … asset at LOD 3: 1, component: 1“. Für die kräftige Figur (`-GamerCharacter=origin_warrior:Ragnar`) bestätigt das Log, dass Haar und Bart simulieren. Die Zöpfe der schmalen Figur sind auf Stufe 3 ein starres Mesh ohne Physik und bewegen sich daher nicht; bei eng geflochtenen Zöpfen ist das vertretbar.
- Grafikspeicher (Test 1920×1080, 1 Spieler): höchstens 3,3 GB gesamt, deutlich unter der 8-GB-Grenze.
- Sichtprüfung: Auf den Standbildern von oben sind nur kleine Formänderungen beim Sprung zu erkennen, weil das Haar kurz ist. Eine deutlich sichtbare Bewegung bräuchte eine lange Frisur; das gehört zum Anbinden der Frisurwahl aus dem Charaktermenü (noch offen).
- Tool: `metahuman.ps1` wartet jetzt mit `WaitForExit()` statt `-Wait`, weil `-Wait` auch auf zenserver wartete und der Lauf deshalb hing.

## Frisur, Bart und Haarfarbe aus dem Charaktermenü, Stand 2026-11-22
- **Haar-Sätze:** Für jeden Körper gibt es zusätzliche Mittel-Fassungen `MHC_<Kraeftig|Schmal>_HK<n>`, die sich nur in Haar und Bart unterscheiden (Tabelle `metahuman_looks.py`, `HAIR_KITS`). Ordner `.../<Body>_HK<n>_Mid`.
  - Zusammengebaut wird mit `metahuman.ps1 Variants -Figure ...`, danach `Assemble -Figure ... -Quality Mid`.
  - `Link` schreibt `hair_kits` nach figures.json: je Körper 7 Optionen.
- **Zuordnung:**

| Menü-Option | Haar bzw. Bart |
|---|---|
| hair_short | Hair_S_Messy |
| hair_long | Hair_L_MessyClumps |
| hair_braids | Hair_M_TwistedBraids |
| hair_undercut | Hair_M_FauxMohawk |
| beard_full | Beard_L_Full |
| beard_braided | Beard_L_Messy (MetaHuman hat keinen geflochtenen Bart) |
| beard_stubble | eingebackener Stoppelbart (siehe unten) |
| hair_none / beard_none | kein Haar bzw. kein Bart |

- **Stoppelbart:** Er ist in der Mittel-Fassung kein eigenes Haar-Asset, sondern in die Gesichtstextur eingebacken. Die Option liefert deshalb das Gesichtsmodell des HK1-Satzes. Es hat dieselbe Topologie, sodass die übrigen Haare weiter passen.
- **Spiel:** `AGamerCharacter::ApplyBodyLook` → `GamerFigures::ApplyHairChoice` ersetzt Haar und Bart der Spielerfigur; die Augenbrauen bleiben. Wechselt das Haar, werden die Groom-Komponenten neu erzeugt.
- **Testschalter:** `-GamerLook=look_hair:hair_long,look_beard:beard_braided,look_hair_color:hair_color_red`.
- **Getestet:**
  - Kräftig mit langem Haar und Wildbart: sichtbar, Haar und Bart simulieren laut Log.
  - Schmal mit kurzem Haar und Stoppelbart: sichtbar.
  - Kurzes Haar hat auch in Epics Asset keine Physik.
- **Haarfarbe (gelöst 2026-11-22):** Die Groom-Assets selbst verweisen nur auf Epics allgemeine Materialien `MI_Hair*`. Die Farbe steckt in den Überschreibungen `MI_WI_*` des zusammengebauten Blueprints.
  - `Link` liest diese Überschreibungen aus dem Blueprint und schreibt sie als `materials` je Groom nach figures.json, auch für die Haar-Sätze.
  - `GamerFigures::ApplyGrooms` setzt sie, `SetHairColour` färbt dann zur Laufzeit über `hairMelanin`, `hairRedness` und `WhiteAmount`.
  - Mit Bildern geprüft (`game/Saved/MpTest/haarfarben.png`; rot, blond, braun, grau, asch, schwarz): Rot, Schwarz, Braun, Blond und Grau sind klar zu unterscheiden. Asch liegt in praller Sonne nahe bei Blond.
  - Einbacken je Farbe ist damit unnötig. Jede zusätzliche Farbe kostet **keinen** Speicher außer einer dynamischen Materialinstanz pro Groom (wenige KB). Sie braucht also weder Installationsgröße noch Grafikspeicher.
- **Größe:** Jeder Haar-Satz belegt im Projekt etwa 140 MB (Editor-Daten), weil Gesicht und Körper mitgebaut werden. Gekocht ist es weniger, aber noch nicht gemessen.

## Umhang und Röcke beim Sprung, Stand 2026-11-22
- **Ursache:** Rock, Tunikasaum, Schürze und die Rückenbahn des Umhangs übernahmen die Hautgewichte der nächstgelegenen Körperfläche, unterhalb der Hüfte also fast ganz Oberschenkel und Waden. Beim Hochziehen der Beine (Sprung) blähte sich der Stoff deshalb auf.
- **Lösung:** `build_clothing.py` enthält `soften_leg_weights`. Unterhalb der Hüfte folgen diese Teile jetzt dem Becken und nur anteilig dem Oberschenkel ihrer Seite: von 0 an der Hüfte bis 35 % am Saum, beim Umhang bis 20 %. Wade und Fuß wirken nicht mehr.
- **Umgesetzt:** neu gebaut und importiert (`tools/clothing.ps1 Build`, danach `Import`).
- **Test:** Sprung mit Bildserie, Bild `game/Saved/MpTest/umhang_sprung.png`.
  - Schmale Figur: Der Umhang schwingt beim Sprung leicht aus, bläht sich aber nicht mehr auf.
  - Kräftige Figur: Der Sprung kam in der Bildserie nicht deutlich ins Bild, nur Stehen und Gehen sind belegt.
- **Grenze:** Das ist Skinning, keine Stoffsimulation; der Stoff weht nicht im Wind. Echte Stoffsimulation (Chaos Cloth) wäre ein eigener Schritt und kostet Rechenzeit, aber kaum Grafikspeicher.
