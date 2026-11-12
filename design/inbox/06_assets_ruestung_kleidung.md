# 06 – Assets: Rüstung und Kleidung (Planung für Phase 3)

**Status:** Vorplanung, keine Asset-Produktion. Meshes, Rigs, Animationen, Icons und Code werden erst erstellt, wenn Phase 3 ansteht.  
**Stil:** Klarform-Saga im mittelalterlich-nordischen Wikingerspiel.  
**Daten:** Keine Schadens-, Gewichts-, Tempo- oder Haltbarkeitswerte; solche Werte gehören später in `data/`.

## Entwurfsprinzip

Die vorhandenen Phase-1-Figuren (`char_player_base`, `char_villager_base`) und ihr gemeinsames Unreal-Mannequin-Skelett bleiben die Grundlage. Rüstung wird modular über passende Körperteile gelegt. Unterliegende Körperbereiche werden ausgeblendet, damit Figuren nicht doppelte Geometrie rendern.

Die Stufenleiter folgt dem Waffenplan: T1 Rohstufe, T2 Raseneisen, T3 Schmiedeeisen, T4 Stahl, T5 Damast, T6 Relikt. Die Stufe wird zuerst über Material und kleine, klar erkennbare Zusatzteile lesbar. Neue Silhouetten gibt es nur, wenn sie wirklich einen Stufensprung zeigen. Keine separate Rüstung pro Materialfarbe.

## Vorgeschlagene Ausrüstungs-Slots

| Slot-ID | Sichtbarer Inhalt | MVP-Regel |
|---|---|---|
| `slot_head` | Stoffkappe/Haube oder Helm | Pro Charakter ein Kopfteil; keine gleichzeitige Kappe und Helm-Kombination im MVP |
| `slot_torso` | Tunika, wattierte Schutzlage oder Kettenhemd | Ein modulares Oberkörperteil; Unterkleidung bleibt Teil des Figuren-Basissets |
| `slot_hands` | Stoffwickel, Lederhandschuhe oder Armschienen | Ein gemeinsames Paar-Mesh statt getrennter Einzelhandslots |
| `slot_legs` | Hose, Beinwickel oder zusätzliche Schutzlage | Basishose bleibt im Figuren-Set; Schutzteil wird darüber gelegt |
| `slot_feet` | Schuhe oder Stiefel | Ein Paar-Mesh, kompatibel mit beiden Körperformen |
| `slot_cloak` | Wollumhang | Optionaler kosmetischer Slot ohne Schmuckwert; wenn Umfang zu groß wird, bleibt der Umhang Teil eines Outfit-Sets |

Es wird **kein Schmuck-Slot** angelegt. Eine Spange oder ein Verschluss darf als integriertes Detail am Kleidungsstück erscheinen, ist aber kein separates Inventar-Asset.

## Asset-Liste

| ID | Unreal-Name / Mesh-Familie | Stufen | Zweck | Zielbudget |
|---|---|---|---|---:|
| `armor_headgear` | `SK_Armor_Headgear_Cap_T1`, `SK_Armor_Headgear_Helmet_T2` | T1–T5 Material-/Zusatzteilvarianten; T6 später eigen | Stoffkappe/Haube und ein einfacher Helm-Grundtyp | Ziel 1.000 Dreiecke; max. 2.000 |
| `armor_torso` | `SK_Armor_Torso_Padded_T1`, `SK_Armor_Torso_Mail_T3` | T1–T5; T6 später eigen | Wattierte Schutzlage, Lederverstärkung, Kettenhemd-Overlay | Ziel 4.000; max. 6.000 |
| `armor_hands` | `SK_Armor_Hands_Wraps_T1`, `SK_Armor_Hands_Bracers_T2` | T1–T5 überwiegend Materialinstanzen; T6 später eigen | Hand-/Unterarm-Schutz | Ziel 2.000; max. 3.000 |
| `armor_legs` | `SK_Armor_Legs_Wraps_T1`, `SK_Armor_Legs_Overlayer_T2` | T1–T5 Material-/Zusatzteilvarianten; T6 später eigen | Beinwickel und zusätzliche Schutzlage über der Hose | Ziel 2.000; max. 3.000 |
| `armor_feet` | `SK_Armor_Feet_Shoes_T1` | T1–T5 Materialvarianten; T6 später eigen | Schuhe/Stiefel als gemeinsames Paar-Mesh | Ziel 1.000; max. 1.500 |
| `clothing_cloak` | `SK_Clothing_Cloak_Wool` | T1–T5 Farb-/Materialvarianten; besondere T6-Form später | Kosmetischer Wollumhang, wenn der Slot im MVP bleibt | Ziel 1.500; max. 2.500 |
| `ui_icons_armor` | `T_Icon_Armor_Head`, `T_Icon_Armor_Torso` usw. | Ein Icon je Slottyp; Tier-Rahmen wiederverwendet | Inventar- und Ausrüstungsübersicht | Entwurf 512²; Spiel 256² |

### `armor_headgear` – Kopfteil

- **T1:** Stoffkappe oder eng sitzende Haube aus Wolle/Leinen; keine Metallteile.
- **T2:** einfacher, dunkler Eisenhelm mit wenigen sichtbaren, groben Verbindungen.
- **T3:** sauberer Helm mit schlichtem Nasenschutz; dieselbe Grundsilhouette kann mit unterschiedlichen Metallinstanzen laufen.
- **T4:** hellere, gut erkennbare Stahlkante oder Stirnverstärkung.
- **T5:** kleines Damastmuster auf einer begrenzten Metallfläche und dezente Messingpunkte.
- **T6:** spätere, eigenständige Boss-Reliktform; nicht Teil des MVP.
- **Stilregel:** keine Hörner am Kampfhelm. Die Forschungslage trägt die ikonische Hornhelm-Optik nicht als verbreitete Kampfausrüstung.

### `armor_torso` – Oberkörper

- **T1:** wattierte Tunika aus grobem Woll-/Leinenstoff als einfache Schutzsilhouette.
- **T2:** gleiche Grundform mit Lederverstärkung und wenigen dunklen Raseneisen-Beschlägen.
- **T3:** Kettenhemd über der Textilunterlage als seltenes, deutlich höherwertiges Set.
- **T4:** verfeinerte Metallteile und helle Stahlkanten; ein neues Vollmesh ist nicht nötig.
- **T5:** Damastmuster nur auf wenigen Metallsegmenten, mit kleinen Ocker-/Messingakzenten.
- **T6:** spätere Bossform mit eigener Silhouette und Materialfamilie.
- **MVP / Ausbau:** T1 wattierte Schutzlage, T2 einfache Leder-/Eisenverstärkung und ein T3-Kettenhemd als seltener Fortschritt / später zusätzliche Silhouetten und Tier-6-Relikt.

### `armor_hands` – Hand- und Unterarmschutz

- **T1:** Stoffwickel und einfache Lederhandschuhe.
- **T2:** breite Lederarmschienen mit einzelnen dunklen Metallnieten.
- **T3:** dieselbe Grundform mit sauberem Eisenbeschlag.
- **T4:** hellere Stahlteile; keine kleinteilige Plattenrüstung an jedem Finger.
- **T5:** Damastmuster nur auf einer kleinen Armschienenfläche.
- **T6:** späteres Boss-Design.
- **MVP / Ausbau:** ein gemeinsames Paar-Mesh pro Grundform / später 1–2 zusätzliche Schutzformen, falls Lesbarkeit oder Animation es verlangt.

### `armor_legs` – Beinschutz

- **T1:** Hose des Figuren-Basissets plus breite Stoffwickel um die Unterschenkel.
- **T2:** zusätzliche Lederstücke an Knie/Schienbein; gemeinsame Meshform für beide Körperformen.
- **T3:** wenige Eisenbeschläge oder ein kurzes, modulares Kettenhemd-Überteil, nur wenn es mit Bewegung und Reiten/Beinanimationen sauber bleibt.
- **T4:** verfeinerte Stahlverstärkung mit klarer Kante.
- **T5:** Damastmuster nur als kleiner Akzent.
- **T6:** späteres Boss-Design.
- **MVP / Ausbau:** Stoffwickel und ein Leder-Overlay / später eine zusätzliche Metall-Schutzform.

### `armor_feet` – Schuhe und Stiefel

- **T1:** einfache, weiche Lederschuhe.
- **T2–T3:** höhere, geschnürte Lederstiefel mit wechselnder Materialinstanz.
- **T4–T5:** wenige Metall-/Damastdetails, ohne die Silhouette unnötig zu vergrößern.
- **T6:** spätere Sonderform.
- **MVP / Ausbau:** eine gemeinsame Paarform / später Materialstufen und optional eine zweite Stiefelsilhouette.

### `clothing_cloak` – Wollumhang

- **Konzept:** Breiter, kurzer bis mittellanger Wollumhang, der die Bewegung der Waffe nicht verdeckt. Eine einfache Spange ist in das Mesh integriert. Farbwahl erfolgt über den Charaktereditor; keine automatische Tönung durch Koop-Spielerfarben.
- **MVP / Ausbau:** optionales Kosmetikteil, ohne separate Schmuckmechanik / später regionale Schnitte, Farbkombinationen und dezente Webmuster.
- **Hinweis:** Cloth-Simulation ist kein MVP-Erfordernis. Eine einfache, rig-kompatible Form reicht für den ersten Kampf-Slice.

## Tier- und Meshstrategie

| Tier | Material-/Look-Vorgabe | Geometrie-Regel |
|---|---|---|
| `tier_1` Rohstufe | Wolle, Leinen, Leder, Knochen-/Holzdetails; helle Naturfarben | Basis-Outfit und Stoffkappe; keine Metallpanzerung |
| `tier_2` Raseneisen | Dunkles, raues Eisen mit rostbraunen Tönen | Leder-/Textilgrundform behalten; Beschläge ergänzen |
| `tier_3` Schmiedeeisen | Mittelgraues, geglättetes Eisen | Ein Kettenhemd-Overlay und ein schlichter Helm als sichtbarer Fortschritt |
| `tier_4` Stahl | Hellere, saubere Schneiden-/Kantenbänder | Materialinstanz und wenige Verstärkungsteile statt Vollmesh-Neubau |
| `tier_5` Damast | Kleines Wellenmuster auf Masken, sparsame Messingakzente | Eine geteilte 512²-Damastmaske; keine eigene Textur je Rüstungsteil |
| `tier_6` Relikt | Bossmaterial und eigene Akzentfarbe | Spätere Erweiterung; neue Silhouette nur für ausgewählte Relikte |

## Budgets und Wiederverwendung

- **Gesamte Spielerfigur inklusive Kleidung/Rüstung:** Ziel 32.000–36.000 Dreiecke, harte Obergrenze 40.000 bei LOD0. Die Slotbudgets in der Assetliste sind darin enthalten, nicht zusätzlich zu rechnen.
- **Texturen:** gemeinsame Kleidungstextur-Familie `T_Clothing_Atlas_BC`, `T_Clothing_Atlas_N`, `T_Clothing_Atlas_ORM`, je 2048²; Metallset aus der Waffenliste 1024²; Damastmaske 512².
- **Materialinstanzen:** Tierfarben und Abnutzung über Materialparameter. Keine individuelle Textur pro Tier oder Slot; keine 4K-Texturen.
- **LOD:** drei LODs je Meshfamilie; dieselbe Spieler-/Dorfbewohner-Skelettbasis wie in Phase 1.
- **Varianten:** Körperform- und Kopfvarianten aus der Phase-1-Figurenliste bleiben kompatibel. Stofffarbe kann im Charaktereditor gewählt werden; Schildbemalung bleibt ebenfalls individuell im Charakter-/Lagereditor und wird nicht automatisch der Koop-Spielerfarbe zugeordnet.
- **Gewaltoptionen:** Blutdarstellung und Abtrennung von Körperteilen bleiben einstellbar. Diese Liste plant keine Gore-Meshes; die Darstellung wird in einem separaten Effekt-/Technikplan festgelegt. Modulare Rüstungsteile erleichtern die spätere technische Prüfung.

## Historischer Stilhinweis

Die Nationalmuseen Dänemarks beschreiben gewöhnliche Kleidung aus Wolle und Flachs, unter anderem Tuniken, Hosen und Umhänge. Helme und vollständige Kettenhemden sind archäologisch selten belegt; deshalb erscheinen hochwertige Metallrüstung und Kettenhemd im Spiel als seltene Fortschritts- oder Statusausrüstung, nicht als Standardkleidung aller Bewohner. Hörner werden nicht als Kampfhelm-Merkmal verwendet.

- [National Museum of Denmark – Clothes and jewellery](https://en.natmus.dk/historical-knowledge/denmark/prehistoric-period-until-1050-ad/the-viking-age/the-people/clothes-and-jewellery/)
- [National Museum of Denmark – Viking helmets](https://en.natmus.dk/historical-knowledge/denmark/prehistoric-period-until-1050-ad/the-viking-age/weapons/helmets/)

## Entscheidungen, die der Nutzer noch treffen muss

1. Soll der Wollumhang ein eigener kosmetischer Slot sein, oder gehört er als Teil des Outfits fest zur Kleidung?
2. Soll Rüstung eigene Haltbarkeit bekommen? Bisher bestätigt ist einfache Haltbarkeit für Waffen und Werkzeuge.

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `06_assets_ruestung_kleidung.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext; historische Hinweise mit Quellen im Dokument; keine externen Assets verwendet |
