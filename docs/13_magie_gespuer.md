# Magie-Gespür: Umschauen und Lauschen

Stand 2026-11-15. Gehört zu den Entscheidungen „Umschauen und Lauschen getrennt“ und „Magie-Gespür: Stufen und Boni“ in `docs/01_entscheidungen.md`. Alle Zahlen sind **Startwerte zum Justieren im Spiel**, sie stehen in `game/Content/Data/scan.json` (Umschauen) und `game/Content/Data/listen.json` (Lauschen). Es gibt noch keine echten magischen Objekte; getestet wird mit einem Testziel.

## Zweck

Material und Magie sind zwei getrennte Sinne:

- **Umschauen** (Taste Z) zeigt alltägliches Material. Es hat keine Stufen und keinen Fortschritt.
- **Lauschen** (Taste Q) spürt den Gezeitenhall, also Magie. Man verbessert sich darin, indem man aktiv nach Magie sucht („Karotte vor der Nase“); das wirkt auch im Mehrspieler. Solo-Spiel ist der Maßstab, Koop ist ein Zusatz (Regel 2: derselbe Code, die Gruppen-Boni greifen bei einem Spieler einfach nicht).

## Umschauen (Z)

- Zeigt Nahrung, Holz, Stein, Pflanzen und liegende Gegenstände, auch durch Wände. Keine Magie, keine Stufen.
- 40 m Radius, Ausbreitung 5 s, Funde bleiben 20 s sichtbar (ab dem Moment, in dem die Welle sie erreicht), Abklingzeit 12 s. Keine Ausdauer.
- Die Welle ist eine **Sinuskurve**: ein weicher, transparenter Ring in Treibholz und Nebelgrau läuft als wellige Linie aus. Amplitude (110 cm), Wellenzahl (7) und Tempo des Kräuselns stehen in `scan.json` („wave“).
- Markierungen erscheinen, wenn die Welle sie erreicht, und blenden aus.

## Lauschen (Q)

### Passiv

Ist ein magisches Ziel innerhalb der Reichweite der aktuellen Stufe, schimmert der **Bildschirmrand türkis** (Gezeitenlicht `#A9E9DC`), **je näher, desto stärker und tiefer** (heiß/kalt). Ein weit entferntes Ziel in Reichweite zeigt kaum etwas. Für das **Summen** gibt es nur einen Audio-Haken (`UGamerListenComponent::OnMagicHum`, Intensität 0..1); es gibt noch keine Sounds.

### Aktiv

Eigene umbelegbare Taste (Standard Q, Gamepad linke Schultertaste). Die Figur bleibt stehen und lauscht: ein **langsamer türkiser Sinus-Ring** (offener Ring mit drei Kerben, Stilvorgabe) breitet sich mit konstant 2 m/s aus. **Bewegung bricht das Lauschen ab** (die Welle endet). Danach gilt eine Abklingzeit. Gefundene Dinge erscheinen als **Silhouette** (türkise Kontur mit Glühen, Kontur-Material `M_Outline`, Stencil 2), dazu eine Beschriftung je nach Stufe.

### Stufen

Reichweite, Wellendauer und Sichtdauer wachsen mit der Stufe (die Dauer ergibt sich aus der Reichweite bei 2 m/s; sie steht je Stufe einzeln in den Daten, damit man sie getrennt justieren kann). Die Reichweite gilt auch für das passive Signal.

| Stufe | ab EP | Reichweite | Wellendauer | Funde sichtbar | Abklingzeit | Wirkung |
|---|---:|---:|---:|---:|---:|---|
| 1 Gespür | 0 | 10 m | 5 s | 15 s | 15 s | Flimmern (nur Silhouette, schwach) |
| 2 Ahnung | 10 | 15 m | 7,5 s | 20 s | 15 s | Richtung (Pfeil am Rand) und grobe Art |
| 3 Deutung | 30 | 20 m | 10 s | 25 s | 15 s | Name und Menge |
| 4 Klarblick | 75 | 25 m | 12,5 s | 30 s | 15 s | Versteckte Funde (Tag `GamerMagicHidden`), Markieren möglich |
| 5 Seidr-Blick | 160 | 30 m | 15 s | 40 s | 10 s | Seltene Funde (Tag `GamerMagicRare`), kürzere Abklingzeit |

Die Deckkraft der Silhouette wächst mit der Stufe (0,30 → 1,00). Jeder startet auf Stufe 1; es gibt kein Sofort-Maximum.

### Erfahrung (EP)

Der Zähler liegt pro Spieler auf dem **Server**, wird an den Besitzer **repliziert** und **gespeichert** (Speicherschlüssel `listen`, Feld `ep`). Quellen (nur als Daten, die Objekte gibt es noch nicht; sie rufen `AwardSource` auf):

| Quelle | EP |
|---|---:|
| neuer Fund entdeckt (`new_find`) | +2 |
| Seeglas ernten (`seaglass_harvest`) | +1 |
| Steinkreis einstimmen (`stone_circle`, einmal pro Kreis) | +8 |
| Gezeitenhall-Ereignis (`tide_event`) | +15 |

**Jeder Fund zählt pro Spieler nur einmal.** Der Server merkt sich pro Spieler, welche Funde (ID) schon gezählt sind (`UGamerListenComponent::AwardSource(Quelle, Ort, Fund-ID)`), und speichert die Liste (Feld `counted_targets`). Mitspieler, die einen geteilten Fund mit 50 % bekommen, zählen ihn ebenfalls nur einmal. Ereignisse ohne Fund-ID zählen immer. Einen „+1 je Lauschen“ gibt es nicht: Man soll nicht neben einem einzigen Ziel endlos lauschen können.

Faktor **Gespür-Tempo** (Weltoption): langsam ×0,6, normal ×1, schnell ×1,6.

### Boni (kein Sofort-Maximum, höchstens Stufe 5)

- **Seherin** (`origin:origin_seer`): startet auf Stufe 2 (kleiner Bonus; alle anderen verbessern sich von Grund auf).
- **Seeglas-Amulett** (`item_seaglass_amulet`): +1 Stufe, solange man es trägt. Das Amulett gibt es noch nicht.
- **Dorf-Seherin**: +25 % EP für alle Spieler, solange sie freigeschaltet ist und im Dorf lebt. Sie wird durch eine **Quest** freigeschaltet; Quests gibt es noch nicht. Bedingung vorgemerkt: `unlock:quest_village_seer` und `job:job_seer`; `unlock:` wird nicht geprüft (gilt als nicht erfüllt).
- **D** (Nacht, Nebel, Steinkreis, Winter): +1 Stufe, solange eine Bedingung gilt. Geprüft wird bisher nur `season:winter`; Nacht und Nebel brauchen Tag/Nacht und Wetter, Steinkreis braucht Orte. **D braucht Tag/Nacht und Wetter.**

## Koop

- **Gemeinsamer Fund:** Mitspieler im Umkreis von 30 m bekommen 50 % der EP.
- **Gemeinsames Lauschen:** lauschen zwei oder mehr Spieler innerhalb von 10 s und 15 m, sieht jeder mindestens die Stufe des Besten minus 1, und Reichweite und Dauer steigen um 25 %. Der Server rechnet das (`ServerStartListen`) und schickt jedem das Ergebnis.
- **Markieren ab Stufe 4, für alle sichtbar:** nur vorgemerkt (`CanMark()`, Daten „marks“); es gibt noch keine Funde zum Markieren. Eigene Markierungen sollen auch solo gespeichert bleiben (Speicherung folgt mit den Funden).

## Zieltempo Solo (Annahme, nicht gemessen)

Die Zahlen sind auf Solo ausgelegt. Annahme: **Stufe 3 nach etwa 3–4 Stunden** Spielzeit (30 EP, also grob 8–10 Funde mit Steinkreisen oder einem Ereignis), **Stufe 5 nach etwa 25–30 Stunden** (160 EP). Das hängt davon ab, wie oft es Funde gibt; erst mit den magischen Objekten lässt es sich justieren. Zum Anpassen dienen die Schwellen und Quellen in `listen.json` sowie das Gespür-Tempo der Welt.

## Weltoption „Gespür-Tempo“

`EGamerGespuerTempo` (langsam, normal, schnell) in `FGamerWorldOptions`, URL-Option `?Tempo=slow|normal|fast`. Sie steht als Zeile „Gespür-Tempo“ unter „Welt erstellen“, wird wie die anderen Weltoptionen mit der Welt gespeichert (Teil des Texts `options` im Spielstand) und gilt für alle Spieler der Welt. Die Faktoren stehen in `listen.json` („tempo“).

## Magische Objekte (Konvention für die Magie-Phase)

Ein magisches Ding ist ein Actor mit dem Tag `GamerMagic`. Optional: `GamerMagicHidden` (sichtbar ab Stufe 4), `GamerMagicRare` (ab Stufe 5), `kind:<Art>`, `coarse:<grobe Art>` (Stufe 2), `name:<Name>` (Stufe 3), `amount:<Menge>`. Beim Entdecken, Ernten, Einstimmen ruft das Objekt auf dem Server `UGamerListenComponent::AwardSource(<Quelle>, <Ort>)` auf.

## Technik und Tests

- Dateien: `GamerListenComponent.h/.cpp` (Stufen, EP, Boni, Lauschen, Gruppe, Silhouette), `GamerScanComponent.h/.cpp` (nur noch Umschauen), `GamerHUDListen.cpp` (Randschimmern, Ring, Beschriftung), Daten `scan.json` und `listen.json`, Material `tools/unreal/build_outline_material.py`.
- Testschritte: `scan`, `scanage:<s>`, `listen`, `listenage:<s>`, `listenstate`, `magictarget:<Art>|<Name>[|hidden][|rare]`, `magicevent:<Quelle>[|<Fund-ID>]` (setzt EP direkt, mit Fund-ID einmal pro Fund), `magicorigin:off|on`, `amulet:on|off`, `magiclevel`; Weltoption per Karten-URL `?Tempo=slow`.
- Gebaut und getestet (2026-11-15): Schwellen 0/10/30/75/160, Seherin auf Stufe 2, Amulett und Winter je +1, Tempo langsam (+1,2 statt +2 je Fund), gemeinsamer Fund (Mitspieler 50 %), gemeinsames Lauschen mit Host + 2 Spielern (Reichweite ×1,25), Speichern und Laden von EP und Weltoption, „Umschauen zeigt keine Magie“. Bilder: Sinus-Welle beim Umschauen, Sinus-Ring und Silhouette beim Lauschen, Randschimmern nah und fern, Menüzeile.

## Offen

- Echte magische Objekte, Amulett, Beruf Seherin samt Quest, Seidr-Blick lernen (Bedingung `ability:ability_seidr_sight`; `ability_seidr_vision` in `characters.json` ist die Wettervorhersage und etwas anderes), D mit Nacht/Nebel/Steinkreis.
- Markieren und gespeicherte eigene Markierungen; Summen (Audio) und Flimmern der Dinge in der Welt selbst.
- Die Silhouette nutzt Stencil 2; ein Ziel im Fadenkreuz (Stencil 1) und Magie am selben Ding gleichzeitig ist nicht gelöst.
- Die Rechnung der Gruppe nutzt die Serverwerte der Spieler; eine Abbruchmeldung bei „zu weit auseinander“ gibt es nicht.
