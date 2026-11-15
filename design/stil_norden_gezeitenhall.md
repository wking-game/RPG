# Stil „Gelebter Norden“ mit Gezeitenhall (Mischung)

Stand 2026-11-15. Auf Wunsch des Nutzers („erstelle eine passende Mischung“) zusammengeführt aus
`design/stil_gelebter_norden.md` (Grundlage) und `design/inbox/08_stilbibel_kuestenasche.md` (Magie, Zeichen, Kreaturen, Palette).
**Festgelegt vom Nutzer (2026-11-15: „Ja, festlegen“).** Dieses Dokument ist die Stilvorgabe und ersetzt beide Quellen; die Quellen bleiben zum Nachlesen.

## Kernidee in einem Satz

**Ein glaubwürdiger, gelebter Norden, in dem man jede Jahreszeit spürt – und an seinen Rändern eine alte Kraft, der Gezeitenhall, die sich als türkises Seeglas, gebrochene Ringe und eine antwortende Natur zeigt.**

Kurz: **Alltag historisch, Ränder mythisch.** Dorf, Werkzeuge, Kleidung und Arbeit bleiben glaubwürdig (9.–11. Jh., vereinfacht). Das Fantastische gehört zu besonderen Orten, Kreaturen und Fähigkeiten, nie zum Alltagsgegenstand.

## Was woher kommt

| Bereich | aus „Gelebter Norden“ | aus „Küstenasche“ |
|---|---|---|
| Grundgefühl | glaubwürdig, nicht museal; Spuren des Lebens; warm gegen kalt | Stimmungswörter (rau, kühl, windgeformt, handgemacht, geheimnisvoll, alt, widerständig) |
| Farbe | Jahreszeit bestimmt die Farbe | feste Grundpalette mit Farbwerten (die Jahreszeit verschiebt sie) |
| Magie | Runen und Seidr als kulturelle Form, selten und leise | Gezeitenhall als Quelle: Seeglas, Lichtadern, gebrochene Ringe, Rhythmus sammeln – ausbreiten – abklingen |
| Zeichen | – | offener Ring mit drei schrägen Kerben |
| Kreaturen | – | magische Merkmale wachsen aus Körper und Umgebung |
| Menschen, UI, Klang, Technik | vollständig übernommen | Kleidung „mehrere Schichten, geflickt“ ergänzt |

## Sieben Stilregeln

1. **Glaubwürdig, nicht museal.** Häuser, Werkzeuge und Kleidung könnten so im 9.–11. Jh. im Norden existiert haben, vereinfacht für das Spiel. Keine Hörnerhelme, keine Plattenrüstung, keine leuchtenden Alltagsgegenstände.
2. **Klare Silhouetten, auch von oben.** Bäume nach Art, erntbare Pflanzen an (leicht übertriebenen) Früchten, Gebäude an der Dachform, Kreaturen an ihrem Umriss. Das Dorf muss aus der Vogelperspektive lesbar sein.
3. **Spuren des Lebens.** Ausgetretene Wege, Späne vor Werkstätten, Asche am Feuer, Ruß an der Schmiede. Neue Bauten hell und sauber, alte vergraut, bemoost, ausgebessert, erweitert.
4. **Die Jahreszeit verschiebt die Grundpalette.** Die Grundpalette (unten) gilt das ganze Jahr; die Jahreszeit färbt sie: Frühling heller und grüner, Sommer wärmer und goldener, Herbst rostiger und nebliger, Winter blau-weiß und leise. Sättigung bleibt gedämpft.
5. **Warm gegen kalt.** Herdfeuer und Fackeln warm (ca. 2000–2700 K) bedeuten Sicherheit; Nacht, Nebel und Wildnis kühl. Aktiver Gezeitenhall ist die einzige zweite Lichtfarbe: blasses Türkis.
6. **Mythos an den Rändern.** Der Gezeitenhall ist selten. Im Ruhezustand: milchiges Seeglas, das kaum oder gar nicht leuchtet. Beim Einsatz: kurzer Puls, gebrochene Ringe, die Umgebung reagiert, dann klingt er ab. Keine bunten Feuerbälle, keine Dauerleuchten.
7. **Ein Zeichen, sparsam.** Der offene Ring mit drei schrägen Kerben erscheint als Schnitzerei, Schmiedemarke, Felsader oder Form eines Effekts – klein und selten, damit er wiedererkennbar bleibt.

## Grundpalette

| Farbe | Wert | Verwendung |
|---|---|---|
| Nebelgrau | `#9AA8A5` | Himmel, Dunst, kaltes Tageslicht |
| Treibholz | `#A99B80` | helles, vergrautes Holz, Leder, trockene Fasern |
| Aschebraun | `#716957` | dunkles Holz, Erde, Alltagskleidung |
| Basaltschwarz | `#303735` | Eisen, Fels, Schatten |
| Moosgrün | `#56654F` | Wald, Flechten, Moos |
| Rostrot | `#904C3E` | Krapprot-Stoffe, Wicklungen, Nähte, Kulturakzente |
| Seeglas | `#5D9990` | schlummernder Gezeitenhall, Mineral, seltene Details |
| Gezeitenlicht | `#A9E9DC` | aktiver Gezeitenhall, sparsame helle Akzente |
| Herdfeuer | `#DB9345` | Feuer, Fackeln, Fensterlicht |

Kräftige Farben gibt es nur an Feuer, Früchten, Färbestoffen und aktivem Gezeitenhall.

## Kleidung und Kleidungsfarben (Charaktereditor)

Mehrere Schichten aus Wolle, Leinen, Leder und Fell, gemacht für Arbeit, Kälte und Reisen. Getragen: geflickt, ausgeblichen, an beanspruchten Stellen dunkler; Tätigkeit sichtbar (Ruß, Erde, Nässe, Schnee). Naturtöne als Alltag, Farbe als Wohlstand. Die Editor-Optionen bleiben, ihre Werte folgen der Palette:

| Option (`look_cloth_color`) | Name | Wert |
|---|---|---|
| `cloth_natural` | ungefärbt | `#A99B80` (Treibholz) |
| `cloth_grey` | Wollgrau | `#8A8C86` |
| `cloth_brown` | braun | `#716957` (Aschebraun) |
| `cloth_ochre` | Ocker | `#A9844A` |
| `cloth_red` | Krapprot | `#904C3E` (Rostrot) |
| `cloth_blue` | Waidblau | `#4E6475` |
| `cloth_green` | Moosgrün | `#56654F` |

Kein Türkis/Seeglas als Kleidungsfarbe: das bleibt dem Gezeitenhall vorbehalten (höchstens ein kleiner Seeglas-Einsatz an Schmuck oder Herkunftszeichen, z. B. bei der Seherin).

## Menschen

- Realistische Proportionen und Gesichter (MetaHuman), wettergegerbte Haut, Zöpfe, Bärte, Tätowierungen/Bemalung als Wahl im Editor.
- Bemalung „Runen“ und Herkunft Seherin dürfen das Kerben-Zeichen tragen; Runen glimmen nur beim Einsatz von Gezeitenhall-Fähigkeiten.

## Materialien

- **Holz:** Kiefer/Fichte, frisch honigfarben, vergraut zu Treibholz-Grau; Maserung, Kerben, Reparaturen.
- **Dächer:** Torf-/Grassoden, Reet; Wände aus Holz, Lehmflechtwerk; Feldstein-Sockel; Birkenrinde.
- **Stein:** grauer Fels und dunkler Basalt mit Moos, Flechten, an der Küste Salzränder.
- **Eisen:** dunkel, matt, Schmiedespuren, Rost an beanspruchten Stellen; wenig Bronze und Silber (Schmuck).
- **Seeglas:** milchig, leicht durchscheinend, trübe innere Schlieren; im Ruhezustand ohne eigenes Leuchten. Je seltener ein Gegenstand, desto klarer das Glas und desto eher erscheinen Lichtadern beim Gebrauch.
- **Texturen:** Foto-/Scan-Materialien (CC0 oder eigene), ca. 512 px pro Meter in der Nähe, Rauheit eher hoch, keine reinen Farben.

## Welt

Küsten, Fjorde, dichte Wälder, Hochmoore, felsige Hochebenen. Wetter prägt die Stimmung: Nebel schluckt die Ferne, Regen dunkelt Holz und Stein, Schnee vereinfacht zu hell und dunkel. Siedlungen wachsen und werden ausgebessert. Alte Steinkreise, überwucherte Wege und halb versunkene Mauern sind älter als die Siedler – dort tritt der Gezeitenhall zutage: türkise Adern in Fels und Wurzeln, Ringmuster auf Wasser, Nebel, der sich kurz in Bögen bewegt.

## Kreaturen

Klare Silhouette zuerst. Gewöhnliche Tiere ohne sichtbare Magie. „Gezeichnete“ Kreaturen mit ein bis zwei Merkmalen, die aus Körper oder Umgebung wachsen (Seeglas-Platten, durchscheinende Geweihe, Mineraladern im Fell, Nebelatem). Alte Wesen verschmelzen mit Wurzeln, Fels, Wasser oder Nebel. Die Magie reagiert auf ihr Verhalten (Adern leuchten beim Angriff, Nebel beim Einatmen).

## Gezeitenhall: Fähigkeiten und Effekte

Ein Rhythmus für alles: **sammeln – in gebrochenen Ringen ausbreiten – abklingen.**
1. Kurzer Lichtimpuls an Hand, Stab, Waffe, Kreatur oder Boden.
2. Unterbrochener türkiser Ring breitet sich aus (gern mit den drei Kerben).
3. Die Umgebung reagiert: Wasser kräuselt sich, Gras legt sich, Staub hebt sich, Nebel wird auseinandergezogen.
4. Das Licht zieht sich zurück, kurze schimmernde Spuren bleiben.

Runen und Seidr sind die menschlichen Wege, den Gezeitenhall zu nutzen: Runen werden geritzt (Seherin: „Rune des Schutzes“ als kurzer Schutzkreis), Seidr zeigt sich als Nebel und Visionen. Nicht kämpferische Fähigkeiten sind ausdrücklich gewollt: Spuren sichtbar machen, Verborgenes enthüllen, Pflanzen bewegen.

## Licht und Atmosphäre

Tag: kühles, weiches Licht, oft bewölkt. Innen und am Feuer warm. Nacht: Mond, Sterne, Feuer; aktiver Gezeitenhall ergänzt blasses Türkis und färbt nahe Flächen sanft, ohne zu überstrahlen. Bodennebel in Senken und über Wasser. Figuren und wichtige Gegenstände bleiben bei Nebel, Regen und Schnee erkennbar.

## Anzeige (UI) und Klang

- UI: so wenig wie nötig, gut lesbar; Rahmen wie geschnitztes Holz, gestanzte Lederstücke, schlichte Runenkanten. Möglichst im Bild statt auf dem Bild. Hervorhebungen und der Umgebungs-Scan dürfen die Ring-Sprache des Gezeitenhalls nutzen (die Scan-Welle als gebrochener Ring), Farbe Gezeitenlicht.
- Klang: Natur und Handwerk im Vordergrund, Musik selten und leise (Viking-Folk am Feuer, Ambient in der Wildnis); Gezeitenhall mit tiefem, hallendem Puls.

## Regeln für einheitliche Assets

- An der Silhouette erkennbar, auch klein und von oben.
- Pro Objekt höchstens ein markantes Seeglas-Detail und das Zeichen – und nur, wenn das Objekt selten oder magisch ist.
- Alltagsgegenstände matt und natürlich; aktiver Gezeitenhall hebt sich klar davon ab.
- Abnutzung, Moos und Vergrauung über gemeinsame Materialien statt pro Modell.

## Technik-Leitplanken

- 8-GB-Grafikkarte: Nanite für Fels und Gebäude, Pflanzen als Karten-Modelle mit LOD, Lumen auf Hoch, einfacher auf Niedrig.
- Gesamtgröße ≤ 20 GB: wenige, gut variierte Materialien.
- Gezeitenhall-Effekte als wenige gemeinsame Niagara-Systeme (Puls, Ring, Spuren) mit Parametern statt vieler Einzeleffekte.

## Gemeinsamer Stilprompt (für Bild- und 3D-Werkzeuge)

```text
Glaubwürdiger, gelebter nordischer Alltag im 9.–11. Jahrhundert, semi-realistisch, mit einer leisen mythischen Ebene an den Rändern der Welt. Raue, bewohnte Landschaften mit Küsten, Wäldern, Mooren und Felsen; Siedlungen aus vergrautem Holz, Grassoden- und Reetdächern, ausgebessert und gewachsen. Glaubwürdige Proportionen, handgemachte, verwitterte Materialien: Treibholz-graues Holz, dunkles Eisen mit Rost, Basalt, Moos, grobe Wolle und Leinen, Leder, Fell. Gedämpfte Farben aus Nebelgrau, Treibholz, Aschebraun, Basaltschwarz und Moosgrün; Rostrot und Ocker als sparsame Stoffakzente; warmes Herdfeuer gegen kühles Tageslicht. Die Jahreszeit färbt die Szene. Die alte Kraft heißt „Gezeitenhall“: milchig-türkises Seeglas, feine Lichtadern, gebrochene, wandernde Ringe, sichtbar nur an alten Orten, bei gezeichneten Kreaturen und beim Einsatz von Fähigkeiten; im Ruhezustand zurückhaltend. Wiederkehrendes Zeichen: ein offener Ring mit drei schrägen Kerben, klein und selten.
```
