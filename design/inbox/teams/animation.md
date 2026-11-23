# Animation Team

[Teamablauf](README.md) · `ANIM`

## Auftrag

Bewegungs- und Arbeitsanimationen für Spieler und Dorfbewohner liefern. Vorrang haben Bewegung, Tragen, Holzfällen und Bauen im Siedlungs-Slice.

## Ablauf

1. Aktion und Abbruch mit SYS, Skelett und Kontaktpunkte mit TA klären.
2. Clip auf dem Zielskelett prüfen. Clip-ID, Start/Ende, Loop oder Einzelaktion, Root-Motion-Modus und Kontaktzeitpunkt vermerken.
3. Übergänge, Fußkontakt und Werkzeuggriff auf den betroffenen Figuren prüfen.

## Übergaben

- ENG erhält Clip-Pfad und Zuordnung zur Spielaktion, z. B. `EGamerAnimAction::Chop`. Claude Code integriert die Clips in `UGamerAnimInstance`.
- SFX erhält Kontaktzeitpunkt in Sekunden ab Clip-Start.
- QA erhält Prüfszene, Figur, Aktion und erwartete Darstellung für Host und Client.

## Abnahme

Sichtprüfung belegt saubere Schleifen, Übergänge und erwartete Wiedergabe bei Host und Client.

## Kontext

[Animationsinstanz](../../../game/Source/Gamer/GamerAnimInstance.cpp) · [MetaHuman-Anbindung](../../../docs/16_metahuman_anmeldung.md)
