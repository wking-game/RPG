# Projekt-Status „Gamer“ – Übersicht für den PC-Umzug

Stand: 22.11.2026 · Erstellt von Claude nach einem Scan von `game/Source` und `game/Content` (plus kurzer Blick auf die Nachbarordner unter `D:\gamer`).
**Es wurde nichts gelöscht, verschoben oder geändert.** Diese Datei ist nur eine Bestandsaufnahme.

---

## Das Wichtigste in drei Sätzen

1. **Dein Spiel selbst ist klein und ordentlich:** der ganze Programm-Code (ca. 100 Dateien) wird benutzt, da ist kein Müll drin.
2. **Der Content-Ordner ist riesig (ca. 20 GB), aber drei Viertel davon sind gekaufte/geschenkte Asset-Pakete, die dein Spiel im Moment gar nicht benutzt.**
3. **Achtung beim Umzug:** Ein großer Teil deiner neueren Arbeit (u. a. die MetaHuman-Figuren, neue Waffen- und Werkzeug-Modelle) ist **noch nicht in Git gesichert**. Wenn du nur „aus GitHub neu herunterlädst“, fehlt das auf dem neuen PC.

---

## 1. Was schon funktioniert

So steht es in deinen Projekt-Notizen (`docs/02_phasenplan.md`), dort jeweils mit Tests belegt. Ich habe das Spiel für diese Übersicht **nicht selbst gestartet**.

| Bereich | Was das im Spiel heißt |
|---|---|
| **Koop / Mehrspieler** | Bis zu 8 Spieler in einer Welt. Einer ist Gastgeber, andere treten über „Beitreten“ bei. Auch ein eigener Server ohne Bild ist möglich. Getestet nur auf einem PC, noch nicht über zwei echte PCs. |
| **Startmenü** | Welt erstellen (mit Spielmodus, Schwierigkeit, Bau-Optionen), Beitreten, Steuerung, Grafik, Beenden. |
| **Spielfigur** | Laufen, Springen, Sprinten (ohne Ausdauerbalken), Kamera über die Schulter, Animationen. |
| **Sammeln & Abbauen** | Äste, Steine, Fasern von Hand; Bäume und Felsen mit Axt/Spitzhacke per Schlag. Werkzeug wird automatisch gewählt. Pflanzen wachsen sichtbar nach. |
| **Inventar & Schnellleiste** | 24 Plätze, 8er-Schnellleiste, Mausrad zum Durchschalten. |
| **Herstellen** | Menü mit Bildern (Icons), Rezepte an Werkbank und Lagerfeuer. |
| **Bauen** | Holzhaus aus Fundament, Wänden, Tür, Dach usw. Erst Bauplan, dann mit Hammer fertigbauen. Abreißen. |
| **Dorfbewohner** | Ziehen in freie Betten ein, haben Hunger, Schlaf, Unterkunft. Berufe: Sammler, Holzfäller, Steinmetz, Koch, Baumeister, Bauer. Laufen um Häuser herum (Wegfindung). |
| **Lager** | Ein gemeinsames Lager für die ganze Siedlung. |
| **Essen & Feldbau** | Essen verdirbt, Obstbäume, Wildpflanzen, Saatgut, Felder, Komposter, Regenfänger, Taverne mit Gerichten. |
| **Welt** | Eigene Karte (250 × 250 m) mit Gras, Farnen, Bäumen, Felsen, Jahreszeiten. |
| **Vogelperspektive** | Taste B: von oben auf das Dorf schauen, Bewohner anklicken, Berufe ändern. |
| **Leben & Tod** | Lebenspunkte, Heilung, Niedergeschlagen/Wiederbeleben, Grab mit deinen Sachen nach dem Tod. |
| **Speichern** | Spielstände für Welten und Spieler. |
| **Umschauen / Lauschen** | Eine „Scan-Welle“, die Dinge in der Nähe hervorhebt (Grundlage für Magie später). |
| **Einstellungen** | Tasten frei umbelegbar, Grafik-Einstellungen, Schriftgröße. |
| **Leistung** | Gemessen am 21.11.: braucht ca. 2,7 GB RAM und ca. 1 GB Grafikspeicher, deutlich unter deinen Grenzen. **Aber:** noch ohne MetaHuman-Figuren gemessen. |

**Noch nicht da:** Kämpfen mit Waffen, Gegner/Tiere, Quests im Spiel (die Geschichte ist erst als Text in `docs/` geschrieben), Tag/Nacht und Wetter.

---

## 2. Welche Dateien wichtig sind

### A) Unverzichtbar – das IST dein Spiel

| Ort | Was ist das? | In Git gesichert? |
|---|---|---|
| `game/Source/` | Der Programm-Code (alle Spielregeln). ~100 Dateien, **alle werden benutzt**. | Ja |
| `game/Gamer.uproject` | Die „Startdatei“ des Projekts. Doppelklick öffnet es. | Ja |
| `game/Config/` | Einstellungen des Projekts. | Ja – **außer** `DefaultEngine.ini`, die enthält ein privates Token und wird absichtlich nicht hochgeladen. **Diese Datei von Hand mitnehmen!** |
| `game/Content/Data/` | Die Spielwerte (Gegenstände, Rezepte, Berufe, Gebäude …) als JSON-Tabellen. | Ja (größtenteils) |
| `game/Content/Gamer/` | Deine eigenen Modelle, Materialien, Karten (`World`, `Showcase`), Icons, Pflanzen. | **Nur teilweise**, siehe unten |
| `game/Content/Characters/` | Die Standard-Figuren „Manny“ und „Quinn“, die deine Spielfigur gerade benutzt. | Ja |
| `docs/` | Deine Konzepte, Entscheidungen, Phasenplan, Story-Kapitel. | Ja (mit ungespeicherten Änderungen) |
| `design/` | Design-Unterlagen und die KI-Inhalte-Liste (`asset_log.md`, wichtig für Steam). | Ja (mit ungespeicherten Änderungen) |
| `Assets/` | Die Rohdateien deiner Modelle (Blender, FBX, Bilder), aus denen die Unreal-Inhalte gebaut werden. | **Nur teilweise** |
| `tools/` | Hilfs-Skripte zum Bauen, Testen, Importieren. | Ja |
| `CLAUDE.md`, `AGENTS.md`, `README.md` | Anleitungen für die KI-Helfer und dich. | Ja |
| `Spiel_starten.bat`, `Spiel_testen.bat` | Doppelklick zum Spielen/Testen. | Ja |

### B) Wichtig, aber noch NICHT in Git gesichert (beim Umzug unbedingt mitkopieren!)

- **`game/Content/Gamer/Characters/MetaHuman/`** – ca. **5 GB**, rund 1000 Dateien. Deine MetaHuman-Figuren.
- **`game/Content/Gamer/Characters/Clothing/`** – Kleidung für die Figuren.
- Teile von `game/Content/Gamer/Buildings/` und `game/Content/Gamer/Nature/`.
- **`Assets/Items/`** – die neuen Werkzeuge und Waffen (Axt, Schwert, Speer, Schild, Hammer …) als FBX.
- **`Assets/Source/`** – die zugehörigen Blender-Dateien.
- `Assets/Buildings/Stations/`, neue Texturen in `Assets/Buildings/StyleTest/`, `Assets/Previews/`.
- Dazu rund 30 bereits gesicherte Dateien, die du seitdem **geändert** hast (Icons, Doku, Config).
- `game/Config/DefaultEngine.ini` (absichtlich nicht in Git, siehe oben).

Außerdem gibt es **9 Arbeits-Zweige** (Branches) in Git, aber online auf GitHub liegt nur `main`. Wenn die anderen Zweige noch gebraucht werden, müssen sie vor dem Umzug hochgeladen oder der ganze Ordner kopiert werden.

### C) Fremde Asset-Pakete – liegen im Projekt, werden aber NICHT benutzt

Ich habe in deinem eigenen Inhalt, im Code und in den Einstellungen nach Verweisen auf diese Pakete gesucht und **keinen einzigen gefunden**. Sie wurden alle am 17.11. hinzugefügt (vermutlich aus dem Fab/Marketplace) und sind auch nicht in Git.

| Paket | Größe | Was drin ist (laut Name) |
|---|---|---|
| `FantasyBundle` | ~5,2 GB | Burg, Höhle, Dorf (Demo-Karten) |
| `MegascansMeadowPack` | ~2,2 GB | Wiesen-Pflanzen |
| `Viking_Village_Pack` | ~1,9 GB | Wikingerdorf |
| `Weapon_Pack` | ~1,2 GB | Waffen |
| `Elves_Wizard` | ~0,9 GB | Elfen-Figuren |
| `QuadrapedCreatures` | ~0,6 GB | Drache, Greif, Zentaur, Barghest |
| `Viking` | ~0,5 GB | Wikinger |
| `Free_Idle_Animation_Pack_anim` | ~0,4 GB | Stand-Animationen |
| `Stylized_Cloth_Casual_01_for_Kvinsi` | ~0,4 GB | Kleidung |
| `AnimatedWoodenCrateV1` | ~0,4 GB | Holzkiste (die gefundenen Verweise kommen nur aus der eigenen Demo-Karte des Pakets) |
| `ModuleManager` | ~0,4 GB | unklar |
| `Viking_Warrior` | ~0,2 GB | Krieger-Figur |
| `PN_GroundFoliage` | ~0,2 GB | Bodenpflanzen |
| `FreeMusicPack` | ~0,1 GB | Musik |
| `HighPoly_Tree_Model` | ~40 MB | ein Baum |
| `RPG_Fantasy_SFX` | ~10 MB | Soundeffekte |
| **Summe** | **ca. 15 GB** | |

**Wichtig:** „Nicht benutzt“ heißt nicht „Müll“. Vielleicht wolltest du daraus noch Dinge nehmen (z. B. Musik, Soundeffekte, Tiere, Waffen). Und: Wenn du diese Pakete in dein Spiel einbaust, werden sie beim Fertigstellen mitgepackt – dein Ziel ist höchstens ca. 20 GB fürs fertige Spiel. Meine Suche war eine Textsuche in den Dateien; sie ist zuverlässig, aber nicht 100 % sicher. Vor einem Löschen sollte man das im Unreal-Editor mit dem „Reference Viewer“ noch einmal gegenprüfen.

### D) Automatisch erzeugte Ordner – muss man NICHT umziehen

Diese Ordner baut Unreal bzw. Visual Studio von selbst neu. Sie mitzukopieren schadet nicht, kostet aber Zeit und Platz:

- `game/Binaries/`, `game/Intermediate/`, `game/DerivedDataCache/`, `game/Build/`, `game/.vs/`, `game/Saved/` (Ausnahme: in `Saved/` liegen ggf. **Spielstände und deine Tasten-/Grafik-Einstellungen** – wenn dir die wichtig sind, `Saved/SaveGames` und `Saved/Config` mitnehmen)
- `game/*.sln`, `game/*.slnx` (Projektdateien für Visual Studio, werden neu erzeugt)
- `D:\gamer\unreal\DDC` – **ca. 38 GB** Zwischenspeicher. Wird neu aufgebaut (der erste Start auf dem neuen PC dauert dann länger, weil Shader neu übersetzt werden).

### E) Außerhalb des Projektordners (unter `D:\gamer`)

| Ordner | Größe | Einschätzung |
|---|---|---|
| `unreal\UE_5.8` | ~30 GB | **Alte Unreal-Installation.** Laut `CLAUDE.md` wurde sie am 16.11. gelöscht und durch die selbst gebaute Engine in `E:\UE58` ersetzt – sie ist aber noch da (Datum 17.11.). Dein Projekt benutzt sie nicht. Bitte prüfen, ob sie noch gebraucht wird. |
| `heruntergeladenes` | ~7,6 GB | Downloads (davon 6,3 GB Sounds, dazu ZIPs wie Medieval Village MegaKit, Fantasy Props, Animationen). Rohmaterial, nicht im Spiel. Mitnehmen, wenn du es noch verwenden willst. |
| `Gamer_Playtest_2026-11-15_v3.zip` | ~1,4 GB | Alte, fertig gepackte Testversion vom 15.11. Kann jederzeit neu gebaut werden. |
| `programme\whisper` | ~1,7 GB | Spracherkennungs-Programm (für deine Spracheingabe?). Nicht Teil des Spiels. |
| `referensbilder - Modell` | ~1,4 GB | Referenzbilder und Test-3D-Modelle. |
| `cc0_quellen` | ~0,75 GB | Freie Original-Texturen/Modelle, aus denen deine Pflanzen und Felsen gebaut sind. **Mitnehmen**, damit sie sich neu bauen lassen. |
| `reference_contact_temp.jpg` | 1 MB | Sieht nach einer Testdatei aus („temp“). |
| `gegenstände blender`, `ki3d_rohdaten` | leer | Leere Ordner. |

**Nicht vergessen – liegt auf Laufwerk E:**
- `E:\UE58` – **die Unreal Engine, mit der dein Projekt wirklich läuft** (selbst gebaut, das hat ca. 10 Stunden gedauert). Auf dem neuen PC entweder komplett mitkopieren oder neu bauen (Anleitung in `docs/05_source_build_plan.md` und `docs/03_setup_checkliste.md`).
- `E:\Git`, `E:\VS\BuildTools`, `E:\Blender` – Programme, auf dem neuen PC neu installieren.

---

## 3. Was als Nächstes sinnvoll wäre

### Vor dem Umzug (in dieser Reihenfolge)

1. **Alles Wichtige in Git sichern.** Die ungesicherten Dateien aus Abschnitt 2B „committen“ und zu GitHub hochladen. Achtung: der MetaHuman-Ordner ist ca. 5 GB groß – vorher klären, ob der in Git (mit LFS) soll oder lieber separat kopiert wird. Claude kann dabei helfen.
2. **Die anderen Git-Zweige hochladen** oder bewusst entscheiden, dass sie weg können.
3. **`game/Config/DefaultEngine.ini` separat sichern** (USB-Stick o. Ä.), weil sie nicht in Git ist.
4. **Am sichersten:** zusätzlich den ganzen Ordner `D:\gamer\gamer` auf eine externe Festplatte kopieren. Die Ordner aus Abschnitt 2D kannst du dabei weglassen.
5. **Engine klären:** `E:\UE58` mitnehmen (am schnellsten) oder auf dem neuen PC neu bauen.

### Auf dem neuen PC

6. Git, Visual Studio Build Tools, Blender installieren (Liste: `docs/03_setup_checkliste.md`).
7. Engine einrichten, Projekt kopieren/herunterladen, `Gamer.uproject` öffnen. Der erste Start dauert lange (Shader).
8. Einmal `Spiel_testen.bat` starten und kurz prüfen, ob Sammeln, Bauen und Bewohner gehen.
9. Pfade in `CLAUDE.md` anpassen, falls die Laufwerke anders heißen (dort stehen z. B. `E:\UE58` und `E:\Git`) – das ist eine „kernelementare“ Datei, Claude fragt vorher.

### Aufräumen (erst nach dem Umzug, nur mit deiner Zustimmung)

10. Entscheiden, welche der **nicht benutzten Asset-Pakete** (ca. 15 GB) du behalten willst. Die übrigen könnte man aus dem Projekt nehmen – vorher im Editor gegenprüfen.
11. Die alte Engine `D:\gamer\unreal\UE_5.8` (30 GB) entfernen, wenn sicher ist, dass sie nicht gebraucht wird.
12. `CLAUDE.md` (Abschnitt „Aktueller Stand“) und `README.md` aktualisieren – die `README.md` ist vom 12.11. und sagt noch „Unreal-Projekt noch nicht angelegt“.

### Für das Spiel selbst (laut deinem Phasenplan)

13. **Leistung neu messen, sobald die MetaHuman-Figuren im Spiel sind** – die werden deutlich mehr Grafikspeicher brauchen (deine Grenze: 8 GB).
14. Danach geht es laut Plan mit **Kampf und Ausrüstung** (Phase 3) weiter; die Waffen-Modelle dafür liegen schon in `Assets/Items/`.

---

## 4. Deine Wunschliste vom 22.11.2026

Du hast per Sprache diktiert. So habe ich es verstanden. **Wo ich unsicher bin, steht „Deutung“ dabei, bitte bestätigen oder korrigieren.**

### Bauen mit dem Bauplan-System (der größte und genaueste Wunsch)

| Wunsch | Was es schon gibt | Was neu wäre |
|---|---|---|
| Bauen läuft über Baupläne (Blaupausen) | Ja: Bauplan setzen, dann mit dem Hammer fertig schlagen (Weltoption „Bauplan + Hammer“) | – |
| Die Rohstoffe für jedes Objekt müssen **angeliefert** werden | Teilweise: Bei „Material beim Bauen“ holt der Baumeister das Material aus dem Lager | Rohstoffe werden sichtbar **zum Bauplan gebracht** (von Bewohnern oder vom Spieler), erst dann geht es weiter |
| Objekte entstehen in **3 Stufen**, jede Stufe braucht andere Rohstoffe, und es wird nur geliefert, was die jeweilige Stufe kostet | Nein. Es gibt „Ausbaustufen“ für Arbeitsgebäude, aber das ist etwas anderes (Verbesserung nach dem Bau) | z. B. Stufe 1 Gerüst/Fundament (Stämme), Stufe 2 Rohbau (Bretter, Steine), Stufe 3 Fertigstellung (Fasern, Lehm …). Jede Stufe sieht im Spiel anders aus |
| Spieler setzt den Bauplan, Bewohner bauen ihn auf | Ja, der Beruf Baumeister macht das schon | Bewohner liefern **und** bauen je Stufe |
| Spieler können auch **selbst** bauen | Ja, mit dem Hammer | **Deutung:** Der Spieler kann statt der Bewohner selbst liefern und hämmern. **Oder meintest du:** Spieler können eigene Baupläne *entwerfen* (z. B. ein ganzes Haus als Vorlage speichern und wieder setzen)? |
| **Koop-Rechte pro Objekt:** Wer den Bauplan setzt, bestimmt für genau dieses Objekt, ob Mitspieler nur **mitbauen** dürfen oder auch **abreißen** | Nein. Abreißen ist bisher nur eine Weltoption für alle | Neue Einstellung je Objekt, nur der Besitzer darf sie ändern; der Server prüft sie |

**Offene Fragen dazu:**
- Gelten die 3 Stufen für **jedes** Bauteil (auch für eine einzelne Wand) oder nur für ganze Gebäude und Arbeitsstätten? Bei einzelnen Wänden würde das sehr kleinteilig.
- Wie verhalten sich die 3 Stufen zu den bestehenden Weltoptionen „sofort fertig“ und „Material beim Bauen“? Vorschlag: Die Stufen werden der neue Standard für „Bauplan + Hammer“, „sofort fertig“ bleibt als Option.
- Sollen Mitspieler standardmäßig „nur mitbauen“ dürfen? (Vorschlag: ja.)
- Was passiert beim Abreißen einer halbfertigen Stufe – gibt es alles zurück, was schon geliefert wurde? (Vorschlag: ja.)

### Weitere Wünsche (nur Stichworte, noch nicht genauer besprochen)

| Wunsch | Was es schon gibt |
|---|---|
| **Karte** (Weltkarte, evtl. Minikarte) | Nein. Die Spielerfarbe „für die Karte“ ist schon beschlossen |
| **Tutorial** | Nein, nur einzelne Anfänger-Tipps (z. B. beim Verderb) |
| **Charakter-Ersteller und -Anpassung** mit Hintergrundgeschichte, Werten, Fähigkeiten | Teilweise: Daten und Prüfung fertig (5 Werte, 24 Fähigkeiten, 5 Herkünfte, Aussehen in `characters.json`), Entwurf in `docs/11_charaktere.md`, MetaHuman beschlossen. **Ob das Menü schon im Spiel nutzbar ist, habe ich nicht geprüft** |
| **Kreaturen und Tiere** | Nein. Beschlossen: zuerst nur Menschen und Tiere, Sagenwesen höchstens später als Weltoption. Hinweis: Im Projekt liegt ein ungenutztes Paket mit Drache, Greif usw. – das passt nicht zu diesem Beschluss |
| **Höhlen** | Nein. In der Story gibt es eine Eishöhle (Kapitel 5) |
| **Wetter** | Nein, aber beschlossen (Regen, Gewitter, Nebel, Schnee, Sturm, mit leichter Wirkung auf Felder usw.) |
| **Sound** (Geräusche, Musik) | Nein. Es gibt noch keine Geräusche und keine Lautstärke-Einstellung |
| **Terraforming** (Boden umgraben, ebnen, Hügel abtragen) | Nein. Achtung: Die Welt hat noch keine echte Landschaft (nur einen flachen Boden), Terraforming braucht die zuerst. Im Koop ist es außerdem technisch aufwendig |

### Ehrliche Einschätzung zum Umfang

Das sind zusammen **mehrere Monate Arbeit**. Deine eigene Regel 9 lautet: „erst Vertical Slice, dann ausbauen“. Höhlen, Kreaturen und Terraforming gehören laut Phasenplan eher in Phase 4 (Welt).

**Vorschlag für die Reihenfolge:**
1. **Erst der Umzug** (Abschnitt 3), damit nichts verloren geht.
2. **Bauen mit 3 Stufen, Anlieferung und Koop-Rechten**, weil es direkt auf dem bestehenden Bausystem aufbaut und Kern der Siedlung ist.
3. **Sound (Grundgeräusche) und Karte**: klein, macht das Spiel sofort greifbarer.
4. **Tutorial**: erst sinnvoll, wenn sich die Grundabläufe nicht mehr ändern.
5. **Charakter-Ersteller im Spiel fertigstellen** (Daten gibt es schon).
6. **Tiere, dann Wetter**.
7. **Später (Phase 4):** Höhlen, echte Landschaft, Terraforming.
