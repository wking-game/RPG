# Sound Design Team

[Teamablauf](README.md) · `SFX`

## Auftrag

Gestalte Natur, Handwerk, Spielsignale und DE/EN-Vertonung. Setze Musik sparsam ein. Der Gezeitenhall klingt als tiefer, hallender Puls.

## Ablauf

1. Übernimm Zeilen-IDs von `NAR`, Bewegungsmarker von `ANIM` und Ereignisse von `SYS`/`ENG`.
2. Liefere pro Sound-ID Audiodatei, Quelle/Lizenz, Auslöser, Hörerkreis und Stop-Regel. Kläre das Speicherbudget mit `TA`.
3. Für Lauschen steuert `UGamerListenComponent::OnMagicHum` die Intensität. Vereinbare das Ausblenden außerhalb der Reichweite mit `ENG`.

## Übergaben

- An `ENG`: Ereigniszuordnung und Tondateien. An `UX`: Untertitel-IDs und Zeitmarken. An `ANIM`: Sprachdateien für MetaHuman-Lippenbewegungen.
- KI-Inhalte im [Asset-Log](../../asset_log.md) erfassen.

## Abnahme

`QA` erhält Hörproben und Sollsignale für Solo/Koop. Prüfe Sprachverständlichkeit, Schleifenübergänge, Ausblenden und doppelte Wiedergabe.

[Klangstil](../../stil_norden_gezeitenhall.md) · [Lauschen](../../../docs/13_magie_gespuer.md)
