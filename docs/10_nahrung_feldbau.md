# Nahrung, Obst, Saatgut und Feldbau – Anforderungen

Stand 2026-11-14. Sammlung der Nutzer-Wünsche (Chat 2026-11-14) und der Ergänzungen von ChatGPT (Komposter, Regenfänger). Umsetzung offen; Entscheidungen stehen in `docs/01_entscheidungen.md`.

## Wünsche des Nutzers (wörtlich zusammengefasst)

1. **Obst nur an Bäumen: Äpfel und Birnen.** Es geht mit der Jahreszeit (Blüte → wächst → reif), wird es nicht geerntet, **fällt es herunter**, bekommt je nach **Fallhöhe Schaden** (Druckstellen) und **verdirbt**, wenn man es nicht aufsammelt.
2. **Feldfrüchte:** Zwiebeln, Möhren, Kartoffeln, Flachs.
3. **Saatgut:** Aus den Früchten gibt es Samen. Samen werden auf Feldern **ausgesät**, das Feld wird **gedüngt**; die Ernte wird zu **Lebensmitteln** oder zu **Dünger**.
4. **Wildpflanzen** aus dem Referenzbild (`design/` folgt): Walderdbeere, Blaubeere, Himbeere (gibt es schon), Brombeere, Hagebutte, Schlehe, Holunder, Zuckermelone.
5. Aus der Gameplay-Liste vom selben Tag: **Verderb** mit Hinweis bei 30 % Restzeit und Anfänger-Tipp („verderbliche Gegenstände trocken und geschützt lagern“); **Komposter** (verdorbene Nahrung → Kompost → Dünger); **primitiver Regenfänger** (1 Holzstamm, 1 großer Stein, 3 kleine Steine, 5 Äste) für Trink- und Gießwasser.

## Kreislauf

Sammeln/Ernten → Essen oder Lagern → (verdirbt) → Komposter → Dünger → Feld (Saatgut + Wasser + Dünger) → Ernte → Saatgut zurück, Rest Essen/Dünger.

## Vorschlag zur Umsetzung (Claude, noch nicht bestätigt)

- **Obstbaum-Zyklus** an die Jahreszeit (`SeasonPhase`) gekoppelt: Frühling Blüte, Sommer grüne Früchte, Spätsommer/Herbst reif (erntbar), danach fallen ungeerntete Früchte nach und nach als einzelne Gegenstände (Actor am Boden, vom Server gesetzt). Schaden = Fallhöhe: unter 2 m keiner, darüber „angeschlagen“ (verdirbt doppelt so schnell). Winter: keine Früchte (behebt „Äpfel hängen im Winter am kahlen Baum“).
- **Verderb** als Eigenschaft der Gegenstände in `items.json` (Haltbarkeit in Spieltagen; schneller am Boden/im Regen, langsamer im Lager unter Dach). Hinweis bei 30 % Restzeit, einmaliger Anfänger-Tipp.
- **Saatgut** als eigene Gegenstände (`seed_apple`, `seed_pear`, `seed_onion`, `seed_carrot`, `seed_potato` = Saatkartoffel, `seed_flax`), entsteht beim Verarbeiten/Essen bzw. als Teil der Ernte.
- **Feld** als Baustück (Beet), Zustände: leer → gesät → wächst (Stufen) → reif; Dünger beschleunigt bzw. erhöht den Ertrag, Wasser nötig (Regen oder Gießen aus dem Regenfänger). Wachstum an die Jahreszeit gebunden (Winter kein Wachstum).
- **Flachs** liefert Fasern (Kleidung, Seile) und Samen (Leinsamen, essbar).

## Offene Fragen an den Nutzer

- Historische Passung: Kartoffeln (erst ab 16. Jh. in Europa) und Zuckermelone (nicht im Norden der Wikingerzeit) – trotzdem rein? Vorläufig: Kartoffeln rein (ausdrücklicher Wunsch), Melone vorerst nicht.
- ~~Zeitpunkt~~: beantwortet 2026-11-14, alles jetzt.
- ~~Bauer~~: beantwortet 2026-11-14, ja, über den Hof (Arbeitsplätze, Ausbau).
- ~~Himbeere~~: bleibt ganzjährig (Nutzer 2026-11-14).
- ~~Bäume pflanzen~~: ja, vom Setzling (Nutzer 2026-11-14), gebaut.
- ~~Kartoffeln~~: bleiben (Nutzer 2026-11-14).

## Umsetzung (Stand 2026-11-14 abends, Session „Obst, Saatgut und Feldbau“)

Alle Zahlen unten sind **Vorschläge von Claude** (⚑), soweit nicht anders vermerkt. Sie stehen in den Daten und lassen sich dort ändern. Ein Spieltag dauert 30 Minuten, eine Jahreszeit 60 Minuten, also zwei Spieltage. Jahreszeit-Phasen: 0 = Frühlingsanfang, 1 = Sommer, 2 = Herbst, 3 = Winter.

### Verderb (`items.json`: `spoil_days`, optional `spoils_into`)
- Die Haltbarkeit in Spieltagen gilt „getragen“, also im Inventar oder in der Hand eines Bewohners. Das Lager (unter Dach) verlängert sie, aber nie unbegrenzt: Faktor 0,25, also viermal so lang ⚑. Am Boden verdirbt es doppelt so schnell ⚑.
- Höhere Stufe hält länger (bestätigt). Formel: × (1 + 0,5 × (Stufe − 1)) ⚑.
- Werte: Obst, Beeren, Pilze und Gemüse 2,5 Tage (bestätigt am 2026-11-12). Fallobst mit Druckstellen 1 Tag ⚑. Gekochtes (Mahlzeit, Eintopf) 8 Tage ⚑; bestätigt ist nur „deutlich deutlich langsamer“. Haselnüsse und Leinsamen verderben nicht ⚑.
- Verdorbenes wird zu `item_spoiled_food`; am Boden verrottet es nach einem halben Spieltag ganz.
- Stapel mit unterschiedlicher Frische werden zusammengelegt, ihre Frische als Mittelwert.
- Hinweis bei 30 % („Bald verdorben: …“), einmaliger Anfänger-Tipp, Meldung beim Verderben; im Lager als Meldung an alle. Frische pro Platz: `FGamerInventorySlot::Freshness`, im Lager `FGamerItemAmount::Freshness`. Einen Haltbarkeitsbalken am Inventarplatz gibt es noch nicht (Anzeige liegt bei der Gameplay-Session).

### Obst und Wildpflanzen (`resource_nodes.json`: Block `season`)
| Vorkommen | Blüte | unreif | reif | fällt ab | kahl | Früchte/Jahr |
|---|---|---|---|---|---|---|
| Wildapfelbaum | 0,3 | 0,6 | 1,8 | 2,2 | 2,9 | 8–14, je Pflücken 5 |
| Wildbirnbaum (neu) | 0,25 | 0,55 | 1,6 | 2,0 | 2,7 | 6–12, je Pflücken 5 |
| Walderdbeere | 0,25 | 0,5 | 0,8 | – | 1,5 | 4–8 |
| Blaubeere | 0,35 | 0,65 | 1,2 | – | 2,0 | 8–14 |
| Brombeere | 0,8 | 1,1 | 1,6 | – | 2,6 | 8–12 |
| Hundsrose (Hagebutten) | 0,9 | 1,2 | 2,2 | – | 3,5 | 6–10 |
| Schlehe | 0,2 | 0,5 | 2,4 | – | 3,5 | 8–12 |
| Holunder | 0,8 | 1,1 | 1,8 | – | 2,5 | 10–16 |

- Alle Phasen ⚑. Hagebutte und Schlehe reichen bis in den frühen Winter (botanisch: Ernte nach dem ersten Frost). Obstbäume tragen im Winter nichts.
- Was ungepflückt hängen bleibt, fällt zwischen „fällt ab“ und „kahl“ nach und nach als einzelner Gegenstand herunter. Fallhöhe 1,2–4,2 m beim Apfel, 1,5–5,5 m bei der Birne ⚑. Über 2 m gibt es Druckstellen ⚑, das Obst wird dann zu „Fallobst (…, Druckstellen)“. Aufheben mit E.
- Die Himbeere („Beeren“) bleibt ganzjährig wie bisher. Sonst gäbe es in einer neuen Welt (Start im späten Frühling) erst nach rund 40 Minuten die Beeren für die erste Mahlzeit. ⚑, bitte entscheiden.
- Holunderbeeren sind roh giftig. Die Bewohner essen sie nicht roh, am Lagerfeuer werden 4 davon zu einer Mahlzeit.

### Saatgut und Felder
- Erstes Saatgut liefern Wildvorkommen ⚑: Wilde Möhre (Möhrensamen), Wildzwiebel (Zwiebelsamen), Wilder Flachs (Leinsamen), verwilderte Kartoffelstaude (Saatkartoffeln, historisch unpassend, siehe offene Frage). Danach kommt es aus der Ernte.
- Ertrag je Einheit (4,5 m² ⚑):

  | Saat | Wachstum | Ertrag |
  |---|---|---|
  | Zwiebelsamen | 2 Tage | 3–5 Zwiebeln, 1–2 Samen |
  | Möhrensamen | 2 Tage | 3–5 Möhren, 1–2 Samen |
  | Saatkartoffel | 2,5 Tage | 4–6 Kartoffeln |
  | Leinsamen (Flachs) | 2 Tage | 4–6 Pflanzenfasern, 2–3 Leinsamen |

  Dünger bringt +50 % Ertrag ⚑. Alles ⚑.
- **Feld aufziehen** (Nutzer 2026-11-14): Feld wählen, erster Klick setzt die Ecke, zweiter Klick die Gegenecke. Je Seite 2 m bis 12 m ⚑. Das Feld liegt auf der tieferen Ecke; die Pflanzen werden einzeln auf den Boden gesetzt.
- Kosten je Einheit: 2 Äste und 1 Stein ⚑. Pro Einheit braucht das Feld 1 Saatgut zum Säen, 1 Wasser für eine volle Füllung und 1 Dünger.
- Wasser reicht für einen Spieltag ⚑. Ohne Wasser und im Winter (Phase 3–4) wächst nichts, das Feld stirbt aber nicht (Regel 6).
- E am Feld macht jeweils das Nächste: ernten → säen → gießen → düngen → Zustand anzeigen.

### Obstbäume selbst aufziehen (Nutzer 2026-11-14: „Vom Setzling aufziehen“)
- Kerne: 1 Apfel oder 1 Fallobst ergibt 3 Kerne (Handrezept), bei der Birne genauso ⚑.
- Die Kerne sät man im Feld. Nach 2 Spieltagen gibt jede Einheit einen Setzling ⚑.
- Pflanzen über das Baumenü: „Apfelbaum pflanzen“ oder „Birnbaum pflanzen“ kostet 1 Setzling, ohne Werkzeug. Der Baum wächst in 4 Spieltagen ⚑ vom Setzling über den Jungbaum zum tragenden Baum und folgt danach dem Jahreszyklus wie die wilden Bäume.
- Daten: `buildables.json` `farm.kind = "tree"` mit `node` und `grow_days`; die Wachstumsmodelle stehen in `resource_nodes.json` unter `meshes.growing`.
- Grenze: Im Feld stehen die Setzlinge als Modell im 50-cm-Raster und wirken dichter als die tatsächliche Zahl (1 je Einheit).

### Komposter, Regenfänger
- Der Komposter nimmt Verdorbenes und Fallobst mit Druckstellen, bis zu 12 Stück. Aus je 3 entsteht 1 Kompost (Dünger), einer dauert 0,25 Spieltage ⚑.
- Regenfänger: 1 Baumstamm, 1 großer Stein, 3 Steine, 5 Äste (nach dem Vorschlag von ChatGPT). Neu ist der Gegenstand „großer Stein“: er fällt beim großen Fels immer, beim mittleren manchmal ⚑. Der Regenfänger füllt 4 Wasser je Spieltag, höchstens 6 ⚑. Wetter gibt es noch nicht.
- Trinken: Ein Durst-Bedürfnis gibt es noch nicht. Wasser ist vorerst nur zum Gießen da.

### Hof, Bauer, Ausbau (Nutzer 2026-11-14)
- **Hof** (`building_farmstead`): 4 Baumstämme, 16 Bretter, 8 Steine ⚑. Er stellt Arbeitsplätze für den Beruf **Bauer**. Ohne Hof keine Feldarbeit durch Bewohner.
- Der Bauer arbeitet im Umkreis von 30 m um den Hof ⚑ in dieser Reihenfolge: reife Felder ernten; trockene Felder gießen, leere säen und Felder düngen (alles aus dem Lager); Regenfänger und Komposter leeren; den Komposter mit Verdorbenem füllen.
- **Ausbaustufen für alle Arbeitsgebäude** (`buildables.json`: `workers`, `upgrades`). Arbeitsplätze ⚑:

  | Gebäude | Stufen |
  |---|---|
  | Holzfällerplatz | 2 → 3 → 4 |
  | Steinmetzplatz | 2 → 3 → 4 |
  | Hof | 2 → 4 → 6 |
  | Lagerfeuer (Koch) | 1 → 2 |

  Ausbauen: E mit dem Hammer in der Schnellleiste am fertigen Gebäude, die Kosten der Stufe werden sofort bezahlt ⚑. Beim Abreißen kommt auch das Ausbaumaterial zurück. Sind alle Plätze besetzt, lehnt die Dorfverwaltung den Beruf mit einer Meldung ab.

### Grenzen, ehrlich
- Modelle stammen aus `tools/blender/build_trees.py`. Brombeere, Hundsrose, Walderdbeere und Holunder nutzen den Blattatlas der Himbeere mit; Schlehe und Blaubeere teilen sich einen neuen Atlas mit kleinen Blättern. Das spart Texturspeicher, die Blattformen sind aber nicht artgenau.
- Feldfrüchte und Fallobst sind einfache Formen in Volltonfarben ohne Blatt-Textur. Feld, Komposter, Regenfänger, Hof und die übrigen Stationen haben seit 2026-11-22 einfache Modelle aus `tools/blender/build_stations.py` (Icons über `tools/blender/icon_map.json`). Ein gezogenes Feld legt die 1-m-Kachel `SM_Station_FieldTile` als Raster aus (`AGamerFarmBuilding::SoilTiles`); die gestreckte Kachel bleibt unsichtbar und gibt nur die Kollision.
- Das Feld ist eine flache Platte. Am Hang kann sie einsinken oder an einer Ecke frei schweben, denn sie wird nicht an das Gelände angepasst.
