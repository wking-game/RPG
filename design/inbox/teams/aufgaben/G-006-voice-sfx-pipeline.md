# G-006 · Vertonungs- und SFX-Pipeline

- Zuständig: SFX
- Beteiligte: NAR, TA, ANIM, UX, ENG, QA, PROD
- Stand: blockiert
- Freigabe durch: PROD und Klio
- Blockiert durch: NAR liefert drei freigegebene Zeilen-IDs; PROD und Klio bestätigen Testwerkzeug und Audio-Testbudget
- Soll: DE/EN-Vertonung und Soundeffekte verwenden stabile IDs, wiederholbare Exporte, Untertitel, Lautheitsvorgaben und belegte Nutzungsrechte.
- Kontext: [Gemini-Auswertung](../gemini-auswertung_2026-11-19.md) · [Vertonungsentscheidungen](../../../../docs/01_entscheidungen.md)
- Ergebnis-Pfad: `design/inbox/voice_sfx_pipeline.md`
- Abnahme: Je eine Erzähler-, NPC- und Spielerzeile liegt in DE/EN mit gleicher ID, Untertitelzeit und dokumentierten Ziel-/Messwerten für Lautheit vor. Rechte, Kosten, Dateigröße, Reproduzierbarkeit und MetaHuman-Gesichtstest sind belegt. QA hört jede Zeile bei Standardlautstärke in ruhiger Szene sowie mit Wind und Feuer ab; der gesprochene Wortlaut muss ohne Untertitel vollständig protokollierbar sein, ohne Übersteuerung. Untertitel bleiben immer verfügbar.
- Ergebnis/Nachweis: Vergleichstest, Dateischema, Sprecher-/Zustandsmatrix, Lizenznachweis und Asset-Log-Eintrag.
- Nächster Schritt: Erst je eine Erzähler-, NPC- und Spielerzeile testen. ElevenLabs und andere Werkzeuge gelten bis zur aktuellen Lizenz- und Qualitätsprüfung als Kandidaten.
