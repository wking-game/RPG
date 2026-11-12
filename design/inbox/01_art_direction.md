# 01 – Art Direction

**Status:** Gewählt vom Nutzer am 2026-11-12: Klarform-Saga als Basis, im mittelalterlich-nordischen Wikinger-Kontext.

## Gemeinsamer Designrahmen

Das Spiel soll eine eigenständige Wikinger- und Mittelalterwelt mit Siedlungsaufbau, Erkundung und Koop-Kämpfen erhalten. Die Vorbilder dienen nur als Orientierung für Spielprinzipien. Figuren, Gebäude, Logos und konkrete Bildsprache werden eigenständig entwickelt.

Alle drei Richtungen priorisieren gut lesbare Silhouetten, wenige wiederverwendbare Materialien und Farbvarianten statt vieler Einzelassets. Wichtige Figuren und Gefahren müssen auch bei niedrigen Grafikeinstellungen sowie in der Vogelperspektive erkennbar bleiben. Die Siedlung soll belebt wirken, ohne dass jedes Haus und jeder Dorfbewohner ein eigenes hochauflösendes Asset benötigt.

---

## A. Nebelholz & Eisen

**ID:** `style_nebelholz_eisen`  
**Kernidee:** Eine raue, glaubwürdige Grenzland-Saga. Die Natur wirkt groß und wechselhaft; die Siedlung ist der warme, von Menschen geschaffene Gegenpol.

- **Formensprache:** robuste, funktionale Baukörper; sichtbare Balken, Verbindungen und Reparaturen. Gebäude dürfen sich durch Dachform und Anbauten unterscheiden, verwenden aber ein gemeinsames modulares Grundset.
- **Farbpalette:** Nebelblau `#9BAEB3`, dunkles Kieferngrün `#485B4D`, verwittertes Holz `#79664C`, Leinen `#D5C8AC`, Herdenglut `#D77B3E`.
- **Licht und Stimmung:** kühles, diffuses Tageslicht; warme, klar erkennbare Lichtinseln an Herd, Schmiede und Fenstern. Nebel und Regen bleiben sparsam, damit Dorf und Kampfgeschehen lesbar bleiben.
- **Detailgrad:** mittlere Geometriedichte. Oberflächenwirkung überwiegend über wenige wiederverwendete Materialfamilien und Farbparameter, nicht über einzigartige Texturen.
- **Begründung:** vermittelt Gewicht und Handwerk. Die warme Siedlung hebt sich deutlich von der offenen, kühlen Wildnis ab.
- **Aufwand:** mittel bis hoch. Glaubwürdige Oberflächen und stimmige Übergänge zwischen Holz, Stein und Metall benötigen sorgfältige Ausarbeitung.

**MVP – zuerst:** eine kleine Siedlungszone, ein Holz-Bauset aus wenigen Wand-, Dach- und Fundamentteilen, ein gemeinsamer Holz- und Steinmaterialpool, drei erkennbare Gebäudesilhouetten und zwei klar unterscheidbare Dorfbewohner-Silhouetten.

**Ausbaustufe:** weitere Bauzustände und Reparaturvarianten, regionale Holz- und Steinfarben, saisonale Stimmung sowie zusätzliche Schmuckteile für wichtige Gebäude und Bossorte.

---

## B. Klarform-Saga

**ID:** `style_klarform_saga`  
**Kernidee:** Eine farbkräftige, stilisierte Saga mit klaren Formen und sofort lesbaren Figuren. Die Welt wirkt rau, aber nicht düster; Farbe und Silhouette tragen die Identität.

- **Formensprache:** breite, ruhige Grundformen mit wenigen markanten Details. Dächer, Schilde, Werkzeuge und Kleidung erhalten erkennbare Umrisse. Muster und Runenzeichen werden als eigens entworfene grafische Akzente sparsam eingesetzt.
- **Farbpalette:** tiefes Fjordgrün `#3E6655`, Schieferblau `#607C8B`, helles Knochenweiß `#E5D8BC`, Ocker `#D2A64F`, Ziegelrot `#A95D48`.
- **Licht und Stimmung:** klare Tageslichtstimmung mit gut sichtbaren Farbflächen. Feuer, Questziele und Gefahrensignale bekommen gezielte Akzentfarben.
- **Detailgrad:** niedrig bis mittel. Unterschiede entstehen durch Form, Materialfarbe und kombinierbare Kleidungsteile statt durch viele Texturvarianten.
- **Begründung:** die wirtschaftlichste Richtung für viele Figuren und eine große Welt. Sie bleibt auch in der Dorf-Kommandoansicht und bei reduzierter Grafik lesbar.
- **Aufwand:** niedrig bis mittel. Ein sauber definiertes Form- und Farbsystem kann auf Bauten, Ausrüstung, Figuren und UI übertragen werden.

**MVP – zuerst:** ein kleines, durchgängiges Bauset; vier Berufs-Silhouetten, die sich über Werkzeug und ein bis zwei Kleidungsteile unterscheiden; eine begrenzte Materialpalette; einfache Licht- und Farbregeln für Dorf, Wildnis und Kampf.

**Ausbaustufe:** zusätzliche regionale Akzentfarben, seltene Ausrüstungsvarianten, saisonale Farbwechsel und stärkere visuelle Kennzeichnung von Fraktionen, Bossen und Weltoptionen.

---

## C. Holzschnitt der Grenzlande

**ID:** `style_holzschnitt_grenzlande`  
**Kernidee:** Eine grafische Saga, die an geschnitzte Holztafeln und handgedruckte Karten erinnert. Sie ist bewusst illustrativ und unterscheidet sich damit von einer naturalistischen Mittelalterdarstellung.

- **Formensprache:** kantige, gestaffelte Formen; Gebäude und Landschaften erhalten klare Außenkonturen durch Silhouette und Licht-Schatten-Flächen. Grafische Kerben und Ornamentbänder markieren wichtige Objekte, ohne jedes Modell zu verzieren.
- **Farbpalette:** Tintenbraun `#292B27`, dunkles Seegrün `#35666A`, Pergament `#D7C8A5`, Rostrot `#A64F3B`, gedämpftes Gold `#C79B4A`.
- **Licht und Stimmung:** gerichtetes, gut kontrolliertes Licht mit deutlichen Schatten. Innenräume und wichtige Interaktionsobjekte erhalten helle, warme Flächen. Dunkelheit soll Stimmung erzeugen, nicht die Orientierung erschweren.
- **Detailgrad:** niedrig bis mittel. Holzmaserung, Stoff und Stein werden über gemeinsame stilisierte Materialien angedeutet; feine Linien bleiben besonderen Objekten vorbehalten.
- **Begründung:** bietet die auffälligste eigene Bildsprache und kann auch auf Karten, Questzeichen und Menüs angewendet werden.
- **Aufwand:** mittel bis hoch. Die grafische Wirkung muss in Materialien, Licht, Figuren, Landschaft und UI zusammenpassen; ein eigener Shader ist möglich, aber nicht Voraussetzung für das MVP.

**MVP – zuerst:** ein begrenztes Testareal, ein reduziertes Holz-Bauset, ein gemeinsamer Satz grafischer Materialien, gut sichtbare Figurenkonturen und eine kleine Testoberfläche für die Vogelperspektive.

**Ausbaustufe:** eigens gestaltete Karten- und Questgrafik, Muster für Biome und Fraktionen, besondere Boss-Silhouetten und zusätzliche Lichtstimmungen für Dämmerung und Sturm.

---

## Vergleich

| Richtung | Aufwand | Stärke | Wichtigstes Risiko |
|---|---|---|---|
| `style_nebelholz_eisen` | Mittel bis hoch | Erdige Stimmung und starkes Handwerksgefühl | Zu viele Oberflächendetails erhöhen Aufwand und Speicherbedarf |
| `style_klarform_saga` | Niedrig bis mittel | Lesbarkeit, Wiederverwendung und gute Skalierung auf viele Figuren | Kann ohne markante Silhouetten zu generisch wirken |
| `style_holzschnitt_grenzlande` | Mittel bis hoch | Eigenständige, über Spiel und UI tragbare Bildsprache | Grafische Konventionen müssen in allen Ansichten konsistent bleiben |

## Festgelegte Richtung

Klarform-Saga wird als stilisierte, nordisch-frühmittelalterliche Bildsprache umgesetzt. Natürliche Abnutzung aus Richtung A dient als zurückhaltender Materialakzent. Die Formen bleiben klar und gut lesbar; eigens entworfene Architekturdetails geben der Siedlung ihre Identität.

## Entscheidungen, die der Nutzer treffen muss

Keine offene Richtungswahl. Der Nutzer hat Klarform-Saga für ein mittelalterlich-nordisches Wikingerspiel gewählt. Der nächste Reviewpunkt sind die Phase-1-Konzepte für Holzbauteile und Arbeitsstätten.
## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `01_art_direction.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext; keine externen Assets verwendet |



