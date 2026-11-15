# KI-3D-Dienste: Lizenz- und Eignungsvergleich (Stand 2026-11-13)

Grundlage: Entscheidung „Herkunft 3D-Assets“ vom 2026-11-12 (`docs/01_entscheidungen.md`): organische Einzelobjekte aus einem KI-3D-Werkzeug mit automatischer Nachbearbeitung in Blender. Offen war nur, welcher Dienst. Dieses Blatt ist Claudes Recherche und Empfehlung, keine Entscheidung.

**Ehrlicher Hinweis:** Die offiziellen Nutzungsbedingungen (Terms of Service) der Dienste habe ich nicht selbst gelesen. Die Angaben stammen aus Hilfeseiten und Preisseiten der Anbieter, Lizenztexten auf GitHub/Hugging Face und Testberichten, die sich teils widersprechen. Vor dem Abschluss eines Abos die aktuellen Bedingungen auf der Seite des Anbieters prüfen.

## Bedingungen für dieses Projekt
- Verkauf auf Steam: Nutzung muss kommerziell erlaubt sein; KI-Inhalte werden im Steam-Formular angegeben und in `design/asset_log.md` eingetragen.
- Standort EU (Deutschland).
- Entwicklungs-PC: RTX 3070 mit 8 GB Grafikspeicher, Windows.
- Budget klein; Konto und Bezahlung macht der Nutzer selbst (Claude legt keine Konten an und zahlt nichts).

## Vergleich

| Dienst | Kommerziell | Kosten | Läuft wo | Eignung hier |
|---|---|---|---|---|
| **Meshy** | Bezahlte Stufen: private Modelle, kommerziell ohne Namensnennung. Kostenlose Stufe: CC BY 4.0 (kommerziell mit Namensnennung, Modelle öffentlich). Modelle aus der Abo-Zeit behalten die Rechte nach einer Kündigung (laut Hilfeseite) | Pro ca. 20 $/Monat (1.000 Credits, verfallen monatlich), Programmierschnittstelle (API) ab Pro | Cloud | **gut**: ausgereift, API für eine automatische Pipeline, Polygonzahl einstellbar |
| **Tripo** | Bezahlte Stufen: privat, kommerziell. Kostenlose Stufe: öffentlich, CC BY 4.0; ob kommerziell erlaubt, ist widersprüchlich beschrieben | ab ca. 12–20 $/Monat | Cloud | gut, ähnlich Meshy; kostenlose Stufe rechtlich unklar |
| **Hyper3D Rodin** | kommerziell erst in bezahlten Stufen, welche genau, ist widersprüchlich (Creator ca. 30 $ oder Business ca. 120 $/Monat) | höher | Cloud | möglich, aber teurer und unklarer |
| **Stable Fast 3D / SPAR3D** (Stability AI) | Community License: kostenlos, auch kommerziell, bis 1 Mio. $ Jahresumsatz | kostenlos | lokal, ca. 7 GB Grafikspeicher (Angaben Dritter) | möglich: keine Abokosten, aber schwächere Qualität und mehr Einrichtung; Modellzugang auf Hugging Face nur nach Zustimmung zur Lizenz |
| **Hunyuan3D** (Tencent) | Lizenz gilt **nicht in der EU**, im Vereinigten Königreich und Südkorea | – | – | **scheidet aus** (Standort EU) |
| **TRELLIS.2** (Microsoft) | MIT-Lizenz, frei | kostenlos | lokal nur mit mind. 24 GB Grafikspeicher, nur Linux getestet | **scheidet lokal aus** (8 GB); nur mit gemieteter Cloud-GPU |

## Empfehlung
1. **Erst kostenlos testen:** 2–3 Vorkommen (z. B. Kiefer, Felsen, Beerenstrauch) in der kostenlosen Meshy-Stufe über die Weboberfläche erzeugen, als GLB/FBX in `Assets/Source/ki3d/` ablegen. Claude baut die Blender-Nachbearbeitung (Maßstab, Drehpunkt, Polygonzahl, Material, Zustände voll/abgeerntet/Rest) und den Import. Danach im Spiel beurteilen. Diese Testmodelle sind CC BY 4.0 und öffentlich: für den Test in Ordnung, für das fertige Spiel nur mit Namensnennung.
2. **Wenn die Qualität passt: Meshy Pro für einen Monat** (ca. 20 $) für alle Slice-Pflanzen und Felsen, über die API automatisiert. Der Schlüssel liegt dann in einer Umgebungsvariable auf dem PC des Nutzers, nie im Repository.
3. Wenn keine Abokosten gewünscht sind: Stable Fast 3D lokal, mit geringerer Qualität.

## Stand: kein Bezahldienst (Nutzer, 2026-11-13)
Zuerst gewählt: Meshy Pro („Gleich Meshy Pro“). Noch am selben Tag geändert: Vegetation kostenlos selbst erzeugen („da brauche ich nicht extra irgendwie Geld ausgeben“). Es wurde nichts bei Meshy erzeugt und kein Credit verbraucht. Das Abrufskript bleibt als Option liegen; die Blender-Nachbearbeitung und der Import taugen für jedes GLB (auch aus freien Generatoren).

### Kostenlose Wege (Recherche 2026-11-13)
| Weg | Lizenz | Vorteile | Nachteile |
|---|---|---|---|
| **Eigener Generator in Blender (Python)**, im vorhandenen Asset-Skript | eigenes Werk | volle Kontrolle über den Stil (Klarform-Saga: breite, klare Formen), Wachstumsstufen per Parameter, Varianten per Zufallszahl, ohne Editor-Klicks wiederholbar, kein Nanite nötig, klein im Speicher | stilisiert statt fotorealistisch; Qualität hängt an der Ausarbeitung, braucht Rückmeldung per Bild |
| **Procedural Vegetation Editor (PVE)** in Unreal 5.8 | Teil der Engine | wächst Bäume „botanisch“, Wind, Nanite Foliage | experimentell (5.7-Dateien gehen in 5.8 nicht mehr), keine dokumentierte Skript-Schnittstelle (nur Editor-Oberfläche), Ausgabe auf Nanite Foliage ausgelegt (Export als normales Modell laut Forum teils fehlerhaft), Gras nicht dokumentiert, Wachstum nur im Editor (nicht im Spiel), eher realistisch; fertige Vorlagen von Drittanbietern kosten Geld |
| **EZ-Tree** (Three.js, Dan Greenheck) | MIT | 15 Baumvorlagen, Export als GLB mit LODs, als npm-Paket per Skript nutzbar | eher realistisch, Nacharbeit nötig, kein Gras |
| **tree-gen** (friggog), **Modular Tree** (Blender) | GPL-3.0 (gilt für das Werkzeug, nicht für erzeugte Modelle) | erprobte Verzweigungs-Algorithmen in Blender | eher realistisch, Einarbeitung |

Quellen: [Epic: Procedural Vegetation Editor](https://dev.epicgames.com/documentation/unreal-engine/procedural-vegetation-editor-in-unreal-engine), [Unreal Engine 5.8 Neuerungen](https://www.unrealengine.com/news/unreal-engine-5-8-is-now-available), [Forum: PVE exportiert kein Non-Nanite](https://forums.unrealengine.com/t/procedural-vegetation-editor-cannot-export-non-nanite-foliage/2718593), [Forum: statische vs. animierte PVE-Bäume](https://forums.unrealengine.com/t/please-export-pp2-texture-maps-for-the-procedural-vegetation-static-mesh/2662240), [Skyroid: PVE in der Praxis](https://www.skyroidstudios.com/insights/procedural-vegetation-editor-unreal-environment-teams), [EZ-Tree auf GitHub](https://github.com/dgreenheck/ez-tree), [tree-gen auf GitHub](https://github.com/friggog/tree-gen), [Modular Tree](https://extensions.blender.org/add-ons/modular-tree/).

### Empfehlung
Eigener Generator in Blender für alle Slice-Pflanzen, Felsen und Grasbüschel, angelehnt an bekannte Verfahren (rekursive Verzweigung nach Weber und Penn, wie bei tree-gen). Dazu im Spiel: sichtbares Nachwachsen (Stumpf → Setzling → junger Baum → ausgewachsen) und Gras als Büschel, die auf dem Boden verteilt werden (eigener Code, kein Plugin nötig). PVE später für einzelne Bäume ausprobieren, wenn es nicht mehr experimentell ist.

## Gewählt (zurückgenommen): Meshy Pro (Nutzer, 2026-11-13: „Gleich Meshy Pro“)

## Pipeline (`tools/ki3d.ps1`)
1. **Abrufen** (`tools/ki3d/meshy_generate.py`): liest `tools/ki3d/flora.json` (Beschreibungstexte, Zielgröße, Dreiecksbudget, Zustände). Pro Modell ein Vorschau-Auftrag (Form, 20 Credits) und pro Zustand ein Textur-Auftrag (10 Credits) aus derselben Form, so bleiben z. B. „voll“ und „abgeerntet“ beim Beerenstrauch exakt gleich geformt. Lädt GLB und Vorschaubild sofort herunter (Meshy löscht API-Aufträge nach 3 Tagen) nach `D:\gamer\ki3d_rohdaten\meshy\` (außerhalb des Repositorys, weil `.glb` nicht über LFS läuft) und protokolliert jeden Auftrag mit Text und Credits in `meshy_log.json` (Grundlage für `design/asset_log.md` und die Steam-Angabe). Schlüssel nur aus der Benutzervariable `MESHY_API_KEY`, wird nie ausgegeben. `-DryRun` zeigt Plan und Kosten, `-MaxCredits` setzt eine Obergrenze, fertige Modelle werden übersprungen.
2. **Nachbearbeiten** (`tools/blender/process_ki3d.py`): Teile zusammenfügen, auf Zielgröße skalieren, Drehpunkt an den Fuß (Mitte der untersten 4 %, bei Bäumen der Stamm), Dreiecke begrenzen, Grundfarb-Textur verkleinern, ein Materialplatz `M_Nature_<Name>`; FBX und PNG nach `Assets/Nature/`, eigenes Manifest `Assets/manifest_ki3d.json` (das Baukasten-Skript überschreibt `Assets/manifest.json`). Normal- und ORM-Karte sind für alle Pflanzen gleich und flach (matt), passend zur stilisierten Art Direction und sparsam im Speicher.
3. **Import** (`tools/import_assets.ps1`): liest beide Manifeste, legt je Modell eine Materialinstanz von `M_Gamer_Base` an, Kollision = Modell.
4. **Spiel:** `resource_nodes.json` bekommt `meshes` (voll, abgeerntet). Ein Vorkommen mit Modellen zeigt diese statt der Platzhalter, mit zufälliger Drehung und ±15 % Größe aus seiner Position (auf allen Rechnern gleich). Ohne Modell für „abgeerntet“ verschwindet es beim Ernten.

Plan für den Slice: 16 Formen, 19 Texturen, geschätzt 510 Credits.

## Was für den Slice gebraucht wird (`design/ASSET_LIST_GESAMT.md`, Abschnitt 8)
Kiefer und Birke (stehend, gefällt, Stumpf), Felsen klein/mittel/groß (voll, abgebaut), Beerenstrauch (voll/leer), Wildapfelbaum (voll/abgeerntet), Pilzstelle, Haselstrauch, lose Steine, herabgefallene Äste, Faserpflanze (Brennnessel). Etwa 10 Vorkommen mit zusammen rund 20 Modellen. Budget laut `design/ASSET_PLAN.md`: Baum bis 15.000 Dreiecke.

## Quellen
- Meshy: [Pricing & Credits](https://docs.meshy.ai/en/webapp/pricing), [Ownership](https://help.meshy.ai/en/articles/10137554-what-is-the-ownership-of-the-generated-models), [Commercial use](https://help.meshy.ai/en/articles/16102098-can-i-use-meshy-assets-commercially), [Preise](https://meshy.ai/pricing)
- Tripo: [Image to 3D (Lizenzhinweis)](https://www.tripo3d.ai/features/image-to-3d-model), [Blog: kommerzielle Nutzung](https://www.tripo3d.ai/blog/commercial-use-ai-3d-models), [Test MakerStack](https://makerstack.co/reviews/tripo-ai-review/)
- Hyper3D Rodin: [Test Affinco](https://affinco.com/hyper3d-review/), [GitHub-Issue mit Zitat der Bedingungen](https://github.com/DarkLordGeo/age-of-wars/issues/2)
- Stability AI: [Stable Fast 3D](https://stability.ai/news/introducing-stable-fast-3d), [SPAR3D](https://stability.ai/news/stable-point-aware-3d), [Lizenzen](https://stability.ai/license)
- Hunyuan3D: [Lizenz 2.0](https://github.com/Tencent-Hunyuan/Hunyuan3D-2/blob/main/LICENSE), [Lizenz 2.1](https://github.com/Tencent-Hunyuan/Hunyuan3D-2.1/blob/main/LICENSE)
- TRELLIS.2: [Modellkarte Hugging Face](https://huggingface.co/microsoft/TRELLIS.2-4B/blob/main/README.md)
