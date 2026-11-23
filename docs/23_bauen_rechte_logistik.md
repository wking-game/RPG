# Bauen in Stufen, Koop-Rechte, Bewohner-Logistik

Stand 2026-11-22. Grundlage ist Geminis „Game Design & System-Architektur (GDD)“ (Kopie: `design/referenz/gemini_gdd_system_architektur.md`; erweiterte Fassung 2 mit Wetter, Charakter und Höhlen: `design/referenz/gemini_gdd_system_architektur_v2.md`), durchgesehen mit dem Nutzer. Dieses Dokument hält fest, **was übernommen wird, was nicht, und wie es zu eurem Stand passt** (Bauplan + Hammer, JSON-Daten, Server-Autorität, Bewohner mit Prioritäten). Nichts davon ist gebaut.

Kennzeichen: **bestätigt** = vom Nutzer entschieden · **Vorschlag** = von Claude.

## 1. Entscheidungen (2026-11-22, alle bestätigt)

| Thema | Entscheidung |
|---|---|
| Bau-Stufen | 3 Stufen mit eigenen Kosten, aber **ein Modell** je Bauteil. Die Stufen zeigen sich über ein Fortschritts-Material (Bauplan durchsichtig → Rohbau roh → fertig). Nur große Bauten (Haus, Taverne) bekommen zusätzlich ein Gerüst-Modell |
| Koop-Rechte | Weltoption „Standard-Freigabe“ für neue Gebäude, dazu pro Gebäude vom Besitzer änderbar. Ausnahmen für einzelne Mitspieler später |
| Terraforming | Später (nach dem Kampf): nur ein Werkzeug, das eine Fläche zum Bauen einebnet. Kein freies Formen, kein Aufschütten, keine Gräben. Die Vision („kein Voxel-Terraforming“) bleibt im Kern |
| Bewohner-Logistik | Bewohner tragen Material aus dem Lager zur Baustelle, mit Reservierungen. Kommt mit dem Dorf-Ausbau, nicht in Phase 3 |

## 2. Was aus dem GDD übernommen wird

- Drei Stufen mit **nur den Kosten dieser Stufe** (keine Verrechnung).
- **Physische Anlieferung** an die Baustelle, durch Spieler oder Bewohner.
- **Reservierungen**: am Lager `frei = gelagert − reserviert`, an der Baustelle `fehlt = Bedarf − geliefert − unterwegs`. Zustände `PendingPickup`, `CarryingToSite`, `Completed`, `Cancelled`, dazu Timeout.
- **`TargetStageSnapshot`**: Ein Auftrag merkt sich die Stufe, für die er erstellt wurde; ändert sich die Stufe, wird er verworfen.
- **Besitzer je Objekt** und drei Freigabe-Stufen: nur Besitzer · Mitspieler dürfen liefern und fertigbauen · Mitspieler dürfen auch abreißen.
- **Eigene Blaupausen** (Gruppen von Bauteilen mit relativer Lage), aber später (Abschnitt 6).

## 3. Korrekturen gegenüber dem GDD

1. **Kein `UStaticMesh*` in Definitionen.** Bauteile stehen in `buildables.json` mit Modellpfad als Text (Regel 3, `docs/06_datenformat.md`). Die Stufen kommen als Feld `stages` dazu: eine Liste mit `cost` (Gegenstand und Menge) und `work` (Arbeitsaufwand) je Stufe, plus Gerüst-Modell nur wo nötig.
2. **Stufenbuch statt Löschen.** Das GDD leert `DeliveredInCurrentStage` beim Stufenwechsel. Damit weiß man beim Abreißen nicht mehr, was in früheren Stufen geliefert wurde. Besser: Je Gebäude wird je Stufe mitgeschrieben, was geliefert wurde (`stage_paid`). So gibt die Weltoption „Abreißen“ (voll / halb / aus) das Material korrekt zurück, auch aus bereits abgeschlossenen Stufen.
3. **Eigene Wörter.** „Stufe“ meint schon die T-Stufe (T1–T6) und die Ausbaustufe der Arbeitsgebäude (`upgrades`). Die neue heißt **Baustufe** (Daten: `stages`, Anzeige „Bauabschnitt 1 von 3“).
4. **Verhältnis zur Weltoption „Bauablauf“** (`blueprint` / `instant` / `material`, bestätigt 2026-11-13): `instant` bleibt (alles sofort fertig, keine Stufen). `blueprint` und `material` werden zu den drei Baustufen; „Material beim Bauen“ heißt dann: Material wird beim Anliefern verbraucht, nicht beim Platzieren. Genaue Zuordnung folgt mit der Umsetzung, ich frage vorher.
5. **Server-Autorität (Regel 1).** Das GDD sagt das nicht. Für Bauen, Liefern und Rechte gilt: Clients senden Anfragen, der Server entscheidet, prüft Reichweite und Rechte, und repliziert den Zustand.
6. **Spieler-ID: schon vorhanden, nur noch nutzen.** `UGamerPlayerIdentity::GetLocalPlayerId` legt beim ersten Start eine GUID an und speichert sie in der Config (pro PC stabil); der Startparameter `-GamerPlayerId=<name>` überschreibt sie (für Tests). Der Server kennt sie pro Spieler (`GetPlayerId(Controller)`), das Grab speichert sie als `OwnerKey`. Für Gebäude-Rechte reicht diese ID, sie muss nur zusätzlich am Gebäude gespeichert werden (Feld `owner`). **Einschränkungen:** Die ID hängt am PC, nicht am Konto (neuer PC = neue ID, bis es die Steam-ID gibt), und die Client-Anzeige „darf das Grab öffnen“ erkennt den Besitzer in gespeicherten Welten noch am Figurnamen (`docs/02_phasenplan.md`). Das ist behebbar, aber kein Vorab-Schritt für die Rechte. (Korrigiert 2026-11-22: Ich hatte die ID zuerst als zu schwach beschrieben.)
7. **Reservierungen sind Laufzeit-Daten.** Sie leben nur auf dem Server und werden **nicht gespeichert**; nach dem Laden berechnet der Server sie aus dem Zustand neu. Keine `AActor*` in gespeicherten Daten, nur IDs. Zeitstempel in Spielzeit, nicht in `float`-Sekunden der Echtzeit.
8. **Einhängen statt ersetzen.** Die Bewohner-KI hat Prioritäten (Hunger, Schlaf, Tragen, Beruf, warten). Die Logistik-Zustände (`FIND_RESOURCE_SRC` bis `DELIVER_RESOURCE`) werden als Beruf-Aufgabe „Baumeister: liefern“ eingehängt. Hunger und Schlaf gehen weiter vor.
9. **Sperre gegen Massen-Baupläne.** Ein Spieler darf nur eine begrenzte Zahl unfertiger Baupläne gleichzeitig setzen (Vorschlag: 200), sonst entstehen riesige Lieferaufträge. Die Grenze steht in den Daten.
10. **Statik** (Gemini-Punkt „strukturelle Stabilität“): bleibt offen. Bisher bleibt eine Wand stehen, wenn das Fundament fehlt. Entscheidung später.

## 4. Datenformat (Vorschlag)

`buildables.json`, je Bauteil zusätzlich:

```json
{ "id": "building_wall_wood",
  "stages": [
    { "cost": [ { "item": "item_plank", "count": 2 } ], "work": 4.0 },
    { "cost": [ { "item": "item_plank", "count": 3 } ], "work": 6.0 },
    { "cost": [ { "item": "item_fiber", "count": 2 } ], "work": 3.0 }
  ],
  "scaffold_mesh": "" }
```

Zustand je Gebäude im Spielstand (Erweiterung des bestehenden Gebäude-Eintrags):

| Feld | Bedeutung |
|---|---|
| `stage` | aktuelle Baustufe 1–3, 4 = fertig |
| `stage_paid` | je Baustufe: was geliefert wurde (für Rückgabe beim Abreißen) |
| `work` | Arbeitsfortschritt in der aktuellen Stufe |
| `owner` | stabile Spieler-ID |
| `access` | `owner_only` / `allow_finish` / `allow_finish_break`; optional `overrides` je Spieler (später) |

Weltoption `build_access` (Standard: `allow_finish`, „Mitspieler dürfen bauen, nur der Besitzer darf abreißen“).

## 5. Terraforming (später)

Nur „Bauplatz ebnen“: Werkzeug (Spitzhacke oder Schaufel), Fläche bis höchstens 100 cm Höhenunterschied, Kosten in Zeit und Material. Voraussetzungen, die mitziehen müssen: Wegfindungs-Netz der Bewohner, Pflanzen und Felsen auf der Fläche, Speichern, neu beitretende Spieler. Aufwand vorher einschätzen, bevor es in einen Phasenplan kommt.

## 6. Was bewusst nicht übernommen oder verschoben ist

| Gemini | Stand |
|---|---|
| Eigene Blaupausen (Gruppen speichern und neu setzen) | später; braucht Server-Prüfung jedes Teils und Platzierung auf dem Raster; Daten aus eurem Spielstand sind eine gute Grundlage |
| Höhlensysteme | nicht als eigenes System. Höhlen sind handgebaute Orte der Story (Eishöhle im Basaltfjell, gefrorenes Wrack), keine prozedurale Unterwelt |
| Nebel des Krieges, Kartenmarkierungen, Pings | später (UI) |
| Wettersound, Unterkühlung, Tod/Respawn | größtenteils entschieden (Wetter, Kälte, Durst, Tod mit Grab); Wetter folgt mit Tag/Nacht |
| Rekrutierung (Befreien, Anwerben, Siedlungsruf) | offen; Bewohner ziehen bisher in freie Betten ein |
| Zufriedenheit und Lohn | offen; bisher drei Bedürfnisse (Hunger, Schlaf, Unterkunft) |
| Terraforming frei | nicht übernommen (Abschnitt 5) |

## 7. Reihenfolge (Vorschlag)

1. Spieler-ID am Gebäude speichern und die Grab-Anzeige auf die ID umstellen (klein).
2. Baustufen mit Stufenbuch, Fortschritts-Material und Anzeige (mit den neuen Bau-Optionen).
3. Besitzer und Freigabe-Weltoption.
4. Bewohner-Logistik mit Reservierungen (mit dem Dorf-Ausbau).
5. Später: Bauplatz ebnen, eigene Blaupausen.

Punkte 1–3 gehören eher zu Phase 1/Dorf, nicht zum Kampf (Phase 3). Ein Zeitpunkt ist nicht festgelegt.

## 8. Ergänzungen aus der erweiterten Fassung (2026-11-22)

Die zweite Fassung des GDD bringt Wetter, Unterschlupf, Bewohner-Bedürfnisse, Charakter und Höhlen. Entscheidungen (alle bestätigt, Einträge im Entscheidungslog): 

| Thema | Ergebnis |
|---|---|
| Wetter | Optik und Ton immer; Gameplay-Wirkungen nur mit der Weltoption „Survivor“ oder einer eigenen Wetter-Option (kein Zeitdruck, Regel 6). Kommt mit Tag/Nacht und Wetter. |
| Unterschlupf | Drei Stufen: ungeschützt, überdacht (fertiges Dach, `provides_shelter` in der Baustufe oder im Bauteil), geschlossen/Höhle. **Der Server** prüft ihn in Zeitabständen mit fünf Strahlen nach oben (mindestens 3 Treffer = überdacht) für Nässe, Feuer, Bewohner. Der Client prüft nur für Anzeige und Ton. Ein unfertiges Gerüst schützt nicht. |
| Feuerstellen | Brennen unter einem fertigen Dach weiter; ohne Dach erlöschen sie nur mit der Wetter-Wirkung (Option). |
| Nahrungs-Reservierung | Reservierungen aus Abschnitt 2 gelten auch für Mahlzeiten: Zwei Bewohner greifen nicht nach derselben letzten Ration. Gleiche Server-Regeln: nicht gespeichert, nach dem Laden neu berechnet, Timeout. |
| Warnsymbole | Schwebende Symbole über Bewohnern nur bei Problemen (Hunger, kein Bett, Nässe). Passt in die Vogelperspektive. |
| Bewohner-System | Sieben Berufe und drei Bedürfnisse bleiben. Geminis vier Rollen, Moral und Wetter-Belastung nicht jetzt (Regel 7); Wetter-Belastung kommt mit dem Wetter; Anwerbung über Siedlungs-Attraktivität später. |
| Charakter | Eure Werte und Herkünfte bleiben. Werte steigen durch Tun (`docs/11_charaktere.md`, Ergänzung vom 2026-11-22). |
| Höhlen | Handgebaute Orte mit **Zonen-Volumen** (Gefahrenstufe 1–3, Hall, Licht, Wetterschutz, Kreaturen, Erzadern). Kein prozedurales System. |
| Kreaturen und Wetter | Verhalten kann von Tageszeit und Wetter abhängen (Gemini: aktiv bei Tag oder Nacht, sucht Unterschlupf im Sturm, stärker bei Nebel). Passt zum Gezeitenhall; kommt mit Tag/Nacht und Wetter. Gemini-Felder als Vorbild für `creatures.json`: aktiv Tag/Nacht, Wahrnehmungsradius, Temperament (passiv, verteidigend, territorial, feindlich, lauernd). |

**Nicht übernommen:** Geminis Werte (Vitality …), Herkünfte (Zimmermann …), sechs Fertigkeiten und das Terraforming in der Fertigkeit „MiningTerraform“.

**Weitere technische Hinweise:** Keine Zeiger auf Akteure in gespeicherten Strukturen (IDs), `PrimaryShelterActor` nur zur Laufzeit; die Strahlen pro Bewohner auf einen Zeitgeber legen (z. B. alle 2 s), nicht jeden Tick.
