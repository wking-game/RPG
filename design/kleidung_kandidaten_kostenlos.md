# Kleidung für die Figuren: kostenlose Kandidaten (Fab, geprüft 2026-11-16)

Vorgabe des Nutzers: eigene Umsetzung, angelehnt an die Figuren von Aska und Enshrouded (mehrere Schichten Wolle, Leinen, Leder, Fell; nichts davon nachbauen), **nur kostenlose Assets**. Stilfarben: `design/stil_norden_gezeitenhall.md`.
Noch nichts heruntergeladen (Download und Fab-Konto: nur durch den Nutzer bzw. nach ausdrücklicher Freigabe). Lizenzen vor Einbau in `design/asset_log.md` eintragen.

| Asset | Was | Passt zu MetaHuman? | Lizenz | Bewertung |
|---|---|---|---|---|
| [Poor man Outfit](https://www.fab.com/listings/ebeea391-8264-44ef-8cb0-f4801ffea4a6) (NOETA Marketstore) | ganzes Outfit, zerschlissen, Schichten | **ja**, parametrisch (.mhpkg, `WI_PoormanOutfit` im Creator) | Fab Standardlizenz, Stufe „kostenlos“ wählbar (bis 22,64 € für Profi-Stufe) | **bester Kandidat** für Bewohner: geht direkt in den Creator, passt sich kräftig/schmal an. Wüsten-Look, Farben lassen sich evtl. anpassen. Bewertung 3,5/5. |
| [Medieval Shirt Lace Up](https://www.fab.com/listings/552462f7-e15e-463f-b305-459c733bba8b) (Clothcraft Studio 3D) | Leinenhemd mit Schnürung | nein, nur Mesh (fbx/obj/Maya), nicht geriggt | Fab Standardlizenz, kostenlos | gute Grundlage für eigene Tunika in Blender (an MetaHuman-Körper anpassen, riggen) |
| [Medieval Hooded Cloak](https://www.fab.com/listings/4bef3f38-bfdc-4623-b081-5e6fd9eb41ee) (Paradoox) | Kapuzenumhang, 4K PBR | nein, nur Mesh | Fab Standardlizenz, kostenlos | Umhang für Seherin/Jäger, in Blender anpassen |
| [Viking Outfit](https://www.fab.com/listings/36ffca37-c25b-4d81-bbae-d09c98915c1a) / [Viking Man Outfit](https://www.fab.com/listings/aa53f092-c884-48ab-b162-018c17b6570b) (a9908244) | Wikinger-Kleidung mit Fell | nein, nur glb | **CC-BY** (Namensnennung nötig; Felltexturen von einer DeviantArt-Quelle, Lizenz dort extra prüfen) | Optik am nächsten an Wikinger-Tracht; Qualität/Topologie unbekannt, erst ansehen |
| [Viking](https://www.fab.com/listings/ca4ba583-8d90-4069-b51f-50e694530b2f) (Art.Hiraeth) | fertige Figur mit eigenem Skelett | nein | keine Lizenzangabe auf der Seite | **nicht geeignet** (eigene Figur, Lizenz unklar) |
| MetaHuman Techwear, Jeans, Hoodie, Survivor (Epic, polycorn) | moderne Kleidung | ja | kostenlos | passen nicht zum Setting |

Kostenpflichtig gesehen (nur zur Information, nicht gewählt): Medieval Commoner Outfit (38,84–49,63 €), Parametric Viking Warrior Outfit Pack Vol 1 (37,75–64,74 €, UE 5.6–5.7).

## Referenzbilder des Nutzers (2026-11-16)

Erstes Bild (Figurenauswahl 3 × 8) nur im Chat; vier weitere liegen in `design/referenz/figuren/figuren_1..4.jpg` (KI-generierte Vorlagen des Nutzers, nur Referenz, nicht im Spiel verwendet). Ergänzungen aus diesen: viele Hauttöne (hell bis dunkel, ostasiatisch), Zöpfe mit Perlen, Cornrows, Kranzzopf bei älteren Frauen, geflochtene Bärte mit Perlen, Stirnbänder, Fellkragen, Karo-Umhänge, Schürzenkleider in Rot/Ocker. Daran ausrichten:
- **Frauen:** knöchellange Wollkleider (Moosgrün, Waidblau, Rostbraun) mit helleren Leinen-Unterkleidern, Schürzenkleid mit zwei ovalen Fibeln und Perlenketten dazwischen, Ledergürtel mit Tasche; Haare in langen Zöpfen (blond, rot).
- **Männer:** Tuniken mit gewebter Borte am Halsausschnitt (Grün, Blau, Rostrot), Lederweste darüber, breite Ledergürtel, Thorshammer-Anhänger; einzelne mit Fellumhang, Kapuzenmantel mit Fibel oder Kettenhemd/Lederrüstung (Krieger-Herkunft). Haare zurückgebunden, Dutt, Seitenschnitt oder lang; Vollbärte, teils geflochten; ein grauhaariger Älterer.
- Haut meist hell und wettergegerbt, einzelne dunklere Figuren.
- Farben passen zur Stilpalette (keine Türkis-Kleidung).
Haare/Bärte/Haut sind danach in `tools/unreal/metahuman_looks.py` gesetzt; die Kleidung ist der Maßstab für die eigene Tracht.

## Vorschlag

1. **Sofort:** „Poor man Outfit“ (kostenlose Stufe) als Bewohner-Grundkleidung im Creator; Farben auf die Stilpalette (Wollgrau, Aschebraun, Ocker, Waidblau, Moosgrün) stellen, sofern das Outfit Farbparameter hat.
2. **Danach, eigene Umsetzung:** Hemd (Lace Up), Umhang (Hooded Cloak) und Wikinger-Teile (CC-BY) in Blender als Ausgangsbasis, an den MetaHuman-Körper anpassen, Schichten ergänzen (Wollkittel, Gürtel, Fellkragen, Wickelgamaschen), riggen, als eigene Kleidungsstücke einbinden. Größere Aufgabe, eigene Sitzung.
