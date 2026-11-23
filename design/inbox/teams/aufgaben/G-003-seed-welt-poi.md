# G-003 · Seed-Welt und Quest-POIs spezifizieren

- Zuständig: SYS
- Beteiligte: PROD, NAR, ENG, UX, QA
- Stand: blockiert
- Freigabe durch: PROD und Klio
- Blockiert durch: PROD setzt die Weltphase aktiv; Klio bestätigt den G-003-Umfang in der Aufgabendatei
- Soll: Eine deterministische Welt aus Terrain, Biomen, Ressourcen und Story-POIs ist aus einem Seed reproduzierbar. Änderungen an einem Generator verschieben unabhängige Systeme nicht unkontrolliert.
- Kontext: [Gemini-Auswertung](../gemini-auswertung_2026-11-19.md) · [Phasenplan](../../../../docs/02_phasenplan.md) · [Saga-Datenentwurf](../../../../docs/14_saga_quests.md)
- Ergebnis-Pfad: `design/inbox/world_seed_poi_spec.md`
- Abnahme: Derselbe Seed erzeugt im selben Build mit identischer Konfiguration dieselben IDs und Transformationen für den Startfjord und einen Story-POI. Speichern/Laden erhält Welt- und Questzustand. Host und Client sehen denselben Stand. Generatorversion und Migration sind beschrieben.
- Ergebnis/Nachweis: Datenvertrag, Beispiel-Seed und reproduzierbarer Prüfbericht.
- Nächster Schritt: Für Phase 4 zunächst Startfjord plus einen festen Story-POI reproduzierbar spezifizieren. ENG setzt erst nach Freigabe um.
