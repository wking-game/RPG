# Taverne, Kochstellen und Gerichte (T1–T6)

Stand 2026-11-15. Wunsch des Nutzers (2026-11-14): „richte als Nächstes eine Taverne oder Ähnliches ein und baue es mit Kochgelegenheit ein und suche dir mit tierstufen passende Gerichte mit dazugehörige puffs“.
Bestätigt (2026-11-15): Gerichteliste übernehmen, Gerste, Hafer und Honig jetzt einbauen, Kochstellen als eigene Bauteile mit Taverne-Stufe.
Alle Zahlen sind Vorschläge von Claude (⚑) und stehen in den Daten (`items.json`, `recipes.json`, `buildables.json`, `jobs.json`, `needs.json`).

## Regeln für Essens-Buffs (Architekturregel 5)

- Essen gibt nur Boni, nie Strafen. Kein Essen = keine Buffs („Satt“-Anzeige leer), keine Sperre.
- Katalog: **Gesundheit** (max. Lebenspunkte), **Regeneration** (LP pro Sekunde), **Traglast** (kg), **Kälteresistenz**, **Giftresistenz** (%), **Schaden** (%). Der Datenprüfer lehnt andere Werte und negative Zahlen ab.
- Höchstens **3 Mahlzeiten gleichzeitig** ⚑ (`UGamerBuffComponent::MaxMeals`). Dasselbe Gericht noch einmal frischt nur die Dauer auf. Ist alles voll, ersetzt das neue Gericht das mit der kürzesten Restzeit.
- Server-autoritativ: Der Client bittet „essen“ (`ServerEat`), der Server prüft das Inventar, zieht 1 ab und setzt den Buff. Die Liste geht nur an den eigenen Spieler (Gerichts-ID + Endzeit in Serverzeit).
- Essen: **Linksklick mit einem Gericht in der Hand** ⚑ (Schnellleiste). Eigene Taste möglich, Absprache mit der Gameplay-Session.

### Stärke und Dauer je Stufe ⚑

| Stufe | Gesundheit | Regeneration | Traglast | Resistenz | Schaden | Dauer (Echtzeit) | Haltbarkeit (getragen) |
|---|---|---|---|---|---|---|---|
| T1 | +10 | +0,5/s | +5 kg | 10 % | +3 % | 10 min | 8 Spieltage |
| T2 | +20 | +1,0/s | +10 kg | 15 % | +5 % | 15 min | 12 |
| T3 | +30 | +1,5/s | +15 kg | 20 % | +8 % | 20 min | 16 |
| T4 | +45 | +2,0/s | +20 kg | 25 % | +10 % | 25 min | 20 |
| T5 | +60 | +2,5/s | +25 kg | 30 % | +12 % | 30 min | 24 |
| T6 | +80 | +3,0/s | +30 kg | 40 % | +15 % | 40 min | 28 |

- Ein Buff: voller Wert. Zwei Buffs: je 70 %. Drei: je 70 %.
- Haltbarkeit über die bestehende Verderb-Formel: `spoil_days` 8 × (1 + 0,5 × (Stufe − 1)), im Lager viermal so lang. Auch Gekochtes verdirbt (bestätigt), nur langsamer.

## Gerichte

**Im Spiel (19):**

| Stufe | Gericht (ID) | Kochstelle | Zutaten | Ergibt | Buffs |
|---|---|---|---|---|---|
| T1 | Einfache Mahlzeit (`item_meal`, bestehend) | Lagerfeuer | 3 Beeren *oder* 4 Holunderbeeren | 1 | Gesundheit +10 |
| T1 | Pilzspieß | Lagerfeuer | 3 Pilze, 1 Ast | 1 | Schaden +3 % |
| T1 | Gebackene Wildäpfel mit Haselnüssen | Lagerfeuer | 2 Wildäpfel, 2 Haselnüsse | 1 | Traglast +5 |
| T1 | Holundersuppe | Lagerfeuer | 4 Holunderbeeren, 1 Wasser | 1 | Giftresistenz 10 % |
| T1 | Hagebuttensud (Getränk) | Lagerfeuer | 3 Hagebutten, 1 Wasser | 1 | Kälteresistenz 10 % |
| T2 | Gemüseeintopf (`item_stew`, bestehend, jetzt T2) | Herdstelle | 1 Zwiebel, 2 Möhren, 2 Kartoffeln | 2 | Gesundheit +14, Regeneration +0,7 |
| T2 | Zwiebel-Pilz-Pfanne | Herdstelle | 2 Zwiebeln, 3 Pilze | 1 | Schaden +5 % |
| T2 | Nuss-Leinsamen-Fladen | Herdstelle | 4 Leinsamen, 3 Haselnüsse, 1 Wasser | 2 | Traglast +10 |
| T2 | Schlehen-Hagebutten-Mus | Herdstelle | 3 Schlehen, 3 Hagebutten | 1 | Kälteresistenz 15 % |
| T2 | Waldbeerenmus | Herdstelle | je 2 Blaubeeren, Walderdbeeren, Brombeeren | 1 | Regeneration +1,0 |
| T3 | Gerstengrütze mit Beeren | Lehmbackofen | 2 Gerste, 2 Beeren, 1 Wasser | 1 | Gesundheit +21, Regeneration +1,05 |
| T3 | Gerstenfladenbrot | Lehmbackofen | 3 Gerste, 1 Wasser | 2 | Traglast +15 |
| T3 | Haferbrei mit Nüssen und Apfel | Lehmbackofen | 2 Hafer, 2 Haselnüsse, 1 Wildapfel | 1 | Regeneration +1,5 |
| T3 | Ofenkartoffeln mit Zwiebeln | Lehmbackofen | 3 Kartoffeln, 1 Zwiebel | 1 | Kälteresistenz 20 % |
| T3 | Ofenbirnen mit Leinsamen | Lehmbackofen | 2 Wildbirnen, 2 Leinsamen | 1 | Gesundheit +30 |
| T5 | Gerstenbier (Getränk) | Braukessel | 2 Gerste, 2 Wasser | 2 | Schaden +8 %, Kälteresistenz 21 % |
| T5 | Met (Getränk) | Braukessel | 2 Honig, 2 Wasser | 2 | Gesundheit +42, Kälteresistenz 21 % |
| T6 | Gewürzter Schlehenmet (Getränk) | Festtafel | 1 Met, 3 Schlehen, 2 Holunderbeeren | 1 | Kälte- und Giftresistenz je 28 % |
| T6 | Honigkuchen mit Nüssen und Birnen | Festtafel | 1 Honig, 2 Gerste, 3 Haselnüsse, 2 Wildbirnen | 2 | Traglast +21, Schaden +10,5 % |

**Noch nicht im Spiel (fehlende Zutaten: Fleisch, Fisch, Milch):** Räucherfisch, Trockenfleisch, Fischsuppe, Wildeintopf (T4, Räucherkammer); Skyr mit Blaubeeren, Braten mit Äpfeln und Zwiebeln (T5); Julbraten mit Met-Soße, Festtags-Eintopf, Fischplatte mit Skyr und Fladen (T6). Werte wie im Vorschlag vom 2026-11-14. Der Datenprüfer erlaubt keine unerreichbaren Gegenstände, deshalb kommen sie erst mit Jagd, Fischerei und Tierhaltung. Die Räucherkammer kommt dann ebenfalls, sie hätte heute keine Rezepte.

## Neue Zutaten

| Zutat | Herkunft | Anmerkung |
|---|---|---|
| Gerste (`item_barley`) | Wildgerste im Startgebiet (2–4), danach Feldfrucht: 2 Spieltage, 4–6 je Einheit | Saatgut = Korn selbst, verdirbt nicht ⚑ |
| Hafer (`item_oats`) | Wildhafer im Startgebiet, Feldfrucht wie Gerste | wie Gerste |
| Honig (`item_honey`) | Wildbienennest (von Hand, 1–2, kommt nach 2 Spieltagen wieder), 2 Stück im Startgebiet | verdirbt nicht (stimmt auch historisch) |

Gerste und Hafer haben vorläufig die Flachs-Modelle im Feld und als Wildpflanze. Das Bienennest ist ein Platzhalter aus Grundformen.

## Taverne (Gebäude)

- `building_tavern` (`AGamerTavern`), Platzhalter-Quader 8 × 5 m. Kosten 6 Baumstämme, 20 Bretter, 10 Steine.
- **Arbeitsplätze** für **Koch** und **Wirt**, je 1 → 2 → 3 ⚑. Die Arbeitsplätze gelten je Beruf, weil der Ausbau-Mechanismus nur eine Zahl je Gebäude kennt. Der Vorschlag „Wirt 1, 1, 2“ ließ sich so nicht abbilden.
- **Ausbau** (E mit Hammer, wie bei den anderen Arbeitsgebäuden): Stufe 2 kostet 16 Bretter und 12 Steine, Stufe 3 kostet 4 Baumstämme, 20 Bretter und 8 Steine.
- **Vorrat** der Taverne: 16 Gerichte und Getränke ⚑. Der Wirt holt sie aus dem Lager, zuerst das, wovon die Taverne am wenigsten hat. Nur Gekochtes mit Buffs, kein Rohes.
- **Bewohner essen in der Taverne**, wenn sie Essen hat, sonst wie bisher am Lager. Dazu trinken sie ein Getränk, wenn eines da ist. **Stimmung:** +0,05 je Stufe des besten Gerichts, +0,1 für ein Getränk ⚑ (`tavern.mood_per_tier`, `mood_drink`). Die Stimmung baut die Gameplay-Session: `OnTavernMeal` ruft `AGamerVillager::AddMood` auf (Bonus bis 0,4, klingt in etwa 4 Spieltagen ab; eingehängt am 2026-11-15, noch nicht gebaut und getestet). Bewohner bekommen keine Kampf-Buffs.
- Das Lagerfeuer hat keine Arbeitsplätze mehr (der Koch arbeitet an der Taverne). Mit der Feldbau-Session abgesprochen.

## Kochstellen

| Kochstelle | Kosten | braucht | Rezepte |
|---|---|---|---|
| Lagerfeuer (bestehend) | wie bisher | – | T1 |
| Herdstelle mit Kessel und Backstein | 8 Steine, 2 Baumstämme, 2 Bretter | Taverne Stufe 1 im Umkreis von 15 m | T2 |
| Lehmbackofen | 12 Steine, 1 großer Stein, 4 Bretter | Taverne Stufe 2 | T3 |
| Braukessel | 6 Steine, 6 Bretter, 2 Baumstämme | Taverne Stufe 3 | T5 (Bier, Met) |
| Festtafel | 2 Baumstämme, 16 Bretter | Taverne Stufe 3 | T6 |

- Der Bauplatz wird schon in der Vorschau geprüft und auf dem Server noch einmal („Braucht eine fertige Taverne (Stufe 2) im Umkreis von 15 m“).
- Der **Spieler** kocht selbst an der Kochstelle: E öffnet das Stationsfenster der Gameplay-Session, gekocht wird über `UGamerWorkComponent::ServerCraft`.
- Der **Koch** (`job_cook`, Station Taverne, Umkreis 15 m) kocht alle Rezepte der Reihe nach, solange davon weniger als 6 im Lager sind ⚑, jeweils an der nächsten passenden Kochstelle bei der Taverne. T1 braucht deshalb ein Lagerfeuer in der Nähe der Taverne.

## Technik

- Daten: `items.json` `food`, `buildables.json` `requires` und `tavern`, `jobs.json` `kind: "serve"` (Format: `docs/06_datenformat.md`).
- Code: `GamerBuffComponent` (an `AGamerCharacter`), `GamerTavern` (Gebäude, Vorrat, Bauplatz-Prüfung), Bewohner in `GamerVillager.cpp`, Testschritte `eat:<item>`, `meals`, `tavern` in `GamerPlayerController.cpp`, Datentest `Gamer.Data.MealsAndTavern`.
- Tests: `-GamerBuffMinuteSeconds=N` verkürzt eine Buff-Minute (nicht in Shipping).
- Schnittstelle für die Anzeige (Gameplay-Session): `GetBuffs()->GetActiveMeals()`, `GetRemainingSeconds(i)`, `GetBonus(stat)`, `OnMealsChanged`.

## Tests (2026-11-15)

- Datentests: 6 von 6 bestanden, darunter der neue Test `Gamer.Data.MealsAndTavern`.
- Host + 2 Clients (`tools/mp_test.ps1`, Testskript) und Einzelspieler (Server + 1 Spieler), jeweils bestanden:
  - **Bauplatz:** Die Herdstelle ohne Taverne wird abgelehnt, der Lehmbackofen bei Taverne Stufe 1 auch. Nach dem Ausbau auf Stufe 3 sind beide gültig.
  - **Kochen und Essen:** Eintopf an der Herdstelle gekocht. Eintopf, Honigkuchen und Met gegessen, die Buffs stimmen und kommen beim Client an. Die vierte Mahlzeit ersetzt die mit der kürzesten Restzeit. „Baumstamm“ wird als Mahlzeit abgelehnt. Nach Ablauf sind keine Buffs mehr aktiv.
  - **Taverne und Bewohner:** Die Arbeitsplätze steigen 1 → 2 → 3. Der Koch kocht am Lagerfeuer bei der Taverne. Der Wirt bringt Mahlzeiten, Eintopf und Bier in die Taverne, der Client sieht den Vorrat. Die Bewohner essen und trinken dort, der Stimmungswert wird geloggt.

## Grenzen, ehrlich

- **Die meisten Buffs wirken noch nicht:** Es gibt noch keine Lebenspunkte, keinen Kampf, keine Traglast-Grenze und keine Kälte. Die Buffs werden gesetzt, übertragen und abgefragt (`GetBonus`); die Systeme müssen sie lesen, sobald es sie gibt.
- **HUD:** Die „Satt“-Anzeige und die Mahlzeiten mit Restzeit (links unten) hat die Gameplay-Session eingebaut, noch nicht gebaut und getestet. Echte Icons kommen mit dem Icon-System.
- **Nicht getestet:**
  - Essen per echtem Linksklick, der Test ruft `ServerEat` direkt auf.
  - Bewohner, die an Herdstelle, Ofen, Braukessel oder Festtafel kochen (getestet ist nur das Lagerfeuer).
  - Verderb im Vorrat der Taverne: Der Vorrat verdirbt dort nicht, das ist eine Lücke.
- **Modelle:** Gebäude, Kochstellen und Bienennest sind Platzhalter, die Gerichte haben noch kein Modell und kein Icon. Später kommen sie im Stil „Gelebter Norden“.
- **Historisch:** Kartoffeln bleiben ein Zugeständnis (offene Frage). Gerste, Hafer, Honig, Met, Bier, Grütze und Fladenbrot sind für die Wikingerzeit belegt.
