# Kreaturen, Level und Kampfwerte (bereinigte Fassung „Küstenasche“)

Stand 2026-11-22. Grundlage ist Geminis „Küstenasche – Gesamtdokumentation“ (Kopie im Projekt: `design/referenz/gemini_kuestenasche_gesamtdokumentation.md`), angepasst an die Entscheidungen vom 2026-11-22 (`docs/01_entscheidungen.md`, acht Zeilen ab „Gegnerarten: Fantasy-Kreaturen (Antwort)“).

- **Dieses Dokument:** Regeln, Formeln, Begründungen, Tiere, Bosse, Herstellen, Technik.
- **`docs/22_kreaturen_daten.md`:** alle Tabellen zu den 48 Kreaturen (Einordnung, Werte je Rang, Spawn je Biom, bereinigte Beute, Gegenstands-Katalog). Es wird von `tools/data/creatures_from_gemini.py` erzeugt und nicht von Hand geändert.
- **Nicht hier:** Story und Boss-Texte (eigene Sitzung „Sagen-Bosse“), Modelle und Animationen (Asset-Plan).

Kennzeichen wie im Entscheidungslog: **bestätigt** = vom Nutzer entschieden · **Vorschlag** = von Claude · **Gemini** = aus dem Gemini-Dokument übernommen, nicht einzeln bestätigt. **Alle Zahlen sind Startwerte.** Einstellen kann man sie erst, wenn man kämpfen kann (Phase 3).

**Durchgang vom 2026-11-22:** Der Nutzer hat die Vorschläge dieses Dokuments durchgesehen. **Bestätigt** sind: die Level-Zuordnung (2.1), Spieler-LP ohne Level-Zuwachs (4), drei Anführer-Stufen (6.4), ein Wurf für die Streuung mit folgender Größe (6.3), der Regler `creature_hp_scale` (5.4), Beute ohne Güte-Varianten (10.2), deutsche Anzeigenamen (7) und die übrigen Zahlen als Startwerte. Dazu kamen Entscheidungen zum zweiten Gemini-Dokument, siehe `docs/23_bauen_rechte_logistik.md`. Was unten noch „Vorschlag“ heißt, sind Startwerte zum Justieren.

---

## 1. Was feststeht

| Punkt | Inhalt | Status |
|---|---|---|
| Gegner | 48 Kreaturen aus dem Gemini-Dokument, dazu Tiere und Menschen; keine Wasserwesen | bestätigt (2026-11-22) |
| Bosse | Sagen-Fassung als Standard (Moorhexe, Draugr-Fürst, Troll, Lindwurm, Frostriese); der Alte Keiler bleibt realistisch; Weltoption „Sagenwelt“ entfällt | bestätigt (2026-11-22) |
| Fortschritt | Spielerlevel 1–50; Rezepte haben eine Level-Anforderung | bestätigt (2026-11-22) |
| Erfahrung | aus allem (Kampf, Bosse, Quests, Entdecken, Herstellen, Bauen, Sammeln); Mitspieler in der Nähe bekommen einen Anteil; Aufstieg gibt Fähigkeitspunkte; Werte steigen weiter durch Tun | bestätigt (2026-11-22) |
| Gebiete | die 6 Biome bleiben, Geminis 5 Gebiete werden zugeordnet | bestätigt (2026-11-22) |
| Beute | Kreaturen fein wie bei Gemini (Sammelbegriffe werden konkret), Tiere schlank | bestätigt (2026-11-22) |
| Herstellen | Metall-Leiter T1–T6 bleibt das Rückgrat, Kreaturenteile kommen als Zusatz und für Spezialsets dazu | bestätigt (2026-11-22) |
| Reihenfolge | alle 48 Kreaturen zuerst als Daten, Modelle gestaffelt, zuerst der Kern | bestätigt (2026-11-22) |
| Kampf | Richtungshiebe (links, rechts, oben) plus Stoß, Zielerfassung, Ausweichschritt, Trefferzonen Kopf/Körper/Glieder | bestätigt (2026-11-13) |
| Ausdauer | leere Ausdauer macht nur langsamer und schwächer, sperrt nie (Regel 5) | bestätigt (2026-11-16) |
| Ausrüstungs-Slots | Helm, Schulter, Oberkörper, Hände, 2 × Ring, Beine, Füße, Umhang | bestätigt (2026-11-12/13) |
| Bauraster | 300 cm (Gemini: 200 cm) | festgestellt (so gebaut, `GamerBuildSnapping.h`) |
| Level-Zuordnung | T1 1–8, T2 9–16, T3 17–25, T4 26–34, T5 35–42, T6 43–50 (2.1) | bestätigt (2026-11-22) |
| Spieler-LP | 100 + 5 × (Zähigkeit − 5) + Mahlzeiten-Buff, kein Zuwachs durch Level | bestätigt (2026-11-22) |
| Anführer-Stufen | drei Stufen statt Captain-Rang 25 (6.4) | bestätigt (2026-11-22) |
| Streuung | ein Wurf 85–115 % für LP, Haltung, Schaden; Größe folgt (6.3) | bestätigt (2026-11-22) |
| Kreaturen-LP | Geminis Werte, dazu Regler `creature_hp_scale` (Start 1,0) | bestätigt (2026-11-22) |
| Beute-Umfang | ohne Güte-Varianten, 162 Gegenstände (10.2) | bestätigt (2026-11-22; Nutzer nannte „160“, die Zählung ergibt 162) |
| Namen | deutsche Anzeigenamen, IDs englisch und stabil | bestätigt (2026-11-22); die Namen selbst sind Vorschlag |
| Startwerte | Erfahrung, Haltung, Angriffsfaktoren, Tiere, Bosse, Fähigkeitspunkte als Startwerte übernommen | bestätigt (2026-11-22) |

---

## 2. Level, Stufen und Biome

### 2.1 Zuordnung (Vorschlag)

Gemini teilt Level 1–50 in fünf Phasen. Wir haben sechs Stufen und sechs Biome. Darum ordne ich die Level so zu, dass jede Stufe etwa 8 Level umfasst, nah an Geminis Rang-Spalte:

| Stufe | Biom | Level | Biom-Rang | Gefährliche Orte (Rang + 1) | Metall | Boss (Sagen-Fassung) → Belohnung |
|---|---|---|---|---|---|---|
| T1 | Fjordwiesen | 1–8 | 1 | Banditenlager, Wolfshöhlen (2) | Rohstufe (Stein, Holz, Knochen) | Der Alte Keiler (realistisch) → Bauplan Rennofen |
| T2 | Gezeitenmarsch | 9–16 | 2 | Wracks, tiefe Priele (3) | Raseneisen | Schlickmutter als Moorhexe → Bossmaterial T3 |
| T3 | Hügelgrab-Heide | 17–25 | 3 | Grabhügel, Gewölbe, Grabfestung (4) | Schmiedeeisen | Hügelkönig als Draugr-Fürst → T4 |
| T4 | Basaltfjell | 26–34 | 4 | Gipfel, Eishöhlen (5) | Stahl | Felsenvater als Troll → T5 |
| T5 | Glutküste | 35–42 | 5 | Lavafelder, Kultstätten (6) | Damast | Aschezunge als Lindwurm → Relikt (T6) |
| T6 | Eisrand | 43–50 | 6 | Gletscherspalten (6, mehr Anführer) | Relikt | Sturmkoloss als Frostriese → Endsieg |

Die Belohnungen stammen aus `design/ASSET_LIST_GESAMT.md`. **Eine Stufe öffnet sich doppelt:** Das Level muss erreicht sein (bestätigt: Rezepte mit Level-Anforderung), und das Material bzw. der Bauplan kommt vom Boss der vorigen Stufe. Im Spielmodus „Freies Spiel“ findet man die Bosse durch Erkunden.

### 2.2 Ausrüstungswerte je Stufe

Die Werte sind aus Geminis Level-Tabelle (Abschnitt 2.2 dort) auf die Level-Bereiche umgerechnet, mit linearer Einteilung innerhalb von Geminis Fünfer-Schritten. Sie gelten für eine Standard-Einhandwaffe und ein volles, mittelschweres Rüstungsset.

| Stufe | Level | Waffen-Schaden | Haltungsschaden | Rüstung (Set) | Haltungswiderstand (Set) | Resistenz je Art höchstens |
|---|---|---|---|---|---|---|
| T1 | 1–8 | 14–24 | 12–19 | 15–37 | 20–35 | 8 % |
| T2 | 9–16 | 26–39 | 21–30 | 41–66 | 37–53 | 16 % |
| T3 | 17–25 | 41–60 | 32–45 | 71–110 | 56–80 | 26 % |
| T4 | 26–34 | 61–86 | 46–62 | 111–156 | 81–108 | 36 % |
| T5 | 35–42 | 89–111 | 64–79 | 162–199 | 112–136 | 46 % |
| T6 | 43–50 | 115–145 | 83–105 | 208–260 | 142–175 | 55 % |

**Innerhalb einer Stufe (Vorschlag):** Das Grundrezept am Stufenanfang gibt den unteren Wert. Verbesserte Fassungen mit Kreaturenteilen (höhere Level-Anforderung) heben den Wert zur Mitte, ein Meisterstück am Stufenende bis zum oberen Wert. So ersetzt die Zusatz-Regel aus Abschnitt 11 Geminis stufenlose Kurve.

### 2.3 Archetypen (Gemini, unverändert)

| Archetyp | Schaden | Haltungsschaden | Rüstung | Haltungswiderstand |
|---|---|---|---|---|
| leichte Waffe (Dolch, Sax) | × 0,80 | × 0,65 | – | – |
| Standard (Schwert, Axt, Speer) | × 1,00 | × 1,00 | – | – |
| schwer (Zweihänder, Hammer, Breitaxt) | × 1,25–1,40 | × 1,35–1,60 | – | – |
| leichte Rüstung | – | – | × 0,75–0,85 | × 0,70–0,80 |
| schwere Rüstung | – | – | × 1,15–1,30 | × 1,20–1,35 |

Leichte Rüstung bringt dafür schnellere Ausdauer-Erholung und Beweglichkeit. Nach Regel 5 heißt das nur „schneller“, nie „schwere Rüstung sperrt etwas“.

---

## 3. Erfahrung und Fähigkeitspunkte

Bestätigt ist: Erfahrung kommt aus allem, Mitspieler in der Nähe bekommen einen Anteil, jeder Aufstieg gibt Fähigkeitspunkte. Gemini nennt dazu keine Zahlen. Die folgenden Startwerte sind ein **Vorschlag**:

| Quelle | Erfahrung (Stufe = Stufe der Quelle) |
|---|---|
| Gegner besiegen | 10 × Rang; Anführer-Stufen × 1,5 / × 2 / × 3 (Abschnitt 6.4) |
| Boss, erster Sieg pro Spieler | 300 × Stufe |
| Quest-Etappe | 50 × Stufe |
| neuer Ort, Steinkreis, Biom entdeckt | 25 × Stufe |
| Rezept zum ersten Mal hergestellt | 20 × Stufe; danach 1 × Stufe je Stück |
| Bauteil fertig gebaut | 2 × Stufe |
| Sammeln, Abbauen, Ernten | 0,5 × Stufe je Stück |

- **Bedarf je Level (korrigiert 2026-11-22):** Die frühere Formel `100 × Level^1,5` war falsch: Mit „10 × Rang je Gegner“ hätte man bis Level 8 etwa 600 besiegte Rang-1-Gegner gebraucht, bis Level 50 rund 690 000 Erfahrung. Jetzt wird die Kurve aus **Spielstunden je Stufe** berechnet (Annahme, solo: 6 / 8 / 10 / 12 / 14 / 16 Stunden für T1–T6, zusammen etwa 66 Stunden). Grundlage ist eine „typische Stunde“ mit **460 Erfahrung je Stufe** (15 Kämpfe, 1 Quest-Etappe, 1 Entdeckung, 3 erste und 20 weitere Herstellungen, 40 Bauteile, 150 gesammelte Stücke). Ergebnis (`progression.json`, `xp_to_next`): Level 1 → 2 braucht 95, Level 8 → 9 braucht 595, Level 49 → 50 braucht 7 220 Erfahrung; bis Level 8 sind es 2 165, bis Level 50 insgesamt 122 370. Der Bedarf je Level steigt gleichmäßig, auch über die Stufengrenzen (die Stufenmitten werden linear verbunden, danach wird jede Stufe auf ihre Summe skaliert), damit er an keiner Grenze sinkt. Das Zieltempo wird mit dem ersten Kampf-Spieltest geprüft; die Stunden sind eine Annahme.
- **Anteil im Koop:** Mitspieler im Umkreis von 50 m (wie die Saga-Gutschrift) bekommen bei Kampf, Bossen, Quests und Entdecken den vollen Wert, bei Herstellen, Bauen und Sammeln 25 %.
- **Bremse gegen Farmen:** Gegner unter der eigenen Stufe geben weniger: eine Stufe darunter 75 %, zwei 25 %, drei oder mehr 10 %. Die eigene Stufe ergibt sich aus dem Level nach Abschnitt 2.1.
- **Fähigkeitspunkte:** 1 je Aufstieg, bei Level 5, 10, … 50 je einer extra, also 59 Punkte bis Level 50. Der Skilltree selbst fehlt noch.
- **Werte (Stärke, Geschick, Zähigkeit, Handwerk, Wille)** steigen weiter durch Tun (`docs/11_charaktere.md`). Das Level erhöht sie nicht direkt.

---

## 4. Lebenspunkte und Haltung des Spielers

Gemini legt keine Lebenspunkte für den Spieler fest. **Vorschlag:**

- **Lebenspunkte = 100 + 5 × (Zähigkeit − 5) + Mahlzeiten-Buff „Gesundheit“.** Die 100 stehen schon in `health.json`. Kein Zuwachs pro Level: Das hält Koop-Gruppen mit unterschiedlichem Level fair, ein neuer Mitspieler wird über Ausrüstung mitgezogen.
- **Haltung des Spielers:** Leiste 100. Bei Bruch taumelt man 0,8 s. Nach 2 s ohne Treffer erholt sie sich um 30 pro Sekunde. Keine Verbindung zur Ausdauer.

---

## 5. Kampfformeln

### 5.1 Rüstung pro Trefferzone (Vorschlag auf Basis der Gemini-Formel)

Geminis Formel bleibt: **Reduktion = min(0,75 ; R / (R + 160))** und **Schaden = max(1 ; Roh × Zonenfaktor × (1 − Reduktion))**. Neu ist, dass R je Trefferzone aus den Slots berechnet wird (bestätigt sind die drei Zonen und der Schutz durch die passenden Slots):

| Slot | Anteil am Set | Zone |
|---|---|---|
| Helm | 20 % | Kopf: R = Helm × 5 |
| Schulter | 10 % | Körper: R = (Schulter + Oberkörper) × 2 |
| Oberkörper | 40 % | |
| Hände | 10 % | Glieder: R = (Hände + Beine + Füße) × 10/3 |
| Beine | 15 % | |
| Füße | 5 % | |
| Umhang, Ringe | – | keine Rüstung; Resistenzen bzw. Wirkungen |

Mit vollem Set bekommt jede Zone den Set-Wert aus Abschnitt 2.2. Fehlt der Helm, ist der Kopf ungeschützt. **Zonenfaktoren (Vorschlag):** Kopf × 1,5 · Körper × 1,0 · Glieder × 0,7.

**Kreaturen** tragen keine Rüstung. Sie bekommen Trefferzonen mit Faktoren in den Daten: Schwachpunkt × 1,5, Panzer × 0,5. Beispiele aus Geminis Beschreibungen: Slagbrute mit Frontkruste (Panzer vorne), Cindercolossus mit Schwachstellen in den Gelenken, Ashweaver mit glimmenden Schwachpunkten.

### 5.2 Resistenzen und Umweltgefahren

Formel (Gemini, unverändert): **wirksame Resistenz = Basis + Ausrüstung + Buffs, begrenzt auf −25 % bis 60 %**; **Wirkung = Roh × (1 − Resistenz)**.

| Gefahr | Biom | Wirkung ohne Schutz (Vorschlag, Regel 5: nur langsamer oder schwächer) | Buff-Art |
|---|---|---|---|
| Hitze | Glutküste | Durst steigt doppelt so schnell; nahe an Lava 2 LP/s | `heat_resist` (neu) |
| Frost | Basaltfjell, Eisrand | Ausdauer-Erholung −40 %; Kälte wirkt stärker | `cold_resist` (gibt es schon) |
| Sturm | Eisrand | Böen machen 10 Haltungsschaden/s; gegen den Wind −20 % Tempo | `storm_resist` (neu) |
| Verfall/Sporen | Gezeitenmarsch, Hügelgrab-Heide | keine LP-Regeneration; in Sporenwolken 3 LP/s | `poison_resist` (gibt es schon) |

Hitze und Sturm brauchen Durst und Wetter. Beides kommt mit Tag/Nacht und Wetter.

### 5.3 Haltung (Poise)

Gemini, ergänzt um den Abbau, der dort fehlt:

- **Gegner:** Haltung += Waffen-Haltungsschaden × Angriffsfaktor × Zonenfaktor. Ist die Leiste voll, kommt der **Haltungsbruch**: Der Gegner taumelt 2,5 s, Treffer in dieser Zeit machen × 1,5 Schaden, danach ist die Leiste leer. Bosse taumeln 3 s. **Abbau (neu):** nach 3 s ohne Treffer 25 % der Leiste pro Sekunde.
- **Spieler:** eingehender Haltungsschaden × 90 / (Haltungswiderstand + 90) (Gemini). Beispiel: 150 Haltungswiderstand dämpfen um 62,5 %.

**Angriffsfaktoren (Vorschlag, passend zum bestätigten Richtungskampf):**

| Aktion | Schaden | Haltungsschaden |
|---|---|---|
| leichter Hieb (links, rechts, oben) | × 1,0 | × 1,0 |
| Stoß | × 0,9 | × 0,8, an Schwachpunkten × 1,5 |
| schwerer (aufgeladener) Hieb | × 1,4 | × 1,6 |
| Schildstoß | 0 | × 2,0 |
| Gegenschlag nach perfektem Parieren | × 1,2 | × 1,5 |

### 5.4 Wie lange dauert ein Kampf? (Prüfrechnung)

Mit Geminis Werten, Waffe und Rüstung aus der Stufenmitte und ohne Parieren:

| Gegner | Treffer bis zum Sieg (Körper / Kopf) | Schwere Treffer, die der Spieler aushält |
|---|---|---|
| Bramblefang, Rang 2 (410 LP), T2-Waffe ~32 | 13 / 9 | 8 (13,6 Schaden je Treffer, 100 LP) |
| Kraggar, Rang 4 (1.120 LP), T4-Waffe ~74 | 16 / 11 | 6 (19 je Treffer, 100 LP) |
| Galewyrm, Rang 6 (2.900 LP), T6-Waffe ~130 | 23 / 15 | 6 (30 je Treffer, 160 LP) |
| Galewyrm als Anführer mit Streuung 115 % | 35 / 23 | 4 (43 je Treffer, 160 LP) |

**Einschätzung (Regler bestätigt 2026-11-22):** Für einen KCD-artigen Kampf sind Geminis Lebenspunkte eher hoch. Kopftreffer und Haltungsbrüche verkürzen das zwar, trotzdem schlage ich einen **Regler `creature_hp_scale`** in den Daten vor (Start 1,0). Mit ihm lassen sich nach dem ersten Kampftest alle Kreaturen auf einmal anpassen, statt 48 Zeilen einzeln zu ändern.

---

## 6. Gegner-Rang, Streuung und Anführer-Stufen

### 6.1 Rang im Biom

- **Biom-Rang = Stufe des Bioms.** An **gefährlichen Orten** gilt Rang + 1, höchstens 6 (Tabelle 2.1). So bleibt etwas von Geminis Gebieten mit mehreren Rängen erhalten.
- **Geminis Ränge 2–6 bleiben**, Rang 1 sind Tiere und Menschen. Gemini hatte für Level 1–5 Rang 1, aber keine Kreatur mit Rang 1. Diese Lücke ist damit geschlossen.
- **Ränge über 6 gibt es nicht.** An die Stelle von Geminis Captain-Rängen 7–25 treten die Anführer-Stufen (6.4).
- Eine Kreatur kommt nur in ihrem **Heimat-Biom und in späteren Biomen** vor, nie früher. Sonst bräuchte man ihr Modell vor ihrer Welle (Abschnitt 7).

### 6.2 Mitwachsen und Nachzügler (Gemini, übertragen)

Geminis Felder bleiben und heißen in den Daten `scale_to_zone`, `scale_chance`, `max_rank` und `fixed_rank`:

- **wächst mit (z. B. 85 %):** 85 % der Exemplare bekommen den Rang des Ortes, der Rest einen Rang darunter, mindestens den MinRang. Geminis Text ließ offen, was mit dem Rest passiert. Das ist die Festlegung dafür (Vorschlag).
- **wächst mit bis Rang X:** wie oben, aber höchstens Rang X.
- **fest Rang X (Nachzügler):** bleibt auf Rang X. Das sind die schwachen Kontrast-Gegner in starken Gebieten, eine Idee von Gemini.

Die Spawn-Tabellen je Biom stehen in `docs/22_kreaturen_daten.md`, Abschnitt 5.

### 6.3 Streuung (Vorschlag)

Gemini: jedes Exemplar zufällig 85–115 % auf LP, Haltung und Schaden. Weil das alle drei Werte trifft, ist ein starkes Exemplar fast doppelt so gefährlich wie ein schwaches (0,85² gegen 1,15²), ohne dass man es sieht. **Lösung:** ein einziger Wurf für alle drei Werte, und **die Größe folgt ihm**: Größe × (1 + 0,5 × (Wurf − 1)), also ±7,5 %. Größere Tiere sind stärker, das kann man lesen. Wirkt es im Test trotzdem unfair, die Spanne auf 90–110 % verkleinern.

### 6.4 Anführer-Stufen statt Captain-Rang 25 (Vorschlag)

Geminis Kurve von Rang 6 bis 25 bringt am Ende nur + 35 % LP, + 20 % Haltung und + 25 % Schaden. Rang 7 ist gerade + 5 % stärker als Rang 6, sichtbar ist das nicht. Ich fasse das in **drei sichtbare Stufen** mit Geminis eigenen Kurvenwerten bei Rang 10, 17 und 25:

| Stufe | entspricht bei Gemini | LP | Haltung | Schaden | sichtbar | Beute | wo und wie oft |
|---|---|---|---|---|---|---|---|
| Erfahren | Captain-Rang ~10 | × 1,13 | × 1,07 | × 1,09 | 5 % größer, Narben | + 1 Wurf Grundbeute | ab Rang 2, 5 % (Orte 10 %) |
| Veteran | ~17 | × 1,25 | × 1,14 | × 1,18 | 10 % größer, Name „Alter …“ | + 1 Wurf Grund- und Rangbeute | ab Rang 4, 3 % (Orte 6 %) |
| Anführer | 25 (Geminis Höchstwert) | × 1,35 | × 1,20 | × 1,25 | 15 % größer, eigener Name | Rangbeute doppelt | nur Rang 6, 2 % (Orte 4 %) |

Höchstens ein Exemplar mit Stufe pro Gruppe. Sollen sich Anführer stärker anfühlen, die Faktoren nach dem Test erhöhen. Geminis Werte sind bewusst vorsichtig.

---

## 7. Kreaturen

Alle 48 Kreaturen mit ID, Rolle, Heimat, Rängen und Werten stehen in `docs/22_kreaturen_daten.md`. Kurzfassung der Modell-Wellen (bestätigt: Daten alle, Modelle gestaffelt):

| Welle | Heimat-Biom | Kreaturen |
|---|---|---|
| Kern | Fjordwiesen | keine Kreaturen: Tiere, Waldbanditen, Alter Keiler |
| Kern (1) | Gezeitenmarsch | 4: Bramblefang, Siltstrider, Mirecrawler, Thornmaw (+ Irrlichter als Boss-Begleiter) |
| 2 | Hügelgrab-Heide | 13 |
| 3 | Basaltfjell | 14 |
| 4 | Glutküste | 10 |
| 5 | Eisrand | 7 |

Das passt zum Stil „Alltag historisch, Ränder mythisch“: Je weiter vom Start, desto fantastischer. **Namen (bestätigt 2026-11-22):** Im Spiel heißen die Kreaturen deutsch bzw. nordisch. Die Anzeigenamen stehen in `docs/22_kreaturen_daten.md`, Abschnitt 3 (z. B. Rimehound → Reifhund, Slagbrute → Schlackenbrecher, Galewyrm → Sturmwurm); sie sind ein Vorschlag, den du vor dem Release durchgehst. IDs bleiben `creature_<name>` auf Englisch und stabil (Riese = `creature_giant`, Wiedergänger = `creature_revenant`). Der Wegfall der englischen Geminis-Namen erledigt auch die Markenfrage (Ironhide, Cragmaw).

---

## 8. Tiere und Menschen

### 8.1 Tiere: Werte (Vorschlag)

Gemini hat keine Tiere. Die Werte gelten im Heimat-Biom des Tiers. In höheren Biomen pro Rang LP × 1,4, Haltung × 1,25, Schaden × 1,25. Das entspricht dem Wachstum einzelner Gemini-Kreaturen von Rang zu Rang.

| Tier (`animal_…`) | Biom (Rang) | LP | Haltung | Schaden | Verhalten (Asset-Liste) |
|---|---|---|---|---|---|
| Hase | Fjordwiesen, Heide (1) | 30 | 5 | – | flieht schnell |
| Fuchs | Fjordwiesen (1) | 50 | 8 | 4 | scheu |
| Rothirsch | Fjordwiesen, Heide (1) | 140 | 20 | 7 | flieht; stößt, wenn gestellt |
| Wolf | Fjordwiesen (1), Heide, Fjell | 140 | 18 | 8 | Rudel, umkreist zuerst |
| Wildschwein | Fjordwiesen (1) | 220 | 35 | 11 | greift an, wenn bedrängt |
| Robbe | Gezeitenmarsch (2) | 200 | 25 | 6 | flieht ins Wasser |
| Seevögel | Küste (2) | 15 | – | – | Vogelfelsen |
| Elch | Hügelgrab-Heide (3) | 650 | 80 | 20 | gefährlich, wenn bedrängt |
| Bär | Heide (3), Fjell | 800 | 95 | 24 | verteidigt seine Höhle |
| Luchs | Basaltfjell (4) | 600 | 60 | 22 | schleicht, greift Einzelne an |
| Rentier | Basaltfjell (4), Eisrand | 700 | 70 | 18 | Herden, scheu |
| Polarfuchs | Eisrand (6) | 300 | 20 | 12 | scheu |

Zum Vergleich die Mittelwerte der Kreaturen: Rang 2 = 379 / 40 / 12, Rang 3 = 607 / 63 / 18, Rang 4 = 910 / 93 / 24 (Dokument 22, Abschnitt 4).

### 8.2 Tiere: Beute (schlank, bestätigt; Teile = Vorschlag)

Jedes Säugetier gibt zusätzlich **Sehne und Knochen** in der Stufe seines Biom-Rangs (`item_sinew_t1…t6`, `item_bone_t1…t6`, dieselben Gegenstände wie bei den Kreaturen). Wildschwein, Rothirsch und Elch geben außerdem **Haut** (`item_hide_t…`).

| Tier | typische Teile | Fleisch |
|---|---|---|
| Wildschwein | Keilerhauer (`item_boar_tusk`) | ja |
| Rothirsch | Hirschgeweih (`item_deer_antler`) | ja |
| Hase | Hasenfell (`item_hare_pelt`) | ja |
| Fuchs | Fuchsfell (`item_fox_pelt`) | – |
| Wolf | Wolfsfell (`item_wolf_pelt`), Wolfszahn (`item_wolf_fang`) | – |
| Robbe | Robbenfell (`item_seal_pelt`), Tran (`item_seal_blubber`) | ja |
| Seevögel, Seeadler | Federn (`item_feather`), Seevogel-Ei (`item_seabird_egg`) | – |
| Elch | Elchschaufel (`item_elk_antler`) | ja |
| Bär | Bärenfell (`item_bear_pelt`), Bärenfett (`item_bear_fat`) | ja |
| Luchs | Luchsfell (`item_lynx_pelt`) | – |
| Rentier | Rentierfell (`item_reindeer_pelt`), Rentiergeweih (`item_reindeer_antler`) | ja |
| Polarfuchs | Polarfuchsfell (`item_arctic_fox_pelt`) | – |

**Fleisch** ist ein einziger Gegenstand, „Rohes Fleisch“ (`item_meat_raw`). Die Taverne-Gerichte, die noch auf Fleisch warten (Trockenfleisch, Wildeintopf, Braten), brauchen nur ihn.

### 8.3 Menschen (Vorschlag)

Banditen, Plünderer, Eidbrecher, Kultisten und Söldner (Gruppen aus der Asset-Liste) folgen **denselben Regeln wie Spieler**: 100 LP, Rüstung und Waffe ihrer Stufe, Haltung 100, Trefferzonen wie beim Spieler. Das passt zum KCD-Kampf: Ein Mensch fällt nach wenigen guten Treffern, wenn er schlecht gerüstet ist. Ihr Hauptmann bekommt eine Anführer-Stufe. Beute: beschädigte Ausrüstung ihrer Stufe und Vorräte. Details kommen mit dem Kampfsystem.

---

## 9. Bosse (Sagen-Fassung)

Story, Ablauf und Phasen der Bosse klärt die eigene Sitzung „Sagen-Bosse“. Hier stehen nur die Kampfwerte (**Vorschlag**): **Boss-Rang = Biom-Rang + 1** (höchstens 6). Die Werte kommen von einer passenden Kreatur dieses Rangs, dann **LP × 4, Haltung × 2, Schaden × 1,3**.

| Boss | Biom | Grundlage | LP | Haltung | Schaden |
|---|---|---|---|---|---|
| Der Alte Keiler (realistisch) | Fjordwiesen | Wildschwein auf Rang 2 | 1.230 | 90 | 18 |
| Schlickmutter als Moorhexe | Gezeitenmarsch | Mittelwert Rang 3 | 2.430 | 125 | 23 |
| Hügelkönig als Draugr-Fürst | Hügelgrab-Heide | Wiedergänger Rang 4 | 3.520 | 190 | 31 |
| Felsenvater als Troll | Basaltfjell | Troll Rang 5 | 6.240 | 330 | 44 |
| Aschezunge als Lindwurm | Glutküste | Cinderwyrm Rang 6 | 11.000 | 430 | 62 |
| Sturmkoloss als Frostriese | Eisrand | Riese Rang 6 + Anführer-Stufe | 13.230 | 530 | 75 |

Die Irrlichter der Moorhexe (`creature_will_o_wisp` aus der Asset-Liste) sind Begleiter mit Rang-2-Werten. Bossbeute bei Wiederholung folgt der bestätigten Regel vom 2026-11-15 (erster Sieg volle Beute, danach Bossmaterial mit Chance je Stufe).

---

## 10. Beute-Regeln

### 10.1 Würfeln (Gemini, bereinigt)

1. **Rang-Sperre (unverändert):** Ein Eintrag wird nur gewürfelt, wenn der Rang des Exemplars mindestens „ab Rang“ ist.
2. **Mengenbonus:** + 1 Stück je Rang über „ab Rang“, **höchstens + 2**. Bei Gemini war der Bonus unbegrenzt. Ein Rang-25-Captain hätte bei einem Rang-6-Eintrag 19 Stück extra bekommen.
3. **Nachzügler-Dämpfung nur für Biom-Zusatzbeute:** Liegt das Exemplar unter dem Rang des Ortes (Differenz 1 / 2 / ab 3), sinkt die Chance der Biom-Zusatzbeute auf × 0,85 / × 0,70 / × 0,55. Grund- und Rangbeute bleiben ungedämpft, denn ein Nachzügler gibt ohnehin nur die Beute seines niedrigen Rangs. Bei Gemini wurde doppelt bestraft. Geminis „Kerndrop-Schutz“ ist damit überflüssig.
4. **Anführer-Stufen** geben zusätzliche Würfe (Tabelle 6.4).
5. **Koop:** Gegner-Beute fällt einmal als Gegenstände auf den Boden (wie beim Grab). Bossbeute gibt es pro Spieler (bestätigt 2026-11-15).
6. **Schwierigkeit:** Der bestehende Faktor `yield` aus `difficulty.json` wirkt auf die Mengen.
7. **Der Server würfelt** (Regel 1), mit einem `FRandomStream` wie in Geminis Code.

### 10.2 Bereinigung von Geminis Beute-Namen (Vorschlag)

Geminis Tabellen enthalten **361 Einträge mit 279 verschiedenen Namen**. Nach diesen Regeln werden daraus **162 Gegenstände** (Zählung und alle Zuordnungen: Dokument 22):

1. **Gleicher Name = gleicher Gegenstand** (z. B. „Verfall-Relikt“ bei neun Kreaturen).
2. **Schlichte Synonyme werden zu allgemeinen Teilen je Stufe:** „Dünne / Zähe / Lange / Schwere Sehnen“ usw. werden zu `item_sinew_t<Rang>`, ebenso Knochen, Haut, Horn und Drachensehne. Die Stufe ist der Rang beim Tod. Kreaturentypische Teile bleiben eigene Gegenstände (Dornenbalg, Klammer-Chitin, Glutschuppe, Frostsehne …). Das ist die einzige Stelle, an der die feine Beute etwas zusammengelegt wird.
3. **„A & B“ wird geteilt:** zwei Einträge mit derselben Chance; A behält die Menge, B fällt einmal (15 Fälle).
4. **„A/B“ wird zum ersten Begriff** („Asche-Balg/Haut“ → Aschebalg).
5. **Sammelbegriffe werden konkrete Teile** der jeweiligen Kreatur, z. B. „Seltene Hochrang-Materialien“ (Dravik) → Dravik-Hornkern, „Spitzen-Komponenten“ (Cinderwyrm) → Glutdrachenherz, „Seltene Rohstoffe“ → Kammkern (Cragmaw) bzw. Riesenzahn (Riese), „Seltenere Relikte“ (Wiedergänger) → Grabbeigabe.
6. **Biom-Zusatzbeute** gilt nur in Biomen, in denen die Kreatur vorkommt. 33 Einträge entfallen, sie wären nie gefallen. Die Biom-Begriffe werden zu 18 Biom-Materialien, die man meist auch sammeln oder abbauen kann (Moosfaser, Verfall-Relikt, Basaltbrocken, Glutschuppe, Sturmfaser …; eines davon, Pflanzenfasern, gibt es schon). Zusammen mit dem Knorpel sind das die 19 Gegenstände mit fester Stufe.
7. **Keine Güte-Varianten (bestätigt 2026-11-22):** „Verstärkter Fangzahn“ neben „Fangzahn“ und ähnliche Paare entfallen (35 Gegenstände). Die Rang-Beute gibt stattdessen mit eigener Chance und Mengenbonus mehr vom Grundteil. Zwei Gegenstände mit „Uralt“ im Namen (Wurzelkern, Wyrmhorn) sind keine Varianten und bleiben.
8. **Rang-Beute über dem erreichbaren Rang rückt vor:** In unserer Biom-Ordnung erreichen manche Kreaturen Geminis Rang 5 oder 6 nie. Damit ihre seltene Beute nicht verloren geht, rückt sie auf den höchsten Rang, den die Kreatur erreicht, mit Geminis Chance (19 Fälle, in Dokument 22 als „vorgezogen“ markiert).

**Aufwand, ehrlich:** 162 Gegenstände heißt 162 Einträge in `items.json` und 162 Icons, dazu Tierteile und Metall. Ich hatte vorher „rund 150“ geschätzt, die echte Zahl bei Gemini war höher (279 Namen).

---

## 11. Herstellen: Metall plus Kreaturenteile

Bestätigt: Die Metall-Leiter bleibt das Rückgrat, Kreaturenteile kommen dazu. **Was die Teile bringen (Vorschlag):**

| Art (Katalog in Dokument 22) | wofür | Wirkung |
|---|---|---|
| Waffe/Werkzeug: Zähne, Klauen, Hörner, Knochen | Klingen, Spitzen, Köpfe | Schaden und Haltungsschaden zur Mitte bzw. zum oberen Ende der Stufe |
| Rüstung: Häute, Schuppen, Platten, Chitin | Rüstungsteile | Rüstung und Haltungswiderstand; Element-Teile geben Resistenz |
| Bindung: Sehnen, Fasern, Gewebe | Griffe, Bögen, Nähte | Haltbarkeit; bei Bögen Zugkraft |
| Veredelung: Kerne, Glut, Frost, Sporen | Veredelung, Tränke | Resistenzen, später Magie |
| Essen: Knorpel, Fett, Fleisch | Taverne | Brühen und Gerichte nach den Taverne-Regeln |
| Bau/Relikt: Platten, Relikte, Altmetall | Bauteile, Quests, Einschmelzen | Altes Eisenstück, Waffenbruchstück, Rüstungsschrott → Metall |

**Beispiele (Vorschlag; Geminis Rezepte an die Metall-Leiter angepasst):**

| Rezept | Stufe, ab Level | Zutaten | Ergebnis |
|---|---|---|---|
| Raseneisen-Schwert (Grundrezept) | T2, 9 | 2× Raseneisenbarren, 1× Zähe Sehne, 1× Holzgriff | 26 / 21 |
| Dornzahn-Klinge (Gemini) | T2, 12 | 2× Raseneisenbarren, 3× Fangzahn (Bramblefang), 2× Zähe Sehne, 1× Dornholz | ~31 / ~24 |
| Stahlschwert mit Felsklaue | T4, 30 | 3× Stahlbarren, 1× Felsklaue (Kraggar), 2× Schwere Sehne | ~74 / ~54 |
| Schlackekiefer-Brecher (Gemini, schwere Waffe) | T4, 29 | 3× Stahlbarren, 2× Gehärteter Kiefer (Charmaw), 1× Schlackeplatte, 2× Schwere Sehne | Grundwert ~71 / ~52, schwer × 1,3 / × 1,5 → ~92 / ~78 |
| Sturmwyrm-Speer (Gemini) | T6, 47 | 2× Reliktmetall, 2× Sturmwyrm-Horn, 2× Sturmwyrm-Schwingenhaut, 2× Massive Drachensehne, 1× Sturmwyrm-Kern | ~130 / ~94 |
| Dornenbalg-Wams (Oberkörper) | T2, 12 | 3× Dornenbalg, 2× Zähe Sehne, 1× Raseneisen-Nieten | Rüstung ~20, Haltungswiderstand ~18 (40 % des Sets) |
| Frost-Schuppenrüstung (Spezialset, Gemini) | T5, 36 | Oberkörper: 3× Frostschuppenbrocken (Frostmauler), 2× Damastringe, 2× Dichtes Hochlagenfell (Rimehound), 2× Gehärtete Sehne | Set ~165 / ~115, Frost-Resistenz + 36 % |
| Knorpelbrühe (Gemini: Knorpel- & Laufbrühe) | T2, Taverne | 2× Knorpel, 1× Knochen (T2) | Gesundheit + 12, Regeneration + 0,4; Geminis „+ 20 % Ausdauer-Erholung“ braucht eine neue Buff-Art `stamina_regen` |
| Sporen-Tonikum (Gemini) | T3 | 2× Sporenkapsel (Sporeback), 1× Verfall-Relikt, 1× Moosfaser | Verfall/Sporen-Resistenz + 15 % (`poison_resist`) |
| Glut- & Sturm-Elixier (Gemini) | T6 | 1× Stabile Glutkapsel, 1× Geladener Kernrest, 2× Glutknoten | Hitze und Sturm je + 20 % (neue Buff-Arten) |
| Dornen-Palisade (300 cm) | T2 | 4× Dornholz, 2× Dornenbalg, 2× Zähe Sehne | Verteidigungsbau (Weltoption Angriffe) |
| Gewölbe-Bollwerk mit Tor (300 cm, Gemini) | T4 | 4× Gewölbeplatte (Vaultcrawler), 3× Altes Eisenstück, 3× großer Stein, 2× Wurzelbast (Mossgolem) | schwere Mauer mit Tor |

Die Zahlen bei Waffen bedeuten Schaden / Haltungsschaden. Metall-Gegenstände (Erze, Barren, Nieten, Ringe) fehlen noch in `items.json`. Bauteile bekommen dieselbe Level-Anforderung wie Rezepte (Vorschlag).

---

## 12. Technik

**Neue Tabellen in `game/Content/Data/`** (Format wie `docs/06_datenformat.md`: `{ "schema": 1, "rows": [ … ] }`, IDs `snake_case`). Sie kommen mit Phase 3, zusammen mit dem Lader:

| Datei | Inhalt |
|---|---|
| `creatures.json` | ID, Name, MinRang, Werte je Rang, Trefferzonen, Welle |
| `animals.json` | Tiere wie in Abschnitt 8 (oder als Art in `creatures.json`) |
| `spawns.json` | je Biom: Kreatur, Gewicht, Rang-Verhalten, nur an Orten |
| `creature_loot.json` | je Kreatur: Grund-, Rang- und Biom-Beute mit Gegenstands-IDs |
| `progression.json` | Level-Kurve, Erfahrungsquellen, Stufen-Grenzen, Ausrüstungswerte je Stufe |
| `combat.json` | Konstanten (Rüstung K = 160 und Deckel 0,75, Resistenz −25 % bis 60 %, Haltung K = 90), Zonenfaktoren, Angriffsfaktoren, Streuung, Anführer-Stufen, `creature_hp_scale` |

Dazu kommen rund 160 neue Einträge in `items.json` für die Kreaturenbeute, die Tierteile aus Abschnitt 8.2 und Metall-Gegenstände. Rezepte bekommen das Feld `min_level`.

**Zu Geminis C++-Beispielen:**
- Die Formeln für Rüstung, Resistenz und Haltung (Abschnitt 9.2 dort) sind sauber und können fast unverändert in einen Namensraum wie `GamerCombatMath` übernommen werden.
- Den Beute-Würfler (9.3) muss man neu auf die JSON-Strukturen bauen. Die Typen, auf die er sich stützt, fehlen im Dokument: `CreatureMasterTypes.h`, `FLootEntry`, `FLootBiomeOverride`, `FCreatureLootRow`, `FAdjustedLootRoll`, `EvaluateFairDownscaledLoot`. So kompiliert er nicht.
- Gemini benutzt Unreal-DataTables. Unser Projekt liest JSON über `GamerDataLoader` (bestätigt 2026-11-13).
- Die Herstell-Bibliothek (9.4) wird nicht gebraucht: Herstellen gibt es schon (`recipes.json`), es fehlt nur `min_level`.
- **Prüfungen** wie bei `GamerDataTests`: Jeder Gegenstand in der Beute existiert, jede Kreatur spawnt irgendwo, kein Spawn unter dem MinRang, Heimat-Regel (nie vor der eigenen Welle), keine Drachensehne unter Rang 5.
- Spawn-Würfe, Streuung und Beute laufen auf dem Server und werden repliziert (Regel 1, gilt auch für Dedicated Server).

---

## 13. Was sich gegenüber Gemini ändert

| Gemini | bereinigt | Grund |
|---|---|---|
| 5 eigene Gebiete | 6 Biome, Gebiete zugeordnet | Entscheidung 2026-11-22 |
| Rang 1 (Level 1–5) ohne Kreaturen | Tiere und Menschen | Lücke |
| Captain-Ränge 7–25 | 3 sichtbare Anführer-Stufen mit denselben Höchstwerten | 19 kaum spürbare Ränge |
| Mengenbonus unbegrenzt | höchstens + 2 | Fehler: bei Rang 25 bis zu + 19 Stück |
| Nachzügler-Dämpfung auf alle Beute, dazu Kerndrop-Schutz | nur auf Biom-Zusatzbeute | doppelte Bestrafung |
| Rang-Beute oberhalb des erreichbaren Rangs | rückt auf den höchsten erreichbaren Rang | sonst nie erreichbar |
| Biom-Zusatzbeute in Biomen ohne die Kreatur | entfällt | tote Daten |
| 279 Beute-Namen | 162 Gegenstände | Doppelungen, Kombinationen, Sammelbegriffe, Güte-Varianten |
| englische Namen (Vaelgorn, Ironhide …) | deutsche Anzeigenamen | nordischer Stil, Marken |
| ein Rüstungswert | Rüstung pro Trefferzone aus den Slots | bestätigte Trefferzonen |
| Ausrüstung aus Kreaturenteilen | Metall-Leiter plus Kreaturenteile | Entscheidung 2026-11-22 |
| Streuung unsichtbar | ein Wurf, Größe folgt | Lesbarkeit |
| Haltung ohne Abbau | Abbau nach 3 s | sonst bricht jede Leiste irgendwann |
| keine Spieler-LP, keine Erfahrung | Abschnitte 3 und 4 | fehlte |
| „Ausdauer-Druck“, Ausdauer-Kosten | nur langsamer und schwächer | Regel 5 |
| 200-cm-Raster | 300 cm | so gebaut |
| Praxis-Beispiel 3.1: „86,25 Rohschaden“ | 50 × 1,15 × 1,4 = 80,5 | Rechenfehler bei Gemini (mit 1,5 statt 1,4 gerechnet) |
| JSON-Beispiele 6.5 mit CamelCase-IDs, teils andere Mengen als in den Tabellen | Tabellen gelten, IDs `snake_case` | Widerspruch im Dokument |
| „Delta == 0“ unter der Überschrift „Delta > 0“ | aufgelöst in 10.1 | Unklarheit |
| C++ mit DataTables und fehlenden Typen | JSON, Typen neu | Projektformat |

Unverändert übernommen: die drei Formeln (Rüstung, Resistenz, Haltung), die Archetyp-Faktoren, die Werte der 48 Kreaturen, die Spawn-Idee mit Mitwachsen und Nachzüglern, die Biom-Zusatzbeute als Idee und die Beute-Chancen.

---

## 14. Offen

- **Zahlen nach dem ersten Kampftest:** Kreaturen-LP (Regler `creature_hp_scale`), Anführer-Faktoren, Streuung, Erfahrungskurve.
- **Skilltree:** wofür die 59 Fähigkeitspunkte ausgegeben werden.
- **Metall-Gegenstände** (Erze, Barren, Nieten, Ringe) und ihre Rezepte fehlen in `items.json`.
- **Icons** für rund 160 neue Gegenstände: größter Einzelposten der Beute; Modelle und Icons entstehen gestaffelt mit den Wellen.
- **Umweltgefahren** Hitze und Sturm brauchen Durst und Wetter.
- **Bosse:** Phasen und Mechaniken mit der Story-Sitzung „Sagen-Bosse“ abstimmen.
- **Anzeigenamen:** die 48 Vorschläge aus Dokument 22 durchgehen und ändern, was nicht gefällt.
- **Nächster Schritt „Daten alle“:** die JSON-Tabellen aus Abschnitt 12 aus `tools/data/creatures_from_gemini.py` heraus erzeugen, sobald der Lader dafür steht.
