# Schritt 2b: Nahkampf gegen Platzhalter-Gegner (Plan, Entwurf 2026-11-22)

Baut auf `docs/24` (Daten, Würfel), `docs/25` (Ausrüstung, Level) und `docs/21` (Regeln) auf. **Noch nichts umgesetzt; der Plan wartet auf Klios Antwort.**

## 1. Ziel und Grenzen

**Ziel:** Die erste spielbare Kampfschleife, mit Kapseln statt Modellen: Ein Gegner aus `creatures.json`/`animals.json` erscheint, bewegt sich, wird mit der Waffe in der Hand getroffen, verliert LP und Haltung, greift zurück, stirbt, lässt Beute fallen und gibt Erfahrung. Alles server-autoritativ (Regel 1), solo = Koop mit einem Spieler (Regel 2).

**Zweck:** Die Zahlen aus `docs/21` (Kampfdauer, Streuung, Erfahrungskurve, `creature_hp_scale`) lassen sich erst jetzt wirklich fühlen. `docs/21` Abschnitt 14 hat sie ausdrücklich bis nach dem ersten Kampftest offen gelassen.

**Gehört nicht dazu:** Modelle und Animationen der Kreaturen (Platzhalter = farbige Kapsel mit Namensschild), Bosse und ihre Phasen, Fernkampf-Zielen mit dem Bogen, Blocken und Parieren mit dem Schild (kommt als 2c), Menschen-Gegner mit Ausrüstung, Rudel-Taktik, Höhlen, Umweltgefahren, Fähigkeiten (Schildstoß, Kriegsschrei).

## 2. Was es schon gibt

- Daten und Würfel: `RollSpawn` (Rang, Streuung, Anführer), `RollLoot`, `FGamerPoiseMeter`, `EnemyPoiseDamageTaken`, `PlayerPoiseDamageTaken`, `KillXp`, `UnderLevelFactor`.
- Spieler: Ausrüstung mit Zonenrüstung (`UGamerHealthComponent::TakeDamage` mit Zone), Level (`UGamerProgressComponent::AwardXp`), Waffen als Gegenstände mit `damage`.
- Schlag am Spieler: `AGamerCharacter::PerformStrike` (Werkzeuge, Bauen, Sammeln). Ein Waffenschlag fehlt noch.
- Bodenbeute: `AGamerGroundItem`. Dorfbewohner (`AGamerVillager`) als Vorlage für Server-KI und Bewegung.

**Lücken:** kein Gegner-Actor, kein Waffenschlag mit Trefferprüfung, keine Beute-Auslösung, kein Spawn-System in der Welt, keine Biom-Karte.

## 3. Arbeitsschritte

| Nr | Inhalt | Dateien | Tests | Größe |
|---|---|---|---|---|
| 2b.1 | **`AGamerCreature`:** Actor (Kapsel + Namensschild + LP-Balken über dem Kopf), liest Werte aus den Daten (Rang, Wurf, Anführer-Stufe), `IGamerDamageable`, repliziert LP und Zustand, Tod nach 3 s weg | neu `GamerCreature.h/.cpp` | `Gamer.Creature.*` (Werte aus Wurf) | L |
| 2b.2 | **Waffenschlag:** Linksklick mit Waffe in der Hand schlägt nach vorn (Kegel/Kapsel-Trace auf dem Server), Schaden = Waffenwert × Angriffsart (leicht/schwer aus `combat.json`) × Zone (Kopf/Körper/Gliedmaßen nach Trefferhöhe), Haltungsschaden, Abklingzeit | `GamerCharacter`, neu `GamerMeleeComponent` | Rechentest (Schaden/Haltung je Zone) | L |
| 2b.3 | **Haltung und Taumeln:** `FGamerPoiseMeter` am Gegner und am Spieler; Taumeln (2,5 s Gegner, 0,8 s Spieler) mit Schadensbonus ×1,5 | `GamerCreature`, `GamerHealthComponent` | `Gamer.Combat.*` erweitern | M |
| 2b.4 | **Gegner-KI (klein):** vier Temperamente aus den Daten: passiv (flieht), scheu, neutral (wehrt sich), aggressiv (jagt); Angriff mit Windup (0,6 s), Reichweite, Pause; Zone beim Spieler zufällig mit Gewichten. Keine Pfadsuche über Hindernisse hinaus, nur NavMesh wie bei Dorfbewohnern | `GamerCreature`, `GamerCreatureBrain` (reine Auswahllogik) | Test der Entscheidungslogik | L |
| 2b.5 | **Tod, Beute, Erfahrung:** `RollLoot` mit Spieler-Level-Zone, Gegenstände auf den Boden, `KillXp` mit Unterlevel-Bremse und Koop-Anteil; Spieler mit dem meisten Schaden bekommt die Beute-Zuweisung zuerst (Koop-Regel offen) | `GamerCreature`, `GamerProgressComponent` | `Gamer.Loot.*` Ablauf | M |
| 2b.6 | **Spawn:** `AGamerCreatureSpawner` (Server): hält je Spieler-Umgebung höchstens N Gegner (Weltoption „Gegnerdichte“: aus/niedrig/normal), wählt Biom per Spieler-Position (vorerst alle Fjordwiesen, Biom-Karte kommt mit der Weltgestaltung), würfelt mit `PickSpawnRule`/`RollSpawn`, nur außerhalb von Siedlungen, nie in Sichtweite | neu `GamerCreatureSpawner.*`, `GamerGameState` | `Gamer.Spawn.*` Platzierungsregeln | L |
| 2b.7 | **HUD:** Namensschild mit Rang/Stufe-Farbe, LP-Balken, Haltungsbalken bei Taumeln, Trefferzahl-Anzeige (kleine Zahlen), Fadenkreuz-Hinweis auf Gegner | `GamerHUDCombat.cpp` | Screenshot | M |
| 2b.8 | **Speichern:** Gegner werden nicht gespeichert (verschwinden beim Laden, Spawner würfelt neu); Spielerwerte wie gehabt. Test, dass Laden mit lebenden Gegnern nichts bricht | Save-Test | `Gamer.Save.*` | K |
| 2b.9 | **Testbefehle und Messung:** `spawn:<id>|<rang>`, `killall`, Protokoll je Kampf (Dauer, Schaden, Treffer) als Zeilen im Log, damit die Kampfdauer-Prüfrechnung aus `docs/21` 5.4 gegen echte Werte läuft | `GamerPlayerController` | Mehrspieler-Lauf | M |
| 2b.10 | **Doku und Zahlen:** `docs/21` Abschnitt 14 mit den ersten Messwerten, Plan-Stand, Entscheidungslog (nur ergänzen) | docs | – | K |

Reihenfolge: 2b.1 → 2b.2 → 2b.3 → 2b.5 → 2b.4 → 2b.6 → 2b.7 → 2b.8 → 2b.9 → 2b.10. Erster Halt nach 2b.5 (Schlagen, Sterben, Beute, ohne KI), zweiter nach 2b.6 (Gegner erscheinen von allein).

## 4. Entwürfe, die du bestätigen sollst

1. **Waffenschlag:** Linksklick mit Waffe in der Hand (Schnellleiste). Gedrückt halten lädt **kein** schweren Schlag; leicht = Klick, schwer = Umschalt + Klick (Sprint-Taste hat Vorrang vor dem Klick nur außerhalb des Kampfs). Ohne Waffe: Fäuste mit sehr kleinem Schaden. Das ist die kleinste Bedienung, die sich gegen KCD-artige Tiefe später erweitern lässt.
2. **Trefferzone:** nach der Höhe des Treffers am Gegner (oberes Drittel Kopf, mittleres Körper, unteres Gliedmaßen), keine Ziel-Taste. Einfach und ohne Modelle prüfbar.
3. **Gegner-Zonenrüstung:** Kreaturen haben eine Rüstung je Zone nur dort, wo `docs/21` sie nennt; sonst 0.
4. **Gegner-Dichte:** Weltoption „Gegnerdichte“ (aus / niedrig / normal), Standard niedrig, damit der Siedlungsspieler (Regel 6: kein Zeitdruck) in Ruhe bauen kann. Gegner greifen Gebäude und Dorfbewohner in 2b nicht an.
5. **Dorfbewohner im Kampf:** Gegner ignorieren sie in 2b. Verteidigung der Siedlung ist ein eigener Schritt.
6. **Platzhalter-Optik:** Kapsel in Farbe nach Temperament (Grün passiv, Gelb scheu/neutral, Rot aggressiv), Größe nach Wurf. Kein Import von Modellen. Das ist Absicht: Die 3D-Entscheidung (Sloyd/Tripo/Meshy) steht noch aus.

## 5. Risiken

- **Gefühl ohne Animation:** Ein Kampf mit Kapseln zeigt Zahlen, aber kein Spielgefühl. Der Test sagt, ob Dauer und Härte stimmen, nicht ob der Kampf Spaß macht.
- **Server-Treffererkennung mit Verzögerung:** Der Schlag wird auf dem Server geprüft; mit höherem Ping fühlt er sich verzögert an. Für 2 bis 8 Spieler im Heimnetz unkritisch, später mit Rückrechnung prüfen.
- **Biom-Karte fehlt:** Spawn nutzt vorerst nur das Startbiom, die Heimat-Regel der anderen Biome ist getestet, aber nicht im Spiel sichtbar.
- **NavMesh:** Gegner brauchen es wie Dorfbewohner; außerhalb des NavMesh-Bereichs bleiben sie stehen. Das Spielgebiet ist begrenzt, im Test ausreichend.
- **Umfang:** 2b.1, 2b.2, 2b.4 und 2b.6 sind die großen Posten. Wenn nötig, 2b.4 zuerst nur mit „aggressiv“ und „passiv“ liefern.
- **Dateigröße:** `GamerCharacter.cpp` ist über 1300 Zeilen; der Waffenschlag kommt deshalb in eine eigene Komponente.

## 6. Abnahme

- Alle alten und neuen Tests grün, Editor- und Server-Build grün.
- Mehrspieler-Lauf mit Server und 2 Clients: Beide sehen denselben Gegner, beide können ihn treffen, Beute und Erfahrung kommen richtig an.
- Messprotokoll: Kampfdauer Rang 1 gegen T1-Satz und gegen nacktes Spielerprofil, verglichen mit der Prüfrechnung in `docs/21` 5.4.
- Spieltest von Hand: Wolf/Keiler besiegen, Beute aufnehmen, Level steigt, Tod im Kampf mit Grab.
- Doku und Entscheidungslog angepasst.

## 7. Offen (vor dem Start)

- Beute-Zuweisung im Koop: jeder würfelt eigen, oder ein gemeinsamer Wurf auf den Boden? Vorschlag: ein gemeinsamer Wurf auf den Boden (einfach, wie bei Ernte), Erfahrung für alle im Umkreis.
- Soll ein toter Gegner liegen bleiben (Kadaver zum Häuten, `ability_skinning`)? Vorschlag: in 2b noch nicht, Beute fällt direkt.
- Soll der Spieler im Kampf Dorfbewohner/Mitspieler treffen können? Vorschlag: Mitspieler nie, Dorfbewohner nie.
