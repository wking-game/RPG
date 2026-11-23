# Kapitel 1: Spieltext (Entwurf)

Stand 2026-11-21. **Spieltext von Claude** (KI-generiert, Eintrag in `design/asset_log.md`). Grundlage: `docs/17_hauptstory_kapitel.md` (Kapitel 1K und 1H, bestätigter Kern), Stil und Mittel aus der Prolog-Probe `docs/15_probe_spur_der_ersten.md`.
Alles wird vertont (DE/EN, zuerst KI-Stimmen). Sprecherangaben in eckigen Klammern entsprechen den `spk_`-IDs.
- **Gedanke:** Stimme der Spielerfigur (4 wählbare Stimmen).
- **Saga-Buch:** liest der Erzähler (`spk_narrator`).
- **Nebelbilder:** halb verwehte Satzfetzen mit Hall.

Gesprächsantworten mit ◇ sind Hilfe-Fragen. Sie erscheinen nur, wenn die Questhilfe im Menü an ist.

**Neue Namen (Vorschlag):**
- **Sæunn**: Seherin, eine von Hallbjörns Gefährten; Dorf-Seherin auf dem Weg des Kreises.
- **Orm** und **Kolla**: die beiden anderen Gefährten.
- **Tove**: Asgerds Tochter; Dorf-Seherin auf dem Weg des Hofs.
- **Arnfinn**: ein alter Bootsbauer bei Tove.

---

# 1K: Der Kreis im Norden (Weg des Kreises)

Voraussetzung: `choice:choice_empty_stone=ring`. Ziel-Gespür: Stufe 1–2.

## 1K.1 Der Ring wird warm

*Trägt man Hallbjörns Ring, wird das Summen im Norden mit der Zeit stärker, je näher man der Anhöhe kommt. Der Ring wirkt wie ein zweites, schwächeres Lauschen: Der Rand des Bildschirms schimmert nur in seine Richtung.*

**Hinweis** (einmalig): „Der Ring zieht. Folge ihm, wenn du willst.“
**Gedanke** (bei 200 m): „Es ist, als würde jemand in einem anderen Raum meinen Namen sagen.“

**Saga-Buch** (Aufgabe): *Hallbjörns Ring zog nach Norden, zu den Steinen auf der Anhöhe.*

## 1K.2 Das Lager am Hang

*Unterhalb der Anhöhe: ein Unterstand aus Fellen und Ästen, ein Feuer, das nur glimmt, Knochen, Fischgräten. Drei Menschen, mager, in geflickter Wolle. Als die Figur näher kommt, steht ein Mann auf (Orm) und hebt einen Speer, nicht ganz. Eine Frau (Sæunn) bleibt sitzen und sieht nur auf die Hand der Figur, auf den Ring.*

**[Orm]** „Bleib, wo du bist. Wir haben nichts mehr, das sich zu stehlen lohnt.“
**[Sæunn]** „Lass ihn, Orm. Sieh auf seine Hand.“
*Orm senkt den Speer. Kolla, eine junge Frau, wendet das Gesicht ab.*
**[Sæunn]** „Das ist Hallbjörns Ring. Er hat ihn niemandem gegeben. Also hast du ihn genommen. Vom Stein, nehme ich an. Vom leeren.“

**Antworten:**
- „Ich habe ihn unter dem Moos gefunden. Ich wusste nicht, wem er gehört.“ → **[Sæunn]** „Jetzt weißt du es. Und er weiß es vermutlich auch.“
- „Er lag dort für den, der ihn braucht.“ → **[Sæunn]** *lächelt kurz.* „So hätte er es auch gesagt.“
- ◇ „Was ist mit dem Kreis dort oben?“ → **[Orm]** „Er schweigt. Seit dem Frühjahr. Seit das Vieh an der Quelle wühlt. Geh nachts hin, dann siehst du, was ich meine.“ *(schaltet `area_help` frei: Keilersuhle wird auf der Karte eingegrenzt)*

**[Sæunn]** „Hallbjörn ist weitergegangen. Wir drei sind geblieben, weil der Kreis ihn hier noch gehört hat. Jetzt hört er nichts mehr. Und wir auch nicht.“
**[Kolla]** *(leise, ohne aufzusehen)* „Wir haben Hunger. Das ist alles, was wir noch hören.“

**Kleine Entscheidung (Nebenstrang-Gewicht):** Etwas zu essen geben (ein beliebiges Gericht aus dem Inventar) oder nicht.
- **Geben:** **[Kolla]** „…Danke.“ *Sie isst sofort, mit beiden Händen.* Kolla wird später zugänglicher, und ihr Gesprächszweig öffnet sich: Sie weiß, wo Hallbjörn Seeglas versteckte (Nebenfund).
- **Nicht geben:** kein Vorwurf. **[Orm]** „Schon gut. Hier oben teilt niemand mehr gern.“

## 1K.3 Die Suhle an der Quelle

*Oben auf der Anhöhe: sieben aufrechte Steine im Kreis, zwei umgestürzt, einer schief. In der Mitte eine Quelle, aufgewühlt zu Schlamm. Tiefe Spuren, Borsten am Stein. Bei Tag liegt der Ort still. Bei Nacht kommt der **Alte Keiler**: ein riesiges, narbiges Tier, das an den Steinen scheuert.*

**Gedanke** (beim ersten Blick auf die umgeworfenen Steine): „Er hat sie nicht umgestoßen. Er hat sich an ihnen gerieben, bis sie fielen.“
**Hinweis** (einmalig, wenn der Keiler auftaucht): „Ein Keiler dieser Größe flieht nicht. Mit dem Speer von vorn, mit dem Bogen aus der Höhe.“

**Bosskampf:** Der Alte Keiler, Stufe T2. Bis das Kampfsystem fertig ist (Phase 3), kämpft man mit dem einfachen Angriff (Speer, Bogen). Fällt man, liegt das Grab in der Suhle; Mitspieler können es nach 120 s bergen. Wölfe kommen nachts an den Rand des Kreises, angelockt vom Kampf.

**Saga-Buch** (nach dem Sieg, persönliche Tat): *{Spieler} erlegte den Alten Keiler an der Quelle des Kreises. Die Steine standen wieder still.*

## 1K.4 Die Steine aufrichten

*Die zwei umgestürzten Steine lassen sich mit Werkzeug aufrichten (Bauhammer, E halten, mit Arbeitsbalken). Im Koop geht es schneller, wenn mehrere halten. Danach klärt sich das Wasser der Quelle, und zum ersten Mal ist das Summen hier oben deutlich, gleichmäßig, wie ein Atem.*

**[Sæunn]** *(kommt den Hang herauf, bleibt am Rand des Kreises stehen)* „Du hast ihn wieder aufgeweckt.“ *Pause.* „Lausch. Aber nicht am Tag. Am Tag ist er zu laut für das, was er sagen will.“

**Antworten:**
- „Warum nicht am Tag?“ → **[Sæunn]** „Weil am Tag alles andere auch redet. Der Wind. Das Meer. Du.“
- ◇ „Wann genau soll ich lauschen?“ → **[Sæunn]** „Wenn die Sonne weg ist. Oder wenn der Nebel kommt. Dann bleib in der Mitte stehen.“ *(Hilfe: Bedingung `night` oder `fog` wird im Saga-Buch genannt)*

## 1K.5 Das Lied der Fjordwiesen (Nebelbild)

*Nachts oder im Nebel, aktives Lauschen in der Mitte: Steinkreis-Fund (+8 Gespür-EP beim ersten Mal). Der türkise Ring läuft langsamer als sonst und bricht an jedem Stein, als würde er zurückgeworfen. Dann das Nebelbild.*

**Nebelbild 4: Hallbjörn am Kreis**
*Eine einzelne Gestalt, ein großer Mann mit einem Beutel. Er legt Seeglas auf jeden Stein, einen Splitter nach dem anderen. Am letzten Stein zögert er.*
**[Hallbjörn, verweht]** „…nicht hier. Sie hören es hier… bis zum Meer…“
*Er nimmt den letzten Splitter wieder an sich. Er dreht sich nach Süden, zur Küste, und zeigt auf etwas, das man nicht sieht.*
**[Hallbjörn, verweht]** „…wo das Meer zweimal am Tag den Weg freigibt… dort hört mich niemand…“
*Er geht nach Süden. Der Nebel verweht.*

**Gedanke:** „Er hatte Angst, gehört zu werden. Nicht von den Steinen. Von Menschen.“

**Lied-Strophe** (ins Buch, `item_saga_fragment_fjord_1`, vom Kreis selbst „gesungen“, liest der Erzähler):
> *Sieben stehen am Wasser, das aus dem Berg kommt,*
> *sieben halten den Ton, den das Meer verlor.*
> *Wer ihn ruft, wird gehört, nah und fern.*
> *Wer ihn hält, wird nicht gefunden.*

**Saga-Buch** (Kapitelabschluss):
> *Am Kreis der Fjordwiesen sahen sie Hallbjörn im Nebel. Er hatte das Glas nicht gelegt, weil er fürchtete, wer es hören würde. Dann ging er nach Süden, in die Marsch, wo das Meer den Weg zweimal am Tag freigibt.*

**Offene Frage im Saga-Buch:** *„Vor wem hatte Hallbjörn Angst?“*

## 1K.6 Abschied am Lager

**[Sæunn]** „Du gehst ihm nach.“ *Keine Frage.* „Dann nimm das mit.“ *Sie gibt einen kleinen Lederbeutel mit drei Seeglas-Splittern (normales Seeglas).* „Er hat sie mir gegeben, bevor er ging. Ich habe sie nie gelegt.“

**Kleine Entscheidung (Nebenstrang-Gewicht):**
- **Annehmen:** Seeglas (+1 je Stück) und **[Sæunn]** „Leg sie nicht leichtfertig.“ Sæunn bleibt am Kreis.
- **Zurückgeben, „Behalte sie für den Kreis“:** **[Sæunn]** *(lange Pause)* „Dann bleibe ich bei ihm.“ Sæunn bleibt ebenfalls am Kreis. Später öffnet sich in „Die Stimme im Seeglas“ ein zusätzliches Gespräch: Sie vertraut dir früher und kommt eher ins Dorf.

**Übergang:** Startet den Nebenstrang „Die Stimme im Seeglas“ (Sæunn als künftige Dorf-Seherin) und Kapitel 2K.

---

# 1H: Rauch am Fjord (Weg des Hofs)

Voraussetzung: `choice:choice_empty_stone=name`. Ziel-Gespür: Stufe 1.

## 1H.1 Die Rauchsäule

*Die dünne Rauchsäule im Osten steht bei jedem Wetter, außer bei Sturm. Sie kommt aus einer Bucht, die man vom Strand aus nicht sieht. Der Weg führt am Fjordufer entlang, über Felsen, durch Birken.*

**Gedanke** (beim ersten Blick auf den Rauch vom Hügel): „Der war vorher nicht da. Oder ich habe nicht hingesehen.“
**Hinweis** (einmalig): „Rauch heißt Feuer. Feuer heißt Menschen.“

## 1H.2 Das versteckte Lager

*Eine schmale Bucht, von außen durch eine Felsnase verdeckt. Zwei Hütten aus Treibholz, ein Trockengestell mit wenig Fisch, ein Boot an Land, der Rumpf eingedrückt. Sieben Menschen. Ein alter Mann (Arnfinn) arbeitet an dem Boot, ohne aufzusehen. Eine Frau um die dreißig (Tove) steht auf, ein Beil in der Hand.*

**[Tove]** „Halt. Wer schickt dich?“

**Antworten:**
- „Niemand. Ich habe euren Rauch gesehen.“ → **[Tove]** „Dann haben ihn andere auch gesehen.“ *Sie sieht kurz zum Feuer, dann zurück.*
- „Ich komme vom Hügel. Von den Gräbern.“ → **[Tove]** *(das Beil sinkt ein Stück)* „…Von den Gräbern.“

**[Tove]** „Wir haben letzte Woche gesehen, dass jemand dort oben war. Das Moos war weg.“ *Pause.* „Und auf dem fünften Stein steht ein Name. Den hat dort niemand von uns hingeschrieben.“

**Antworten:**
- „Ich habe ihn geritzt. Hallbjörn.“ → **[Tove]** „Warum?“
  - „Weil der Stein auf ihn gewartet hat.“ → **[Tove]** *(lange Pause)* „Meine Mutter hat ihn gegraben. Asgerd. Sie hat gesagt, er sei schon tot, als er noch vor ihr stand.“ *Sie legt das Beil auf den Boden.* „Komm ans Feuer. Es ist nicht viel, aber es ist warm.“
  - „Weil ich wollte, dass es zu Ende ist.“ → **[Tove]** „…Für sie war es nie zu Ende.“ *Sie legt das Beil auf den Boden.* „Komm ans Feuer.“

**Gedanke** (am Feuer): „Sieben. Asgerd hat siebzehn geschrieben.“

**Saga-Buch:**
> *In einer verborgenen Bucht fanden sie Tove, Asgerds Tochter, und sechs, die mit ihr überlebt hatten. Asgerd selbst lebte nicht mehr.*

## 1H.3 Der Winter, der kommt

**[Tove]** „Wir sind hier, weil uns hier niemand sieht. Aber hier wächst nichts. Wir hatten eine Wiese oben am Bach, mit Flachs und Rüben. Ein Keiler hat sie umgegraben. Der Alte, sagen die Kinder. Er kommt nachts.“

**[Arnfinn]** *(vom Boot aus, ohne sich umzudrehen)* „Und das Boot fährt nicht mehr. Also bleiben wir. Also brauchen wir Essen. So einfach ist das.“

**Antworten:**
- „Was braucht ihr?“ → **[Tove]** „Vorräte für den Winter. Etwas, das sich hält. Und Wasser, das nicht nach Salz schmeckt.“
- ◇ „Wo ist die Wiese?“ → **[Tove]** „Den Bach hinauf, bis zu den zwei Birken, die zusammengewachsen sind. Er kommt, wenn es dunkel ist.“ *(Hilfe: Wiese auf der Karte eingegrenzt)*

**Aufgaben (Reihenfolge frei, keine Frist):**
- **Vorräte:** 10 haltbare Gerichte oder Lebensmittel bringen (z. B. Trockenfisch, Haselnüsse, gekochte Gerichte; Verderb zählt). Was man bringt, legt man in ihren Vorratskorb.
- **Wasser:** einen Regenfänger im Lager bauen (Rezept bekannt, bestätigt: 1 Baumstamm, 1 großer Stein, 3 kleine Steine, 5 Äste).
- **Die Wiese:** den Alten Keiler erlegen.

**Gedanke** (wenn ihr Winter kommt, bevor alles erledigt ist): „Sie werden es schaffen, irgendwie. Aber nicht gut.“ *(kein Scheitern, die Aufgaben bleiben offen)*

## 1H.4 Der Alte Keiler

*Die Wiese am Bach: umgewühlte Erde, ein zerbrochener Zaun, Rübenreste. Nachts kommt der Keiler, derselbe Boss wie im Weg des Kreises.*

**Bosskampf:** wie in 1K.3, aber auf offener Wiese. Mehr Platz, keine Steine als Deckung, dafür der Bach als Hindernis für den Keiler.

**[Tove]** *(nach dem Sieg, am nächsten Morgen in der Bucht)* „Du hast ihn erlegt. Allein?“
- Solo: „Allein.“ → **[Tove]** „Meine Mutter hätte dich gemocht. Sie mochte Leute, die zu dumm waren, um Angst zu haben.“
- Koop: „Nicht allein.“ → **[Tove]** „Dann seid ihr mehr, als ich dachte. Gut.“

## 1H.5 Asgerds Kamm

*Wenn Vorräte, Wasser und Wiese erledigt sind: Tove holt etwas aus der Hütte, ein Kamm aus Knochen, mit eingeritztem Zeichen (der offene Ring mit drei Kerben).*

**[Tove]** „Er gehörte meiner Mutter. Sie hat dieses Zeichen überall hingeritzt, auch wenn sie es gehasst hat. Sie sagte, man soll wissen, wovor man sich hütet.“ *Sie gibt ihn der Figur.* „Behalte ihn. Wer auf dem Hügel einen Namen schreibt, hat ein Andenken verdient.“

**Fund: Asgerds Kamm** (ins Buch, Erbstück)
> Knochen, abgegriffen, drei Zinken fehlen. In den Griff ist der offene Ring mit drei Kerben geritzt, so tief, dass er fast durchgeht.

**[Arnfinn]** *(zum ersten Mal mit der Figur)* „Und jetzt das Boot.“ *Er klopft auf den Rumpf.* „Nicht dieses. Das andere. Unser erstes. Die Plünderer haben es mitgenommen, als sie kamen. Darin war Asgerds Saatkiste: Gerste aus der Heimat, Flachs, alles. Das Boot liegt draußen in der Marsch, hab ich gehört. Auf einem Wrack. Bei einer, die sie die Schlickmutter nennen.“

**[Tove]** „Arnfinn.“
**[Arnfinn]** „Was? Sie hat gefragt, was wir brauchen. Das brauchen wir.“

## 1H.6 Die Frage des Ortes

**Kleine Entscheidung (Nebenstrang-Gewicht, bestätigtes Muster aus dem Kapitelplan):**
- **„Kommt mit in meine Siedlung.“** → **[Tove]** „…Nicht jetzt. Erst, wenn wir wissen, dass uns dort keiner findet. Wenn das Boot zurück ist, reden wir weiter.“ Sie ziehen nach Kapitel 3 als Bewohner ein.
- **„Ich helfe euch, hier zu bleiben.“** → **[Tove]** „Dann bleiben wir. Und du kommst, wann du willst. Wir haben Fisch und Leinen, mehr als wir brauchen.“ Die Bucht wird ein Handelsposten.

**Saga-Buch** (Kapitelabschluss):
> *Sie halfen Toves Leuten über den Winter und erlegten den Keiler auf ihrer Wiese. Tove gab ihnen den Kamm ihrer Mutter. Und Arnfinn sprach von einem Boot draußen in der Marsch, auf dem Wrack der Schlickmutter, mit Asgerds Saat darin.*

**Offene Frage im Saga-Buch:** *„Siebzehn kamen. Wer fehlt noch?“* (4 auf dem Hügel, 4 mit Hallbjörn, Asgerd tot, 7 bei Tove = 16; siehe Anmerkung)

**Übergang:** Startet „Die Stimme im Seeglas“ (Tove als künftige Dorf-Seherin) und Kapitel 2H.

---

## Anmerkungen für das Team

- **Rechenfehler im Prolog:** Die Kapitelzeile in `docs/15_probe_spur_der_ersten.md` sagt „Acht sind noch irgendwo“. Richtig ist **neun** (17 − 4 auf dem Hügel − 4 mit Hallbjörn). Korrektur dort: „Neun sind noch irgendwo“.
- **Zählung der Ersten (bestätigt 2026-11-21, der Siebzehnte ist der Lotse):** 17 gesamt = 4 auf dem Gräberhügel + Hallbjörn mit 3 Gefährten (Sæunn, Orm, Kolla) + Asgerd (tot, nach dem Prolog) + Tove mit 6 = 16. **Eine Person fehlt.** Vorschlag: Das ist der „Lotse“, der die Plünderer zur Bucht führte. Er taucht in Kapitel 2 bei der Schlickmutter auf (Verräter oder Gefangener, je nach Weg). Damit wäre auch die offene Frage im Saga-Buch eine echte Spur.
- **Gemeinsame Orte:** Beide Wege nutzen denselben Keiler und dieselbe Bossarena-Logik, aber andere Orte (Kreis bzw. Wiese). Gespräche und Belohnungen sind getrennt.
- **Koop:** Haben zwei Spieler verschiedene Wege, sieht jeder seine eigenen Gespräche. Das Lager bei Tove und das Lager am Kreis existieren beide in der Welt (Folgen addieren sich), sprechen aber nur mit dem, dessen Weg dazu passt; die anderen bekommen eine kurze, freundliche Abfuhr.
- **Länge:** etwa 90–150 Sprechzeilen je Weg und Sprache, Spielzeit 1–2 Stunden je Weg (mit Sammeln, Bauen, Jagd).
- **Keine Etappe braucht Horden;** mit Weltoption Horden kann der Rauch der Bucht nachts Plünderer-Kundschafter anlocken (nur Atmosphäre, kein Angriff auf die eigene Siedlung in Kapitel 1).
