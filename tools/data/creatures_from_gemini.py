"""Erzeugt docs/22_kreaturen_daten.md aus Geminis „Küstenasche“-Dokument.

Aufruf aus dem Projektordner:
    python -I tools/data/creatures_from_gemini.py

Liest:    design/referenz/gemini_kuestenasche_gesamtdokumentation.md (Original, nicht ändern)
Schreibt: docs/22_kreaturen_daten.md (wird jedes Mal ganz neu geschrieben)

Regeln und Begründungen stehen in docs/21_kreaturen_level_kampf.md. Die Tabellen in diesem
Skript (Spawn, Beute-Zuordnung, Gegenstände) sind Vorschläge von Claude (2026-11-22).
Sobald es creatures.json, spawns.json und creature_loot.json gibt, sind diese Dateien die
Quelle; dann wird das Skript nicht mehr gebraucht.
"""
from __future__ import annotations

import datetime
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "design" / "referenz" / "gemini_kuestenasche_gesamtdokumentation.md"
TARGET = ROOT / "docs" / "22_kreaturen_daten.md"

# ---------------------------------------------------------------------------
# Biome: key -> (ID, Name, Rang = Stufe, gefährliche Orte)
# ---------------------------------------------------------------------------
BIOMES = {
    "fjord_meadow": ("biome_fjord_meadow", "Fjordwiesen", 1, "Banditenlager, Wolfshöhlen"),
    "tidal_marsh": ("biome_tidal_marsh", "Gezeitenmarsch", 2, "Wracks, tiefe Priele"),
    "barrow_heath": ("biome_barrow_heath", "Hügelgrab-Heide", 3, "Grabhügel, Gewölbe, Grabfestung"),
    "basalt_fell": ("biome_basalt_fell", "Basaltfjell", 4, "Gipfel, Eishöhlen"),
    "ember_coast": ("biome_ember_coast", "Glutküste", 5, "Lavafelder, Kultstätten"),
    "ice_rim": ("biome_ice_rim", "Eisrand", 6, "Gletscherspalten, gefrorene Wracks"),
}
BIOME_ORDER = list(BIOMES)
WAVE_OF_BIOME = {"tidal_marsh": 1, "barrow_heath": 2, "basalt_fell": 3, "ember_coast": 4, "ice_rim": 5}

# Geminis Gebiete -> unsere Biome (Entscheidung 2026-11-22)
GEMINI_BIOMES = {
    "Moosgeäst": ["fjord_meadow", "tidal_marsh"],
    "Bruchstein": ["barrow_heath"],
    "Glut-Fluren": ["ember_coast"],
    "Schroffen-Grate": ["basalt_fell"],
    "Böen-Kämme": ["ice_rim"],
}

BIOME_NOTES = {
    "fjord_meadow": "Keine Kreaturen (bestätigt). Tiere: Wildschwein, Rothirsch, Hase, Fuchs, Wolf. "
                    "Menschen: Waldbanditen. Boss: Der Alte Keiler (realistisch).",
    "tidal_marsh": "Dazu Robbe und Seevögel (friedlich), Strandplünderer. "
                   "Boss: Die Schlickmutter als Moorhexe mit Irrlichtern (`creature_will_o_wisp`).",
    "barrow_heath": "Dazu Rothirsch, Hase, Wolf, Bär, Elch; Waldbanditen, Eidbrecher. "
                    "Boss: Der Hügelkönig als Draugr-Fürst (die Wiedergänger sind sein Gefolge).",
    "basalt_fell": "Dazu Wolf, Bär, Rentier, Luchs; Eidbrecher. "
                   "Boss: Der Felsenvater als Troll (der größte und älteste Troll).",
    "ember_coast": "Keine Tiere; Aschebund-Kult. Boss: Die Aschezunge als Lindwurm.",
    "ice_rim": "Dazu Rentier, Polarfuchs; Menschen laut Story. "
               "Boss: Der Sturmkoloss als Frostriese. Anführer-Stufen hier am häufigsten.",
}

# ---------------------------------------------------------------------------
# Kreaturen: Nr -> (ID, Name, Rolle). Rollen für Nr. 1–30 aus der Beute abgeleitet,
# weil Gemini diese Kreaturen nicht beschreibt.
# ---------------------------------------------------------------------------
CREATURES = {
    1: ("creature_vaelgorn", "Vaelgorn", "Krustenwesen mit Mineralplatten"),
    2: ("creature_cinderloom", "Cinderloom", "glimmender Faserweber mit Glutknoten"),
    3: ("creature_osskarr", "Osskarr", "Knochenwesen mit Gelenkpanzer und Stofffetzen"),
    4: ("creature_morrowear", "Morrowear", "Lauscher mit großem Schwingungsorgan"),
    5: ("creature_pyroclast", "Pyroclast", "grabender Schlacke-Panzerträger"),
    6: ("creature_dravik", "Dravik", "schwer gepanzerter Gräber mit Schuppenplatten"),
    7: ("creature_murghul", "Murghul", "Chitin-Jäger mit Sichelklauen"),
    8: ("creature_kraggar", "Kraggar", "Fels-Raubtier mit Granithaut"),
    9: ("creature_galeborn", "Galeborn", "Sturmwesen aus Faserbändern mit geladenen Kernen"),
    10: ("creature_ashstalker", "Ashstalker", "Asche-Pirscher mit leichtem Balg"),
    11: ("creature_thornmaw", "Thornmaw", "Dornenmaul mit verholzter Haut"),
    12: ("creature_dustmote", "Dustmote", "kleiner, flinker Staubflatterer (Schwarm)"),
    13: ("creature_mirecrawler", "Mirecrawler", "Schlick-Kriecher mit breiten Greifpratzen"),
    14: ("creature_cinderhound", "Cinderhound", "Glut-Jagdhund im Rudel"),
    15: ("creature_graveclaw", "Graveclaw", "Grabkrallen-Wühler mit Schutt-Schultern"),
    16: ("creature_rimestalker", "Rimestalker", "kaltgrauer Frost-Pirscher mit Hornspitzen"),
    17: ("creature_hollowback", "Hollowback", "Hohlrücken, stößt Staub- und Sporenwolken aus"),
    18: ("creature_ironhide", "Ironhide", "schwerer Eisenkrusten-Panzer"),
    19: ("creature_shroudwalker", "Shroudwalker", "Schattenweber der Gewölbe mit glimmendem Kern"),
    20: ("creature_cragmaw", "Cragmaw", "Felskiefer-Räuber mit Steinkamm"),
    21: ("creature_dreadcolossus", "Dreadcolossus", "uralter Koloss aus Stein und Relikten, sehr selten"),
    22: ("creature_troll", "Troll", "Troll mit dicker Haut"),
    23: ("creature_giant", "Riese", "Riese mit Rüstungsresten"),
    24: ("creature_revenant", "Wiedergänger", "Untoter aus den Grabhügeln"),
    25: ("creature_cinderwyrm", "Cinderwyrm", "Glut-Drache"),
    26: ("creature_cragdrake", "Cragdrake", "Fels-Drache der Gipfel"),
    27: ("creature_stormmaw", "Stormmaw", "großer Sturm-Drache"),
    28: ("creature_ashwing", "Ashwing", "Asche-Gleitdrache"),
    29: ("creature_frostdrake", "Frostdrake", "Frost-Drache"),
    30: ("creature_dreadwyrm", "Dreadwyrm", "Spitzen-Drache, sehr selten"),
    31: ("creature_bramblefang", "Bramblefang", "Dornen-Raubtier, schnelle Schnappangriffe, leicht aus dem Tritt zu bringen"),
    32: ("creature_siltstrider", "Siltstrider", "langbeiniger Senkenläufer, stolpert bei Flankentreffern"),
    33: ("creature_cinderling", "Cinderling", "kleiner Funkenläufer, flankiert in Gruppen"),
    34: ("creature_sporeback", "Sporeback", "buckliger Schleicher, platzt bei Haltungsbruch zur Sporenwolke"),
    35: ("creature_skitterclaw", "Skitterclaw", "vielbeiniger Mauer- und Felskrabbler"),
    36: ("creature_gloomstalker", "Gloomstalker", "Schattenpirscher, lauert in dunklen Ecken"),
    37: ("creature_charmaw", "Charmaw", "Glut-Beißer, beißt sich fest"),
    38: ("creature_rimehound", "Rimehound", "Frost-Rudeljäger, bremst die Ausdauer-Erholung"),
    39: ("creature_slagbrute", "Slagbrute", "Schlacke-Schläger mit dicker Frontkruste"),
    40: ("creature_vaultcrawler", "Vaultcrawler", "gepanzerter Gewölbekletterer mit Greifarmen"),
    41: ("creature_mossgolem", "Mossgolem", "Wächter aus Moos, Wurzeln und altem Gestein"),
    42: ("creature_galehound", "Galehound", "Sturm-Läufer, lädt vor Stößen hörbar auf"),
    43: ("creature_cragstalker", "Cragstalker", "Grat-Jäger mit weiten Sprüngen"),
    44: ("creature_ashweaver", "Ashweaver", "Asche-Weber mit Netzen und glimmenden Schwachpunkten"),
    45: ("creature_frostmauler", "Frostmauler", "massiger Frost-Brecher mit Bodenstößen"),
    46: ("creature_stormwyrmling", "Stormwyrmling", "junger Sturm-Drache, Vorbote der Sturm-Spitzen"),
    47: ("creature_cindercolossus", "Cindercolossus", "glühender Schlacke-Koloss, Schwachstellen in den Gelenken"),
    48: ("creature_galewyrm", "Galewyrm", "Spitzen-Sturm-Drache mit Schwingenböen"),
}

# ---------------------------------------------------------------------------
# Spawn: Biom -> Liste (Nr, Gewicht, Rangverhalten, nur an gefährlichen Orten)
# Rangverhalten: ("mit", Anteil) | ("bis", Rang, Anteil) | ("fest", Rang)
# Werte aus Gemini übernommen, wo Gemini sie für die Kreatur angibt (Nr. 31–48),
# sonst Vorschlag von Claude.
# ---------------------------------------------------------------------------
M85 = ("mit", 0.85)
SPAWNS = {
    "fjord_meadow": [],
    "tidal_marsh": [
        (31, 1.25, ("mit", 1.0), False),
        (11, 1.00, M85, False),
        (13, 1.10, M85, False),
        (32, 0.90, M85, False),
    ],
    "barrow_heath": [
        (3, 1.00, M85, False),
        (4, 0.80, M85, False),
        (15, 1.00, M85, False),
        (24, 1.10, M85, False),
        (12, 1.00, M85, False),
        (34, 1.05, M85, False),
        (35, 1.15, M85, False),
        (36, 0.95, ("mit", 0.80), False),
        (37, 0.70, ("bis", 4, 0.60), False),
        (31, 0.60, ("fest", 2), False),
        (32, 0.50, ("fest", 2), False),
        (11, 0.50, ("fest", 2), False),
        (17, 0.80, M85, True),
        (19, 0.80, M85, True),
        (40, 0.90, M85, True),
        (41, 0.60, ("mit", 0.70), True),
    ],
    "basalt_fell": [
        (1, 0.90, M85, False),
        (7, 0.90, M85, False),
        (8, 1.00, M85, False),
        (16, 0.90, M85, False),
        (18, 0.80, M85, False),
        (22, 0.50, M85, False),
        (38, 1.05, M85, False),
        (39, 0.70, ("bis", 5, 0.65), False),
        (41, 0.85, M85, False),
        (3, 0.60, M85, False),
        (4, 0.50, M85, False),
        (35, 0.70, ("fest", 3), False),
        (20, 0.80, M85, True),
        (26, 0.30, M85, True),
        (43, 0.85, M85, True),
        (23, 0.35, M85, True),
        (45, 0.75, M85, True),
        (46, 0.45, ("mit", 0.80), True),
    ],
    "ember_coast": [
        (2, 0.90, M85, False),
        (5, 0.90, M85, False),
        (6, 0.80, M85, False),
        (10, 1.10, M85, False),
        (14, 1.00, M85, False),
        (37, 1.00, M85, False),
        (39, 1.05, M85, False),
        (44, 0.75, M85, False),
        (25, 0.35, M85, False),
        (28, 0.40, M85, False),
        (1, 0.60, M85, False),
        (18, 0.60, M85, False),
        (17, 0.50, M85, False),
        (33, 1.30, ("bis", 3, 0.85), False),
        (32, 0.60, ("fest", 2), False),
        (12, 0.60, ("fest", 2), False),
        (47, 0.35, ("mit", 1.0), True),
    ],
    "ice_rim": [
        (9, 0.90, M85, False),
        (42, 1.05, ("mit", 0.90), False),
        (43, 0.65, ("mit", 0.80), False),
        (45, 0.60, ("mit", 0.80), False),
        (46, 0.55, M85, False),
        (29, 0.35, M85, False),
        (27, 0.20, ("mit", 1.0), False),
        (48, 0.35, ("mit", 1.0), False),
        (21, 0.15, ("mit", 1.0), False),
        (30, 0.10, ("mit", 1.0), False),
        (23, 0.50, M85, False),
        (8, 0.50, M85, False),
        (16, 0.60, M85, False),
        (20, 0.40, M85, False),
        (26, 0.25, M85, False),
        (38, 0.75, ("fest", 4), False),
    ],
}

# ---------------------------------------------------------------------------
# Gegenstände
# ---------------------------------------------------------------------------
KINDS = {"R": "Rüstung", "W": "Waffe/Werkzeug", "B": "Bindung", "V": "Veredelung",
         "E": "Essen", "X": "Bau/Relikt"}

# Allgemeine Teile, deren Stufe der Rang der Kreatur beim Tod ist (auch für Tiere)
GENERIC = {
    "sinew": ("item_sinew_t{}", ["Dünne Sehne", "Zähe Sehne", "Kräftige Sehne", "Schwere Sehne",
                                 "Gehärtete Sehne", "Uralte Sehne"], "B", "Sehne"),
    "bone": ("item_bone_t{}", ["Leichte Knochen", "Knochen", "Kräftige Knochen", "Schwere Knochen",
                               "Gehärtete Knochen", "Uralte Knochen"], "W", "Knochen"),
    "hide": ("item_hide_t{}", ["Dünne Haut", "Zähe Haut", "Dicke Haut", "Schwere Haut",
                               "Gehärtete Haut", "Uralte Haut"], "R", "Haut"),
    "horn": ("item_horn_t{}", ["Hornsplitter", "Horn", "Kräftiges Horn", "Schweres Horn",
                               "Gehärtetes Horn", "Uraltes Horn"], "W", "Horn"),
    "dragon_sinew": ("item_dragon_sinew_t{}", [None, None, None, None, "Drachensehne",
                                               "Massive Drachensehne"], "B", "Drachensehne"),
}

# Gegenstände mit fester Stufe: Biom-Materialien (auch sammelbar) und Knorpel
FIXED = {
    "item_fiber": ("Pflanzenfasern", "B", 1, "gibt es schon in `items.json`"),
    "item_cartilage": ("Knorpel", "E", 1, "Zutat für Brühen"),
    "item_moss_fiber": ("Moosfaser", "B", 2, "Gezeitenmarsch, auch sammelbar"),
    "item_thorn_wood": ("Dornholz", "X", 2, "Gezeitenmarsch, auch sammelbar"),
    "item_damp_leather": ("Feuchtleder", "R", 2, "Gezeitenmarsch"),
    "item_decay_relic": ("Verfall-Relikt", "X", 3, "Hügelgrab-Heide, auch in Ruinen"),
    "item_ruin_chitin": ("Ruinen-Chitin", "R", 3, "Hügelgrab-Heide"),
    "item_old_iron": ("Altes Eisenstück", "X", 3, "Hügelgrab-Heide, einschmelzbar"),
    "item_ruin_dust": ("Ruinenstaub", "X", 3, "Hügelgrab-Heide, für Mörtel und Alchemie"),
    "item_iron_ore": ("Eisenerz", "X", 3, "Metall-Leiter, auch abbaubar"),
    "item_basalt_chunk": ("Basaltbrocken", "X", 4, "Basaltfjell, auch abbaubar"),
    "item_granite_scale": ("Granitschuppe", "R", 4, "Basaltfjell, auch sammelbar"),
    "item_frost_shard": ("Frostsplitter", "V", 4, "Basaltfjell und Eisrand"),
    "item_ash_mineral": ("Asche-Mineral", "V", 5, "Glutküste, auch abbaubar"),
    "item_slag_core": ("Schlackekern", "V", 5, "Glutküste"),
    "item_ember_scale": ("Glutschuppe", "R", 5, "Glutküste"),
    "item_storm_fiber": ("Sturmfaser", "B", 6, "Eisrand"),
    "item_storm_wing_hide": ("Sturm-Schwingenhaut", "R", 6, "Eisrand"),
    "item_charged_core": ("Geladener Kernrest", "V", 6, "Eisrand"),
}

# Entscheidung 2026-11-22: keine reinen Güte-Varianten. Die Rang-Beute gibt stattdessen
# (mit eigener Chance und Mengenbonus) mehr vom Grundteil.
VARIANT_OF = {
    "item_bramblefang_fang_strong": "item_bramblefang_fang",
    "item_bramblefang_pelt_hard": "item_bramblefang_pelt",
    "item_dustmote_feeler_dense": "item_dustmote_feeler",
    "item_mirecrawler_paw_strong": "item_mirecrawler_paw",
    "item_siltstrider_hide_tough": "item_siltstrider_hide",
    "item_thornmaw_thorn_thick": "item_thornmaw_thorn",
    "item_charmaw_jaw_hard": "item_charmaw_jaw",
    "item_cinderhound_pelt_tough": "item_cinderhound_pelt",
    "item_digging_claw_heavy": "item_digging_claw",
    "item_graveclaw_shoulder_hard": "item_graveclaw_shoulder",
    "item_morrowear_ear_fine": "item_morrowear_ear",
    "item_murghul_sickle_claw_hard": "item_murghul_sickle_claw",
    "item_murghul_chitin_thick": "item_murghul_chitin",
    "item_shadow_weave_dense": "item_shadow_weave",
    "item_skitterclaw_chitin_hard": "item_skitterclaw_chitin",
    "item_skitterclaw_chitin_plate": "item_skitterclaw_chitin",
    "item_sporeback_hide_hard": "item_sporeback_hide",
    "item_vaelgorn_crust_plate": "item_vaelgorn_crust_shard",
    "item_vaultcrawler_plate_strong": "item_vaultcrawler_plate",
    "item_hollowback_shell_hard": "item_hollowback_shell",
    "item_ironhide_plate_massive": "item_ironhide_plate",
    "item_kraggar_rock_claw_massive": "item_kraggar_rock_claw",
    "item_rimehound_fur_dense": "item_rimehound_fur",
    "item_rimestalker_hide_dense": "item_rimestalker_hide",
    "item_slag_plate_dense": "item_slag_plate",
    "item_troll_hide_thick": "item_troll_hide",
    "item_giant_bone_massive": "item_giant_bone",
    "item_cragdrake_scale_heavy": "item_cragdrake_scale",
    "item_cragstalker_hide_hard": "item_cragstalker_hide",
    "item_dravik_scale_heavy": "item_dravik_scale_plate",
    "item_frostdrake_scale_hard": "item_frostdrake_scale",
    "item_frostmauler_armor": "item_frostmauler_scale",
    "item_ember_scale_fine": "item_ember_scale",
    "item_heat_fiber_fine": "item_heat_fiber",
    "item_storm_fiber_dense": "item_storm_fiber",
}

# Anzeigenamen (Vorschlag von Claude, 2026-11-22; die IDs bleiben englisch und stabil)
DISPLAY = {
    1: "Krustenwandler", 2: "Glutweber", 3: "Beinrassler", 4: "Lauscher", 5: "Schlackengräber",
    6: "Panzergräber", 7: "Sichelkrabbler", 8: "Felsreißer", 9: "Sturmgeborener", 10: "Aschepirscher",
    11: "Dornenmaul", 12: "Staubmotte", 13: "Schlickkriecher", 14: "Glutrüde", 15: "Grabkralle",
    16: "Reifpirscher", 17: "Hohlrücken", 18: "Eisenhaut", 19: "Schleierwandler", 20: "Felsrachen",
    21: "Schreckenskoloss", 22: "Troll", 23: "Riese", 24: "Draugr", 25: "Glutwurm",
    26: "Felsdrache", 27: "Sturmrachen", 28: "Aschenschwinge", 29: "Frostdrache", 30: "Schreckenswurm",
    31: "Dornenbeißer", 32: "Schlickschreiter", 33: "Funkling", 34: "Sporenbuckel", 35: "Mauerkrabbler",
    36: "Dämmerpirscher", 37: "Kohlenmaul", 38: "Reifhund", 39: "Schlackenbrecher", 40: "Gewölbekletterer",
    41: "Mooswächter", 42: "Sturmhund", 43: "Gratjäger", 44: "Aschespinner", 45: "Frostbrecher",
    46: "Sturmwürmling", 47: "Aschekoloss", 48: "Sturmwurm",
}

ITEMS: dict[str, tuple[str, str]] = {}  # benannte Gegenstände, Stufe wird berechnet


def I(item_id: str, name: str | None = None, kind: str | None = None):
    """Benannter Gegenstand. Beim ersten Vorkommen mit Name und Art angeben."""
    if name is not None:
        old = ITEMS.get(item_id)
        if old and old != (name, kind):
            raise ValueError(f"{item_id}: widersprüchliche Angaben {old} / {(name, kind)}")
        ITEMS[item_id] = (name, kind)
    return ("item", item_id)


def F(item_id: str):
    """Gegenstand mit fester Stufe (Biom-Material, Knorpel)."""
    if item_id not in FIXED:
        raise KeyError(item_id)
    return ("item", item_id)


def G(family: str):
    """Allgemeiner Teil, Stufe = Rang beim Tod."""
    return ("gen", family)


def S(first, second):
    """Geminis „A & B“: zwei Einträge; A behält die Menge, B fällt einmal."""
    return ("split", (first, second))


# Geminis Name -> Gegenstand. Gilt für alle Kreaturen, außer es gibt eine Ausnahme unten.
LOOT = {
    # allgemeine Teile (Synonyme zusammengelegt)
    "Sehnenfasern": G("sinew"), "Verstärkte Sehnenstränge": G("sinew"),
    "Trockene Sehnenbänder": G("sinew"), "Lange Sehnen": G("sinew"), "Dünne Sehnen": G("sinew"),
    "Stärkere Sehnen": G("sinew"), "Sehnen": G("sinew"), "Asche-Sehne": G("sinew"),
    "Dicke Sehnen": G("sinew"), "Zähe Sehnen": G("sinew"), "Zähe Dravik-Sehnen": G("sinew"),
    "Gedämpfte Sehnen": G("sinew"), "Dünne Gliedsehnen": G("sinew"), "Schwere Sehnen": G("sinew"),
    "Knochensplitter": G("bone"), "Leichte Knochenreste": G("bone"), "Knochenreste": G("bone"),
    "Grobe Knochen": G("bone"), "Schwere Knochen": G("bone"), "Knochen-/Sehnenmaterial": G("bone"),
    "Schwere Vorderlauf-Reste": G("bone"), "Fels-Kieferrest": G("bone"),
    "Dunkle Knochensplitter": G("bone"),
    "Dicke stumpfe Haut": G("hide"), "Zähere Häute": G("hide"), "Dickere Häute": G("hide"),
    "Moos-/Erd-Balg": G("hide"), "Ruß-Balgrest": G("hide"),
    "Hornstacheln": G("horn"), "Fels-Stachel": G("horn"), "Hornkamm-Splitter": G("horn"),
    "Kanten-Horn": G("horn"), "Gesteins-Horn": G("horn"), "Schwere Hornreste": G("horn"),
    "Staubige Horn-/Knochenreste": G("horn"), "Kalte Horn-/Knochenreste": G("horn"),
    "Staub-Klaue": G("horn"), "Asche-Horn": G("horn"), "Hitze-Horn": G("horn"), "Kalt-Horn": G("horn"),
    "Zähe Drachensehnen": G("dragon_sinew"), "Dicker Drachenknorpel/Sehnen": G("dragon_sinew"),
    "Drachensehnen": G("dragon_sinew"), "Elastische Drachensehnen": G("dragon_sinew"),
    "Massive Drachensehnen": G("dragon_sinew"), "Massive Sehnen": G("dragon_sinew"),
    "Knorpelreste": F("item_cartilage"), "Kiefer-/Knorpelreste": F("item_cartilage"),
    "Gebirgs-Knorpel": F("item_cartilage"),
    # Biom-Materialien
    "Faser-/Harzreste": F("item_fiber"), "Waldfaser": F("item_fiber"), "Unterholz-Reste": F("item_fiber"),
    "Unterholz-Faser": F("item_fiber"),
    "Moosfaser": F("item_moss_fiber"), "Dornen-Holz": F("item_thorn_wood"),
    "Feucht-Leder": F("item_damp_leather"), "Feucht-Leder/Schlammrest": F("item_damp_leather"),
    "Feuchtes Balg-/Lederstück": F("item_damp_leather"),
    "Verfall-Relikt": F("item_decay_relic"), "Verfall-Reste": F("item_decay_relic"),
    "Alte Stoff-/Reliktfäden": F("item_decay_relic"), "Relikt-Faser": F("item_decay_relic"),
    "Asche-Relikt": F("item_decay_relic"),
    "Ruinen-Chitin": F("item_ruin_chitin"),
    "Altes Eisenstück": F("item_old_iron"), "Alte Ausrüstungs-/Metallreste": F("item_old_iron"),
    "Altes Metall-/Reliktsück": F("item_old_iron"),
    "Staub-/Asche-Korn": F("item_ruin_dust"), "Ruinen-Gestein": F("item_ruin_dust"),
    "Trümmer-Staub": F("item_ruin_dust"), "Feiner Staub-/Asche-Rest": F("item_ruin_dust"),
    "Umgebungs-Kapsel/Rest": F("item_ruin_dust"), "Ruinen-Staubrest": F("item_ruin_dust"),
    "Ruinen-Rußrest": F("item_ruin_dust"), "Kalk-/Mauerstaub-Kapsel": F("item_ruin_dust"),
    "Geröll-Erzrest": F("item_iron_ore"), "Grab-Erz/Schuttrest": F("item_iron_ore"),
    "Erz-/Eisen-Kruste": F("item_iron_ore"),
    "Gesteinssplitter": F("item_basalt_chunk"), "Felsstaub-/Splitterreste": F("item_basalt_chunk"),
    "Panzer-Schutt": F("item_basalt_chunk"), "Hochgebirgs-Stein": F("item_basalt_chunk"),
    "Grobe Schlacke-/Steinreste": F("item_basalt_chunk"), "Uralter Fels-/Mineralrest": F("item_basalt_chunk"),
    "Grobe Gesteinsbrocken": F("item_basalt_chunk"),
    "Granit-Schuppe": F("item_granite_scale"), "Frost-Splitter": F("item_frost_shard"),
    "Asche-Mineral": F("item_ash_mineral"), "Ruinen-/Asche-Reste": F("item_ash_mineral"),
    "Schlacke-Kern": F("item_slag_core"), "Glut-Schlacke": F("item_slag_core"),
    "Glut-Schuppe": F("item_ember_scale"), "Glut-Schuppen": F("item_ember_scale"),
    "Sturm-Faser": F("item_storm_fiber"), "Sturm-Faserbänder": F("item_storm_fiber"),
    "Sturm-Schwingenhaut": F("item_storm_wing_hide"),
    "Verdichtete Sturm-Schwingenhaut": F("item_storm_wing_hide"),
    "Geladener Kernrest": F("item_charged_core"), "Geladene Kernreste": F("item_charged_core"),
    "Flackernde Funken-/Spannungsreste": F("item_charged_core"),
    # gemeinsam genutzte benannte Teile
    "Verkohlte Faser": I("item_charred_fiber", "Verkohlte Faser", "B"),
    "Verkohlte Faserstränge": I("item_charred_fiber"), "Asche-Faser": I("item_charred_fiber"),
    "Verkohltes Holz-/Faserstück": I("item_charred_fiber"),
    "Funken-Asche": I("item_spark_ash", "Funkenasche", "V"), "Glut-Funke": I("item_spark_ash"),
    "Glimmende Funken-/Glut-Reste": I("item_spark_ash"),
    "Kleine Glutknoten": I("item_ember_knot", "Glutknoten", "V"), "Glimmende Glutknoten": I("item_ember_knot"),
    "Zähe Hitze-Fasern": I("item_heat_fiber", "Hitzefaser", "B"), "Zähe Asche-Faserstränge": I("item_heat_fiber"),
    "Hochfester Glut-Faserstrang": I("item_heat_fiber_fine", "Hochfester Glut-Faserstrang", "B"),
    "Glimmende Glutkapsel": I("item_ember_capsule", "Glutkapsel", "V"),
    "Stabile Glutkapseln": I("item_ember_capsule_stable", "Stabile Glutkapsel", "V"),
    "Seltene Glutkapsel": I("item_ember_capsule_stable"),
    "Hitzebeständige Kerne/Brocken": I("item_heat_core", "Hitzekern", "V"),
    "Glühender Schlacke-Kern": I("item_heat_core"),
    "Schlacke-Panzerstücke": I("item_slag_plate", "Schlackeplatte", "R"),
    "Schlacke-Frontkruste": I("item_slag_plate"),
    "Dichte Schlackeplatten": I("item_slag_plate_dense", "Dichte Schlackeplatte", "R"),
    "Schlacke-Platten": I("item_slag_plate_dense"),
    "Grabklauen": I("item_digging_claw", "Grabklaue", "W"),
    "Schwere Grabklauen": I("item_digging_claw_heavy", "Schwere Grabklaue", "W"),
    "Grobe Gewebe-/Stofffetzen": I("item_cloth_scrap", "Stofffetzen", "B"),
    "Modriges Gewebe-/Relikt": I("item_cloth_scrap"),
    "Schatten-Gewebefetzen": I("item_shadow_weave", "Schattengewebe", "R"),
    "Dunkle Faser-/Gewebebahnen": I("item_shadow_weave"),
    "Dichtere Schatten-/Faserbahnen": I("item_shadow_weave_dense", "Dichtes Schattengewebe", "R"),
    "Frost-Sehnen": I("item_frost_sinew", "Frostsehne", "B"),
    # 1 Vaelgorn
    "Mineral-Krustensplitter": I("item_vaelgorn_crust_shard", "Mineral-Krustensplitter", "R"),
    "Dichtere Krustenplatten": I("item_vaelgorn_crust_plate", "Dichte Krustenplatte", "R"),
    # 3 Osskarr
    "Knochengelenke": I("item_osskarr_bone_joint", "Knochengelenk", "W"),
    "Gehärtete Knochenstreben": I("item_osskarr_bone_strut", "Knochenstrebe", "W"),
    # 4 Morrowear
    "Hör-/Schwingungsorgan": I("item_morrowear_ear", "Schwingungsorgan", "V"),
    "Ausgeprägtere Schwingungsorgane": I("item_morrowear_ear_fine", "Feines Schwingungsorgan", "V"),
    # 5 Pyroclast
    "Glühendes Gestein": I("item_pyroclast_glowstone", "Glühgestein", "V"),
    # 6 Dravik
    "Dicke Horn-/Schuppenplatten": I("item_dravik_scale_plate", "Dravik-Schuppenplatte", "R"),
    "Schwere Spitzen-Schuppen": I("item_dravik_scale_heavy", "Schwere Dravik-Schuppe", "R"),
    "Seltene Hochrang-Materialien": I("item_dravik_horn_core", "Dravik-Hornkern", "V"),
    # 7 Murghul
    "Sichel-Greifklauen": I("item_murghul_sickle_claw", "Sichelklaue", "W"),
    "Chitin-Plattenstücke": I("item_murghul_chitin", "Chitin-Plattenstück", "R"),
    "Gehärtete Sichelklauen": I("item_murghul_sickle_claw_hard", "Gehärtete Sichelklaue", "W"),
    "Dickere Chitinpanzer": I("item_murghul_chitin_thick", "Dicker Chitinpanzer", "R"),
    # 8 Kraggar
    "Fels-Klauen": I("item_kraggar_rock_claw", "Felsklaue", "W"),
    "Granit-/Schiefer-Hautstücke": I("item_kraggar_granite_hide", "Granithaut", "R"),
    "Massive Felsklauen": I("item_kraggar_rock_claw_massive", "Massive Felsklaue", "W"),
    "Gebirgs-/Schmiede-Reste": I("item_kraggar_ore_vein", "Erzader-Brocken", "X"),
    # 9 Galeborn
    "Hornspangen": I("item_galeborn_horn_clasp", "Hornspange", "W"),
    "Ruinen-Spange": I("item_galeborn_horn_clasp"),
    "Dichte Sturm-Fasern": I("item_storm_fiber_dense", "Dichte Sturmfaser", "B"),
    # 10 Ashstalker
    "Asche-Balg/Haut": I("item_ashstalker_pelt", "Aschebalg", "R"),
    # 11 Thornmaw
    "Dornen-/Horn-Splitter": I("item_thornmaw_thorn", "Dornsplitter", "W"),
    "Verholzte Hautstücke": I("item_thornmaw_bark_hide", "Verholzte Haut", "R"),
    "Dickere Dorn-/Horn-Teile": I("item_thornmaw_thorn_thick", "Dicker Dorn", "W"),
    # 12 Dustmote
    "Staubige Hautlappen": I("item_dustmote_skin", "Staubhautlappen", "R"),
    "Feine Fühler-/Faserreste": I("item_dustmote_feeler", "Fühlerfaser", "B"),
    "Dichtere Faser-/Staub-Teile": I("item_dustmote_feeler_dense", "Dichte Fühlerfaser", "B"),
    # 13 Mirecrawler
    "Zähe Feuchthaut": I("item_mirecrawler_hide", "Feuchthaut", "R"),
    "Breite Greifpratzen": I("item_mirecrawler_paw", "Greifpratze", "W"),
    "Kräftigere Pratzen": I("item_mirecrawler_paw_strong", "Kräftige Greifpratze", "W"),
    # 14 Cinderhound
    "Verkohlte Faser-/Fellreste": I("item_cinderhound_pelt", "Brandfell", "R"),
    "Zähere Brand-Häute": I("item_cinderhound_pelt_tough", "Zähes Brandfell", "R"),
    "Stärkere Sehnen & Glut-Reste": S(G("sinew"), I("item_spark_ash")),
    # 15 Graveclaw
    "Erd-/Schutt-Schulterstücke": I("item_graveclaw_shoulder", "Schutt-Schulterplatte", "R"),
    "Gehärtete Schulterplatten": I("item_graveclaw_shoulder_hard", "Gehärtete Schulterplatte", "R"),
    # 16 Rimestalker
    "Kalt-graue Hautstücke": I("item_rimestalker_hide", "Kaltgraue Haut", "R"),
    "Scharfe Glieder-/Hornspitzen": I("item_rimestalker_spike", "Hornspitze", "W"),
    "Dichte Kälte-Häute": I("item_rimestalker_hide_dense", "Dichte Kältehaut", "R"),
    "Hochlagen-Teile": I("item_rimestalker_frost_spike", "Frostdorn", "W"),
    # 17 Hollowback
    "Ausgehöhlte Rücken-/Rumpfstücke": I("item_hollowback_shell", "Hohlrückenschale", "R"),
    "Staub-/Sporen-/Ruß-Reste": I("item_hollowback_spore_dust", "Sporenstaub", "V"),
    "Gehärtete Rumpfstrukturen": I("item_hollowback_shell_hard", "Gehärtete Rumpfschale", "R"),
    "Dichtere Wolken-/Materialreste": I("item_hollowback_spore_sac", "Sporensack", "V"),
    # 18 Ironhide
    "Eisen-/Krusten-Panzerplatten": I("item_ironhide_plate", "Eisenkrusten-Platte", "R"),
    "Massive Panzerplatten": I("item_ironhide_plate_massive", "Massive Panzerplatte", "R"),
    "Metall-/Mineral-Reste": I("item_ironhide_ore_crust", "Erzkruste", "X"),
    # 19 Shroudwalker
    "Glimmende Kernreste": I("item_shadow_core_shard", "Schattenkern-Splitter", "V"),
    "Relikt-Teile": I("item_shroud_relic", "Schleier-Relikt", "X"),
    # 20 Cragmaw
    "Zackige Kiefer-/Horn-Teile": I("item_cragmaw_jaw", "Zackenkiefer", "W"),
    "Steinkamm-Stücke": I("item_cragmaw_crest", "Steinkamm", "R"),
    "Große Kiefer-/Horn-Trophäen": I("item_cragmaw_trophy", "Kiefer-Trophäe", "X"),
    # 21 Dreadcolossus
    "Massive Stein-/Mineral-/Relikt-Teile": I("item_colossus_stone", "Koloss-Gestein", "X"),
    "Koloss-Knochen/Sehnen": I("item_colossus_bone", "Koloss-Knochen", "W"),
    "Spitzen-Materialien": I("item_colossus_heartstone", "Koloss-Herzstein", "V"),
    "Seltener Koloss-Spitzenrest": I("item_colossus_relic", "Koloss-Relikt", "X"),
    "Seltener Koloss-Kern/Rest": I("item_colossus_core", "Koloss-Kern", "V"),
    # 22 Troll
    "Dicke Trollhaut": I("item_troll_hide", "Trollhaut", "R"),
    "Besonders dicke Häute": I("item_troll_hide_thick", "Dicke Trollhaut", "R"),
    # 23 Riese
    "Große Knochen-/Sehnenstücke": I("item_giant_bone", "Riesenknochen", "W"),
    "Schwere Rüstungs-/Reliktreste": I("item_giant_armor_scrap", "Riesen-Rüstungsschrott", "X"),
    "Kolossale Knochen-/Sehnen-Teile": I("item_giant_bone_massive", "Kolossaler Riesenknochen", "W"),
    "Spitzen-Relikte": I("item_giant_relic", "Riesen-Relikt", "X"),
    # 24 Wiedergänger
    "Bessere Metall-/Waffen-/Rüstungsfragmente": I("item_revenant_weapon_fragment", "Waffenbruchstück", "X"),
    "Seltenere Relikte": I("item_revenant_grave_goods", "Grabbeigabe", "X"),
    # 25 Cinderwyrm
    "Hitzebeständige Knochen/Hörner": I("item_cinderwyrm_horn", "Glutdrachenhorn", "W"),
    "Hochwertige Glut-Schuppen": I("item_ember_scale_fine", "Edle Glutschuppe", "R"),
    "Spitzen-Komponenten": I("item_cinderwyrm_heart", "Glutdrachenherz", "V"),
    # 26 Cragdrake
    "Granit-/Fels-Schuppen": I("item_cragdrake_scale", "Felsdrachenschuppe", "R"),
    "Klammerkrallen": I("item_cragdrake_claw", "Klammerkralle", "W"),
    "Fels-Klaue": I("item_cragdrake_claw"),
    "Schwere Berg-Schuppen": I("item_cragdrake_scale_heavy", "Schwere Bergschuppe", "R"),
    "Hochgebirgs-/Drachen-Werkstoffe": I("item_cragdrake_heart", "Felsdrachenherz", "V"),
    # 27 Stormmaw
    "Gebogene Hörner/Zähne": I("item_stormmaw_horn", "Sturmdrachenhorn", "W"),
    "Sturm-Horn": I("item_stormmaw_horn"),
    "Seltene Sturm-/Relikt-Materialien": I("item_stormmaw_heart", "Sturmdrachenherz", "V"),
    # 28 Ashwing
    "Asche-Schwingenhaut": I("item_ashwing_wing_hide", "Asche-Schwingenhaut", "R"),
    "Asche-Haut": I("item_ashwing_wing_hide"),
    "Hochwertige Gleit-Schwingenhäute": I("item_ashwing_wing_hide_fine", "Gleit-Schwingenhaut", "R"),
    "Asche-/Drachen-Teile": I("item_ashwing_heart", "Aschedrachenherz", "V"),
    # 29 Frostdrake
    "Frost-/Schiefer-Schuppen": I("item_frostdrake_scale", "Frostdrachenschuppe", "R"),
    "Frost-Schuppe": I("item_frostdrake_scale"),
    "Gehärtete Frost-Schuppen": I("item_frostdrake_scale_hard", "Gehärtete Frostschuppe", "R"),
    # 30 Dreadwyrm
    "Spitzen-Drachenschuppen": I("item_dreadwyrm_scale", "Wyrmschuppe", "R"),
    "Uralte Knochen-/Horn- & Relikt-Teile": S(I("item_dreadwyrm_horn", "Uraltes Wyrmhorn", "W"),
                                             I("item_wyrm_relic", "Wyrm-Relikt", "X")),
    "Uralte Drachen-Reliktreste": I("item_wyrm_relic"),
    "Seltene Wyrm-Spitzenrelikte": I("item_dreadwyrm_heart", "Wyrmherz", "V"),
    # 31 Bramblefang
    "Dornen-Balg": I("item_bramblefang_pelt", "Dornenbalg", "R"),
    "Scharfe Fangzähne": I("item_bramblefang_fang", "Fangzahn", "W"),
    "Gehärteter Dornen-Balg": I("item_bramblefang_pelt_hard", "Gehärteter Dornenbalg", "R"),
    "Verstärkte Fangzähne": I("item_bramblefang_fang_strong", "Verstärkter Fangzahn", "W"),
    # 32 Siltstrider
    "Trockene Laufhaut": I("item_siltstrider_hide", "Laufhaut", "R"),
    "Lange Laufsehnen": I("item_siltstrider_sinew", "Lange Laufsehne", "B"),
    "Zähere Laufhaut & längere Sehnen": S(I("item_siltstrider_hide_tough", "Zähe Laufhaut", "R"),
                                         I("item_siltstrider_sinew")),
    "Gehärtete Laufknochen & dichtere Fasern": S(G("bone"), I("item_siltstrider_sinew")),
    # 33 Cinderling
    "Glimmender Splitter": I("item_cinderling_ember_shard", "Glimmsplitter", "V"),
    "Dichtere Funken-Asche & zähere Fasern": S(I("item_spark_ash"), I("item_charred_fiber")),
    # 34 Sporeback
    "Sporenkappen-Rest": I("item_sporeback_cap", "Sporenkappe", "V"),
    "Buckelige Rumpfhaut": I("item_sporeback_hide", "Buckelhaut", "R"),
    "Konzentrierte Sporenkapsel & gehärtete Rumpfhaut": S(
        I("item_sporeback_spore_capsule", "Sporenkapsel", "V"),
        I("item_sporeback_hide_hard", "Gehärtete Buckelhaut", "R")),
    "Seltene Sporen-Essenz": I("item_spore_essence", "Sporen-Essenz", "V"),
    # 35 Skitterclaw
    "Klammer-Chitin": I("item_skitterclaw_chitin", "Klammer-Chitin", "R"),
    "Scharfe Krallenspitzen": I("item_skitterclaw_claw", "Krallenspitze", "W"),
    "Gehärtetes Klammer-Chitin & dickere Krallen": S(
        I("item_skitterclaw_chitin_hard", "Gehärtetes Klammer-Chitin", "R"), I("item_skitterclaw_claw")),
    "Seltene Chitin-Platte": I("item_skitterclaw_chitin_plate", "Seltene Chitinplatte", "R"),
    # 36 Gloomstalker
    "Dichtere Schatten-Bahnen & stärkere Sehnen": S(I("item_shadow_weave_dense"), G("sinew")),
    "Glimmender Schattenkern": I("item_shadow_core", "Schattenkern", "V"),
    # 37 Charmaw
    "Verkohlter Kieferknochen": I("item_charmaw_jaw", "Verkohlter Kiefer", "W"),
    "Brand-Sehnen": I("item_charmaw_burn_sinew", "Brandsehne", "B"),
    "Rußige Kieferzähne": I("item_charmaw_tooth", "Rußzahn", "W"),
    "Gehärteter Kieferknochen & glimmende Zahnreste": S(
        I("item_charmaw_jaw_hard", "Gehärteter Kiefer", "W"), I("item_charmaw_tooth")),
    "Schwerer Glut-Kiefer": I("item_charmaw_ember_jaw", "Glutkiefer", "W"),
    # 38 Rimehound
    "Kalt-borstiges Fell": I("item_rimehound_fur", "Kaltborstenfell", "R"),
    "Raue Fangzähne": I("item_rimehound_fang", "Raufangzahn", "W"),
    "Dichtes Hochlagen-Fell & kristalline Sehnen": S(
        I("item_rimehound_fur_dense", "Dichtes Hochlagenfell", "R"), I("item_frost_sinew")),
    "Frost-Kernstück": I("item_frost_core", "Frostkern", "V"),
    # 39 Slagbrute
    "Dichte Schlacke-Platte & gehärtete Gesteinsbrocken": S(I("item_slag_plate_dense"), F("item_basalt_chunk")),
    # 40 Vaultcrawler
    "Gewölbe-Panzerplatten": I("item_vaultcrawler_plate", "Gewölbeplatte", "R"),
    "Greifarm-Gelenke": I("item_vaultcrawler_joint", "Greifarm-Gelenk", "W"),
    "Verstärkte Gewölbe-Platten & gehärtete Greifgelenke": S(
        I("item_vaultcrawler_plate_strong", "Verstärkte Gewölbeplatte", "R"), I("item_vaultcrawler_joint")),
    "Seltene Gewölbe-Reliktplatte": I("item_vaultcrawler_relic_plate", "Gewölbe-Reliktplatte", "X"),
    # 41 Mossgolem
    "Verwachsene Wurzel-/Steinbrocken": I("item_mossgolem_rootstone", "Wurzelstein", "X"),
    "Moosdurchsetzte Rinde": I("item_mossgolem_bark", "Moosrinde", "R"),
    "Zähes Bindematerial": I("item_mossgolem_bast", "Wurzelbast", "B"),
    "Uralte Wurzel-/Steinkerne & dichtere Rindenplatten": S(
        I("item_mossgolem_root_core", "Uralter Wurzelkern", "V"), I("item_mossgolem_bark")),
    "Seltener Wächter-Kern": I("item_mossgolem_warden_core", "Wächterkern", "V"),
    # 42 Galehound
    "Sturm-Borstenfell": I("item_galehound_fur", "Sturmborstenfell", "R"),
    "Gespannte Böen-Sehnen": I("item_galehound_sinew", "Böensehne", "B"),
    "Geladener Kernrest & hochgespannte Sehnen": S(F("item_charged_core"), I("item_galehound_sinew")),
    # 43 Cragstalker
    "Grat-Schuppenhaut": I("item_cragstalker_hide", "Gratschuppenhaut", "R"),
    "Kräftige Sprungsehnen": I("item_cragstalker_sinew", "Sprungsehne", "B"),
    "Gebogene Kammhörner": I("item_cragstalker_horn", "Kammhorn", "W"),
    "Gehärtete Grat-Schuppen & seltene Hochgebirgs-Trophäe": S(
        I("item_cragstalker_hide_hard", "Gehärtete Gratschuppe", "R"),
        I("item_cragstalker_trophy", "Grat-Trophäe", "X")),
    # 44 Ashweaver
    "Verkohlte Chitin-/Faserreste": I("item_ashweaver_chitin", "Verkohltes Chitin", "R"),
    # 45 Frostmauler
    "Dicke Frost-Schuppenbrocken": I("item_frostmauler_scale", "Frostschuppenbrocken", "R"),
    "Massive Prankenknochen": I("item_frostmauler_paw_bone", "Prankenknochen", "W"),
    "Gehärteter Frost-Panzer": I("item_frostmauler_armor", "Gehärteter Frostpanzer", "R"),
    "Seltener Hochlagen-Knochen": I("item_frostmauler_rare_bone", "Hochlagen-Knochen", "W"),
    # 46 Stormwyrmling
    "Junge Sturm-Schwingenhaut": I("item_stormwyrmling_wing_hide", "Junge Sturm-Schwingenhaut", "R"),
    "Schmale Drachenhörner": I("item_stormwyrmling_horn", "Schmales Drachenhorn", "W"),
    "Geladener Drachenkernrest": I("item_charged_dragon_core", "Geladener Drachenkern", "V"),
    # 47 Cindercolossus
    "Massive Schlacke-Kolossplatte": I("item_colossus_slag_plate", "Schlacke-Kolossplatte", "R"),
    "Glühender Koloss-Kern": I("item_colossus_core_glowing", "Glühender Kolosskern", "V"),
    "Schwere Gesteins-/Knochenmasse": I("item_colossus_stone"),
    "Seltener Spitzen-Schlacke-Relikt": I("item_colossus_slag_relic", "Schlacke-Relikt", "X"),
    # 48 Galewyrm
    "Spitzen-Sturm-Schwingenhaut": I("item_galewyrm_wing_hide", "Sturmwyrm-Schwingenhaut", "R"),
    "Gebogene Sturm-Drachenhörner": I("item_galewyrm_horn", "Sturmwyrm-Horn", "W"),
    "Seltener Sturm-Wyrm-Kern": I("item_galewyrm_core", "Sturmwyrm-Kern", "V"),
}

# Gleicher Gemini-Sammelbegriff, aber je Kreatur ein anderer konkreter Gegenstand
OVERRIDES = {
    (9, "Hochlagen-Komponenten"): I("item_galeborn_storm_gland", "Sturmdrüse", "V"),
    (29, "Hochlagen-Komponenten"): I("item_frostdrake_heart", "Frostdrachenherz", "V"),
    (20, "Seltene Rohstoffe"): I("item_cragmaw_crest_core", "Kammkern", "V"),
    (23, "Seltene Rohstoffe"): I("item_giant_tooth", "Riesenzahn", "W"),
}


# ---------------------------------------------------------------------------
# Einlesen
# ---------------------------------------------------------------------------
def parse_values(text: str) -> dict[int, tuple[str, int, dict[int, tuple[int, int, int]]]]:
    section = text[text.index("## 4."):text.index("## 5.")]
    row_re = re.compile(r"^\| \*\*(\d+)\*\* \| \*\*([^*]+)\*\* \| (?:`(\d)`|\*\*Rang (\d)\*\*) \|(.+)\|\s*$", re.M)
    cell_re = re.compile(r"`([\d.]+) / (\d+) / (\d+)`")
    out = {}
    for m in row_re.finditer(section):
        nr, name = int(m.group(1)), m.group(2).strip()
        min_rank = int(m.group(3) or m.group(4))
        cells = [c.strip() for c in m.group(5).split("|")]
        values = {}
        for rank, cell in zip(range(2, 7), cells):
            mm = cell_re.fullmatch(cell)
            if mm:
                values[rank] = (int(mm.group(1).replace(".", "")), int(mm.group(2)), int(mm.group(3)))
        out[nr] = (name, min_rank, values)
    return out


def parse_counts(text: str) -> tuple[int, int]:
    parts = text.replace("-", "–").split("–")
    return int(parts[0]), int(parts[-1])


def parse_loot(text: str) -> dict[int, list[dict]]:
    section = text[text.index("### 6.2"):text.index("### 6.5")]
    row_re = re.compile(r"^\| \*\*(\d+)\*\* \| \*\*([^*]+)\*\* \(`Rang (\d)`\) \| (.*?) \| (.*?) \| (.*?) \|\s*$", re.M)
    entry_re = re.compile(r"•\s*([^•<]+?)\s*\(`([^`]+)`\s*\\\|\s*`([^`]+)`\s*\\\|\s*R(\d)\s*\\\|\s*(Ja|Nein)\)")
    biome_part_re = re.compile(r"•\s*\*([^*]+):\*\s*(.+?)(?=<br>|$)")
    biome_item_re = re.compile(r"([^,(]+?)\s*\(`([^`]+)`,\s*`([^`]+)`\)")
    out: dict[int, list[dict]] = {}
    for m in row_re.finditer(section):
        nr = int(m.group(1))
        entries = []
        for kind, cell in (("base", m.group(4)), ("rank", m.group(5))):
            for e in entry_re.finditer(cell):
                entries.append({"kind": kind, "name": e.group(1).strip(), "count": parse_counts(e.group(2)),
                                "chance": float(e.group(3)), "min_rank": int(e.group(4)),
                                "scale_count": e.group(5) == "Ja"})
        for b in biome_part_re.finditer(m.group(6)):
            labels = [x.strip() for x in b.group(1).split("/")]
            for it in biome_item_re.finditer(b.group(2)):
                entries.append({"kind": "biome", "name": it.group(1).strip(), "count": parse_counts(it.group(2)),
                                "chance": float(it.group(3)), "min_rank": None, "scale_count": False,
                                "labels": labels})
        out[nr] = entries
    return out


# ---------------------------------------------------------------------------
# Auswerten
# ---------------------------------------------------------------------------
def zone_ranks(biome: str, only_places: bool) -> list[int]:
    rank = BIOMES[biome][2]
    place = min(6, rank + 1)
    return [place] if only_places else [rank, place]


def spawn_ranks(entry, biome: str, min_rank: int) -> set[int]:
    """Alle Ränge, die ein Spawn-Eintrag erzeugen kann (voll mitgewachsen oder einen Rang darunter)."""
    _, _, behaviour, only_places = entry
    ranks: set[int] = set()
    zones = zone_ranks(biome, only_places)
    if behaviour[0] == "fest":
        if behaviour[1] < min_rank:
            raise ValueError(f"Nr. {entry[0]} in {biome}: fester Rang unter MinRang")
        return {behaviour[1]}
    for zone in zones:
        top = zone if behaviour[0] == "mit" else min(zone, behaviour[1])
        ranks.add(top)
        ranks.add(max(min_rank, top - 1))
    lowest_top = zones[0] if behaviour[0] == "mit" else min(zones[0], behaviour[1])
    if lowest_top < min_rank:
        raise ValueError(f"Nr. {entry[0]} in {biome}: Rang {lowest_top} liegt unter MinRang {min_rank}")
    return ranks


def behaviour_text(behaviour) -> str:
    if behaviour[0] == "mit":
        return f"wächst mit ({behaviour[1]:.0%})".replace("100%", "immer").replace("%", " %")
    if behaviour[0] == "bis":
        return f"wächst mit bis Rang {behaviour[1]} ({behaviour[2]:.0%})".replace("%", " %")
    return f"fest Rang {behaviour[1]} (Nachzügler)"


def rank_span(ranks: set[int]) -> str:
    lo, hi = min(ranks), max(ranks)
    return str(lo) if lo == hi else f"{lo}–{hi}"


def resolve(nr: int, name: str):
    spec = OVERRIDES.get((nr, name)) or LOOT.get(name)
    if spec is None:
        raise KeyError(name)
    return spec


def fmt_count(count: tuple[int, int]) -> str:
    return str(count[0]) if count[0] == count[1] else f"{count[0]}–{count[1]}"


def fmt_values(v) -> str:
    return f"{v[0]:,}".replace(",", ".") + f" / {v[1]} / {v[2]}"


def main() -> dict:
    text = SOURCE.read_text(encoding="utf-8")
    values = parse_values(text)
    loot = parse_loot(text)
    if sorted(values) != list(range(1, 49)) or sorted(loot) != list(range(1, 49)):
        raise SystemExit(f"Erwartet 48 Kreaturen, gefunden Werte={len(values)}, Beute={len(loot)}")

    missing = sorted({(nr, e["name"]) for nr, es in loot.items() for e in es
                      if (nr, e["name"]) not in OVERRIDES and e["name"] not in LOOT})
    if missing:
        raise SystemExit("Ohne Zuordnung: " + "; ".join(f"{nr}: {n}" for nr, n in missing))

    # Spawn, Heimat, erreichte Ränge
    biomes_of: dict[int, list[str]] = defaultdict(list)
    ranks_of: dict[int, set[int]] = defaultdict(set)
    for biome in BIOME_ORDER:
        for entry in SPAWNS[biome]:
            nr = entry[0]
            ranks = spawn_ranks(entry, biome, values[nr][1])
            entry_ranks = ranks
            ranks_of[nr] |= entry_ranks
            if biome not in biomes_of[nr]:
                biomes_of[nr].append(biome)
    if sorted(biomes_of) != list(range(1, 49)):
        raise SystemExit("Kreaturen ohne Spawn: " + str(sorted(set(range(1, 49)) - set(biomes_of))))
    home = {nr: min(bs, key=BIOME_ORDER.index) for nr, bs in biomes_of.items()}
    wave = {nr: WAVE_OF_BIOME[home[nr]] for nr in home}
    max_rank = {nr: max(r) for nr, r in ranks_of.items()}

    # Beute bereinigen
    clean: dict[int, list[dict]] = {}
    dropped: dict[int, list[str]] = defaultdict(list)
    item_tiers: dict[str, set[int]] = defaultdict(set)
    item_sources: dict[str, set[int]] = defaultdict(set)
    generic_ranks: dict[str, set[int]] = defaultdict(set)
    stats = defaultdict(int)
    gemini_names = set()
    for nr in range(1, 49):
        rows = []
        for e in loot[nr]:
            stats["entries"] += 1
            gemini_names.add(e["name"])
            spec = resolve(nr, e["name"])
            parts = list(spec[1]) if spec[0] == "split" else [spec]
            if spec[0] == "split":
                stats["split"] += 1
            merged = [("item", VARIANT_OF[p[1]]) if p[0] == "item" and p[1] in VARIANT_OF else p for p in parts]
            stats["variants_merged"] += sum(1 for a, b in zip(parts, merged) if a != b)
            parts = merged
            if e["kind"] == "biome":
                mapped = []
                for label in e["labels"]:
                    if label.startswith("Alle"):
                        mapped.extend(biomes_of[nr])
                    else:
                        mapped.extend(GEMINI_BIOMES[label])
                applies = [b for b in BIOME_ORDER if b in mapped and b in biomes_of[nr]]
                if not applies:
                    dropped[nr].append(f"{' / '.join(e['labels'])}: {e['name']}")
                    stats["dropped_biome"] += 1
                    continue
                min_rank, shifted = None, False
            else:
                applies = []
                min_rank, shifted = e["min_rank"], False
                if min_rank > max_rank[nr]:
                    min_rank, shifted = max_rank[nr], True
                    stats["shifted"] += 1
            for i, part in enumerate(parts):
                count = e["count"] if i == 0 else (1, 1)
                row = {"kind": e["kind"], "count": count, "chance": e["chance"], "min_rank": min_rank,
                       "scale_count": e["scale_count"], "biomes": applies, "gemini": e["name"],
                       "split": spec[0] == "split", "shifted": shifted, "orig_rank": e["min_rank"]}
                if part[0] == "gen":
                    row["generic"] = part[1]
                    lo = min_rank if min_rank is not None else min(BIOMES[b][2] for b in applies)
                    reach = [r for r in ranks_of[nr] if r >= lo]
                    if part[1] == "dragon_sinew" and min(reach) < 5:
                        raise SystemExit(f"Drachensehne unter Rang 5 bei Nr. {nr}")
                    generic_ranks[part[1]] |= set(reach)
                else:
                    item_id = part[1]
                    row["item"] = item_id
                    item_sources[item_id].add(nr)
                    if item_id in ITEMS:
                        tier = min_rank if min_rank is not None else min(BIOMES[b][2] for b in applies)
                        item_tiers[item_id].add(tier)
                rows.append(row)
        clean[nr] = rows

    unknown = sorted(i for i in item_sources if i not in ITEMS and i not in FIXED)
    if unknown:
        raise SystemExit("Gegenstand ohne Namen: " + ", ".join(unknown))
    tier_of = {i: min(t) for i, t in item_tiers.items()}
    for i in item_sources:
        if i in FIXED:
            tier_of[i] = FIXED[i][2]

    named_used = sorted(i for i in item_sources if i in ITEMS)
    fixed_used = sorted(i for i in item_sources if i in FIXED)
    generic_count = sum(len(r) for r in generic_ranks.values())

    # Mittelwerte je Rang
    means = {}
    for rank in range(2, 7):
        vs = [values[nr][2][rank] for nr in values if rank in values[nr][2]]
        means[rank] = tuple(round(sum(v[k] for v in vs) / len(vs)) for k in range(3)) + (len(vs),)

    def item_label(item_id: str) -> str:
        name = ITEMS[item_id][0] if item_id in ITEMS else FIXED[item_id][0]
        return name

    # ------------------------------------------------------------------
    # Ausgabe
    # ------------------------------------------------------------------
    L: list[str] = []
    w = L.append
    w("# Kreaturen-Daten (erzeugt)")
    w("")
    w(f"> **Erzeugt** am {datetime.date.today().isoformat()} von `tools/data/creatures_from_gemini.py` aus "
      "`design/referenz/gemini_kuestenasche_gesamtdokumentation.md`. **Nicht von Hand ändern:** Zuordnungen im "
      "Skript anpassen und neu erzeugen. Regeln und Begründungen: `docs/21_kreaturen_level_kampf.md`.")
    w(">")
    w("> Status: Einordnung, Spawn und Beute-Zuordnung sind **Vorschläge von Claude** (2026-11-22). Die Werte "
      "(LP / Haltung / Schaden), Spawn-Gewichte für Nr. 31–48 und die Beute-Chancen sind **Geminis Startwerte**.")
    w("")
    w("## 1. Überblick")
    w("")
    w("| Was | Anzahl |")
    w("|---|---|")
    w("| Kreaturen | 48 |")
    w(f"| Beute-Einträge bei Gemini | {stats['entries']} |")
    w(f"| verschiedene Beute-Namen bei Gemini | {len(gemini_names)} |")
    w(f"| davon „A & B“-Einträge, die in zwei Gegenstände geteilt werden | {stats['split']} |")
    w(f"| Biom-Zusatzbeute, die entfällt (Kreatur kommt in dem Biom nicht vor) | {stats['dropped_biome']} |")
    w(f"| Rang-Beute, die auf den höchsten erreichbaren Rang vorgezogen wird | {stats['shifted']} |")
    w(f"| Beute-Einträge, die auf das Grundteil statt auf eine Güte-Variante zeigen (Entscheidung 2026-11-22) | {stats['variants_merged']} |")
    w(f"| **Gegenstände nach der Bereinigung** | **{len(named_used) + len(fixed_used) + generic_count}** |")
    w(f"| davon kreaturentypische Teile | {len(named_used)} |")
    w(f"| davon Biom-Materialien und Knorpel (feste Stufe) | {len(fixed_used)} |")
    w(f"| davon allgemeine Teile je Stufe, die Kreaturen geben (Sehne, Knochen, Haut, Horn, Drachensehne; "
      f"Stufe 1 kommt mit den Tieren dazu) | {generic_count} |")
    w("")
    w("Für jeden dieser Gegenstände braucht es später einen Eintrag in `items.json` und ein Icon.")
    w("")

    w("## 2. Kreaturen nach Modell-Welle")
    w("")
    w("Eine Kreatur kommt nur in ihrem Heimat-Biom und in späteren Biomen vor. Ihr Modell wird mit der Welle "
      "ihres Heimat-Bioms gebaut (Entscheidung „Kreaturen: Reihenfolge“, 2026-11-22).")
    w("")
    w("| Welle | Heimat-Biom | Anzahl | Kreaturen |")
    w("|---|---|---|---|")
    w("| Kern | Fjordwiesen | 0 | keine (Tiere, Waldbanditen, Alter Keiler) |")
    for biome, wv in WAVE_OF_BIOME.items():
        nrs = [nr for nr in range(1, 49) if home[nr] == biome]
        label = "Kern (1)" if wv == 1 else str(wv)
        w(f"| {label} | {BIOMES[biome][1]} | {len(nrs)} | " + ", ".join(CREATURES[nr][1] for nr in nrs) + " |")
    w("")

    w("## 3. Einordnung aller 48 Kreaturen")
    w("")
    w("| Nr. | ID | Name (Gemini) | Anzeigename (Vorschlag) | Rolle | MinRang | Heimat | auch in | erreichte Ränge | Welle |")
    w("|---|---|---|---|---|---|---|---|---|---|")
    for nr in range(1, 49):
        cid, name, role = CREATURES[nr]
        also = [BIOMES[b][1] for b in biomes_of[nr] if b != home[nr]]
        w(f"| {nr} | `{cid}` | {name} | {DISPLAY[nr]} | {role} | {values[nr][1]} | {BIOMES[home[nr]][1]} | "
          f"{', '.join(also) or '–'} | {rank_span(ranks_of[nr])} | {wave[nr]} |")
    w("")

    w("## 4. Werte je Rang (LP / Haltung / Schaden)")
    w("")
    w("Geminis Basiswerte vor Streuung (85–115 %) und vor Anführer-Stufen. Werte für Ränge, die die Kreatur in "
      "unserer Biom-Ordnung nie erreicht, stehen in Klammern.")
    w("")
    w("| Nr. | Name | Rang 2 | Rang 3 | Rang 4 | Rang 5 | Rang 6 |")
    w("|---|---|---|---|---|---|---|")
    for nr in range(1, 49):
        cells = []
        for rank in range(2, 7):
            v = values[nr][2].get(rank)
            if v is None:
                cells.append("–")
            elif rank in ranks_of[nr]:
                cells.append(fmt_values(v))
            else:
                cells.append(f"({fmt_values(v)})")
        w(f"| {nr} | {CREATURES[nr][1]} | " + " | ".join(cells) + " |")
    w("")
    w("**Mittelwerte je Rang** (über alle Kreaturen mit Werten für diesen Rang; Grundlage für Tiere und Bosse):")
    w("")
    w("| Rang | LP | Haltung | Schaden | Kreaturen |")
    w("|---|---|---|---|---|")
    for rank in range(2, 7):
        m = means[rank]
        w(f"| {rank} | {m[0]:,} | {m[1]} | {m[2]} | {m[3]} |".replace(",", "."))
    w("")

    w("## 5. Spawn-Tabellen je Biom")
    w("")
    w("Biom-Rang = Stufe des Bioms; an gefährlichen Orten gilt Rang + 1 (höchstens 6). „Wächst mit (85 %)“: "
      "85 % erreichen den Rang des Ortes, der Rest einen Rang darunter (mindestens MinRang).")
    w("")
    for biome in BIOME_ORDER:
        bid, bname, brank, places = BIOMES[biome]
        w(f"### {bname} (`{bid}`, Rang {brank}; gefährliche Orte Rang {min(6, brank + 1)}: {places})")
        w("")
        w(BIOME_NOTES[biome])
        w("")
        if not SPAWNS[biome]:
            continue
        w("| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |")
        w("|---|---|---|---|---|---|")
        for entry in SPAWNS[biome]:
            nr, weight, behaviour, only_places = entry
            ranks = spawn_ranks(entry, biome, values[nr][1])
            where = "nur gefährliche Orte" if only_places else "überall"
            weight_text = f"{weight:.2f}".replace(".", ",")
            w(f"| {CREATURES[nr][1]} | {weight_text} | {behaviour_text(behaviour)} | {where} | "
              f"{rank_span(ranks)} | {wave[nr]} |")
        w("")

    w("## 6. Beute je Kreatur (bereinigt)")
    w("")
    w("Spalten: **Menge** und **Chance** wie bei Gemini; **ab Rang** = Rang-Sperre; **+Menge** = Mengenbonus "
      "(+1 je Rang über „ab Rang“, höchstens +2). Allgemeine Teile (Sehne, Knochen, Haut, Horn, Drachensehne) "
      "bekommen die Stufe des Rangs, den die Kreatur beim Tod hat. Biom-Zusatzbeute fällt nur im genannten Biom "
      "und wird bei Nachzüglern gedämpft (siehe `docs/21_kreaturen_level_kampf.md`, Abschnitt 10).")
    w("")
    for nr in range(1, 49):
        cid, name, _ = CREATURES[nr]
        w(f"### {nr}. {name} (`{cid}`, MinRang {values[nr][1]}, erreicht {rank_span(ranks_of[nr])})")
        w("")
        w("| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |")
        w("|---|---|---|---|---|---|---|---|")
        for row in clean[nr]:
            if row["kind"] == "biome":
                art = "Biom: " + ", ".join(BIOMES[b][1] for b in row["biomes"])
                at = "–"
            else:
                art = "Grund" if row["kind"] == "base" else "Rang"
                at = str(row["min_rank"])
            if "generic" in row:
                fam = GENERIC[row["generic"]]
                label = f"{fam[3]} (Stufe = Rang)"
                ident = f"`{fam[0].format('<Rang>')}`"
            else:
                label = item_label(row["item"])
                ident = f"`{row['item']}`"
            note = row["gemini"]
            if row["split"]:
                note += " (geteilt)"
            if row["shifted"]:
                note += f" (vorgezogen von R{row['orig_rank']})"
            chance_text = f"{row['chance']:.2f}".replace(".", ",")
            w(f"| {art} | {label} | {ident} | {fmt_count(row['count'])} | {chance_text} | "
              f"{at} | {'Ja' if row['scale_count'] else '–'} | {note} |")
        if dropped[nr]:
            w("")
            w("Entfällt (Kreatur kommt dort nicht vor): " + "; ".join(dropped[nr]) + ".")
        w("")

    w("## 7. Gegenstands-Katalog")
    w("")
    w("### 7.1 Allgemeine Teile je Stufe")
    w("")
    w("Stufe = Rang der Kreatur bzw. des Tiers beim Tod. Auch Tiere geben diese Teile (Abschnitt 8 in Dokument 21).")
    w("")
    w("| Teil | IDs | Namen (Stufe 1–6) | Art |")
    w("|---|---|---|---|")
    for fam, (pattern, names, kind, title) in GENERIC.items():
        tiers = [t for t in range(1, 7) if names[t - 1]]
        ids = ", ".join(f"`{pattern.format(t)}`" for t in tiers)
        w(f"| {title} | {ids} | {' · '.join(n for n in names if n)} | {KINDS[kind]} |")
    w("")
    w("### 7.2 Biom-Materialien und Knorpel (feste Stufe)")
    w("")
    w("| ID | Name | Stufe | Art | Herkunft | Kreaturen |")
    w("|---|---|---|---|---|---|")
    for item_id in fixed_used:
        name, kind, tier, note = FIXED[item_id]
        srcs = ", ".join(CREATURES[n][1] for n in sorted(item_sources[item_id]))
        w(f"| `{item_id}` | {name} | {tier} | {KINDS[kind]} | {note} | {srcs} |")
    w("")
    w("### 7.3 Kreaturentypische Teile")
    w("")
    w("Stufe = niedrigster Rang, ab dem der Teil fallen kann.")
    w("")
    w("| ID | Name | Stufe | Art | Kreaturen |")
    w("|---|---|---|---|---|")
    for item_id in sorted(named_used, key=lambda i: (tier_of[i], i)):
        name, kind = ITEMS[item_id]
        srcs = ", ".join(CREATURES[n][1] for n in sorted(item_sources[item_id]))
        w(f"| `{item_id}` | {name} | {tier_of[item_id]} | {KINDS[kind]} | {srcs} |")
    w("")

    TARGET.write_text("\n".join(L) + "\n", encoding="utf-8", newline="\n")
    print(f"geschrieben: {TARGET.relative_to(ROOT)}")
    print(f"Gemini-Namen: {len(gemini_names)}, Einträge: {stats['entries']}, geteilt: {stats['split']}, "
          f"Biom entfällt: {stats['dropped_biome']}, vorgezogen: {stats['shifted']}")
    print(f"Gegenstände: benannt {len(named_used)}, fest {len(fixed_used)}, allgemein {generic_count}, "
          f"gesamt {len(named_used) + len(fixed_used) + generic_count}")
    print("Mittelwerte je Rang:", means)
    print("Wellen:", {wv: sum(1 for n in wave if wave[n] == wv) for wv in range(1, 6)})
    return {
        "values": values, "clean": clean, "biomes_of": biomes_of, "ranks_of": ranks_of, "home": home, "wave": wave,
        "tier_of": tier_of, "item_sources": item_sources, "generic_ranks": generic_ranks,
        "named_used": named_used, "fixed_used": fixed_used,
    }


# ---------------------------------------------------------------------------
# JSON-Ausgabe (Schritt 1.1, docs/24): game/Content/Data/*.json
# ---------------------------------------------------------------------------
JSON_DIR = ROOT / "game" / "Content" / "Data"

TIER_HOURS = [6, 8, 10, 12, 14, 16]  # Vorschlag: Spielstunden je Stufe (solo), Annahme
TIER_LEVELS = [(1, 8), (9, 16), (17, 25), (26, 34), (35, 42), (43, 50)]
# „Typische Stunde“: Mischung der Erfahrungsquellen, daraus ergibt sich die Erfahrung pro Stunde und Stufe
TYPICAL_HOUR = {"kills": 15, "kill_xp_per_rank": 10, "quests": 1, "quest_xp_per_tier": 50, "discoveries": 1,
                "discovery_xp_per_tier": 25, "first_crafts": 3, "first_craft_xp_per_tier": 20, "repeat_crafts": 20,
                "repeat_craft_xp_per_tier": 1, "build_pieces": 40, "build_xp_per_tier": 2, "gathered": 150,
                "gather_xp_per_tier": 0.5}


def typical_hour_xp_per_tier() -> float:
    h = TYPICAL_HOUR
    return (h["kills"] * h["kill_xp_per_rank"] + h["quests"] * h["quest_xp_per_tier"]
            + h["discoveries"] * h["discovery_xp_per_tier"] + h["first_crafts"] * h["first_craft_xp_per_tier"]
            + h["repeat_crafts"] * h["repeat_craft_xp_per_tier"] + h["build_pieces"] * h["build_xp_per_tier"]
            + h["gathered"] * h["gather_xp_per_tier"])


# Tiere (docs/21, Abschnitt 8)
ANIMAL_PARTS = {
    "item_boar_tusk": ("Keilerhauer", "W", 1), "item_deer_antler": ("Hirschgeweih", "W", 1),
    "item_hare_pelt": ("Hasenfell", "R", 1), "item_fox_pelt": ("Fuchsfell", "R", 1),
    "item_wolf_pelt": ("Wolfsfell", "R", 1), "item_wolf_fang": ("Wolfszahn", "W", 1),
    "item_seal_pelt": ("Robbenfell", "R", 2), "item_seal_blubber": ("Tran", "V", 2),
    "item_feather": ("Federn", "B", 2), "item_seabird_egg": ("Seevogel-Ei", "E", 2),
    "item_elk_antler": ("Elchschaufel", "W", 3), "item_bear_pelt": ("Bärenfell", "R", 3),
    "item_bear_fat": ("Bärenfett", "V", 3), "item_lynx_pelt": ("Luchsfell", "R", 4),
    "item_reindeer_pelt": ("Rentierfell", "R", 4), "item_reindeer_antler": ("Rentiergeweih", "W", 4),
    "item_arctic_fox_pelt": ("Polarfuchsfell", "R", 6), "item_meat_raw": ("Rohes Fleisch", "E", 1),
}
SIN, BON, HID = ("gen", "sinew"), ("gen", "bone"), ("gen", "hide")
MEAT = "item_meat_raw"
# id, Anzeigename, Heimat-Rang, Biome, LP, Haltung, Schaden, Temperament, Rudel, Spawn-Gewicht, Beute
ANIMALS = [
    ("animal_hare", "Hase", 1, ["fjord_meadow", "barrow_heath"], 30, 5, 0, "passive", False, 1.5,
     [("item_hare_pelt", (1, 1), 0.8), (MEAT, (1, 2), 0.9)]),
    ("animal_fox", "Rotfuchs", 1, ["fjord_meadow"], 50, 8, 4, "passive", False, 0.8,
     [("item_fox_pelt", (1, 1), 0.8)]),
    ("animal_deer", "Rothirsch", 1, ["fjord_meadow", "barrow_heath"], 140, 20, 7, "passive", False, 1.0,
     [("item_deer_antler", (1, 1), 0.6), (HID, (1, 2), 0.8), (SIN, (1, 2), 0.7), (BON, (1, 2), 0.65), (MEAT, (2, 4), 0.9)]),
    ("animal_wolf", "Wolf", 1, ["fjord_meadow", "barrow_heath", "basalt_fell"], 140, 18, 8, "hostile", True, 0.8,
     [("item_wolf_pelt", (1, 2), 0.85), ("item_wolf_fang", (1, 2), 0.65), (SIN, (1, 2), 0.6), (BON, (1, 1), 0.5)]),
    ("animal_boar", "Wildschwein", 1, ["fjord_meadow"], 220, 35, 11, "defensive", False, 1.0,
     [("item_boar_tusk", (1, 2), 0.75), (HID, (1, 2), 0.8), (SIN, (1, 2), 0.7), (BON, (1, 2), 0.65), (MEAT, (2, 4), 0.9)]),
    ("animal_seal", "Robbe", 2, ["tidal_marsh"], 200, 25, 6, "passive", False, 1.0,
     [("item_seal_pelt", (1, 2), 0.8), ("item_seal_blubber", (1, 2), 0.7), (BON, (1, 1), 0.5), (MEAT, (2, 3), 0.9)]),
    ("animal_seabird", "Seevögel", 2, ["tidal_marsh"], 15, 0, 0, "passive", False, 1.5,
     [("item_feather", (2, 4), 0.9), ("item_seabird_egg", (1, 2), 0.5)]),
    ("animal_elk", "Elch", 3, ["barrow_heath"], 650, 80, 20, "defensive", False, 0.7,
     [("item_elk_antler", (1, 2), 0.65), (HID, (1, 2), 0.8), (SIN, (1, 2), 0.7), (BON, (1, 2), 0.65), (MEAT, (3, 5), 0.9)]),
    ("animal_bear", "Braunbär", 3, ["barrow_heath", "basalt_fell"], 800, 95, 24, "territorial", False, 0.5,
     [("item_bear_pelt", (1, 1), 0.85), ("item_bear_fat", (1, 2), 0.7), (SIN, (1, 2), 0.7), (BON, (1, 2), 0.65), (MEAT, (3, 5), 0.9)]),
    ("animal_lynx", "Luchs", 4, ["basalt_fell"], 600, 60, 22, "ambush", False, 0.6,
     [("item_lynx_pelt", (1, 1), 0.8), (SIN, (1, 1), 0.6)]),
    ("animal_reindeer", "Rentier", 4, ["basalt_fell", "ice_rim"], 700, 70, 18, "passive", False, 1.0,
     [("item_reindeer_pelt", (1, 2), 0.8), ("item_reindeer_antler", (1, 2), 0.6), (SIN, (1, 2), 0.7), (BON, (1, 2), 0.65),
      (MEAT, (2, 4), 0.9)]),
    ("animal_arctic_fox", "Polarfuchs", 6, ["ice_rim"], 300, 20, 12, "passive", False, 0.8,
     [("item_arctic_fox_pelt", (1, 1), 0.8)]),
]
# Temperamente der Kreaturen: Standard feindlich; Ausnahmen (Vorschlag von Claude, aus den Beschreibungen)
TEMPERAMENT_OVERRIDE = {35: "ambush", 36: "ambush", 40: "ambush", 41: "territorial", 21: "territorial", 47: "territorial"}


def family_of(pattern: str) -> str:
    return pattern.replace("_t{}", "")


def r4(x: float) -> float:
    return round(x, 4)


def write_table(path: Path, rows: list, header: dict | None = None) -> None:
    """Schreibt { "schema": 1, ..., "rows": [ eine Zeile je Eintrag ] } (lesbar und gut vergleichbar in Git)."""
    lines = ["{", '  "schema": 1,']
    for key, value in (header or {}).items():
        dumped = json.dumps(value, ensure_ascii=False, indent=2).replace("\n", "\n  ")
        lines.append(f"  {json.dumps(key)}: {dumped},")
    lines.append('  "rows": [')
    lines.append(",\n".join("    " + json.dumps(r, ensure_ascii=False, separators=(", ", ": ")) for r in rows))
    lines.append("  ]")
    lines.append("}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def write_json(ctx: dict) -> None:
    values, clean, home = ctx["values"], ctx["clean"], ctx["home"]
    tier_of = ctx["tier_of"]
    biome_id = {key: spec[0] for key, spec in BIOMES.items()}
    existing = {r["id"] for r in json.loads((JSON_DIR / "items.json").read_text(encoding="utf-8"))["rows"]}

    # --- Gegenstände ------------------------------------------------------------------------------------------
    weight = {"R": 1.0, "W": 0.5, "B": 0.2, "V": 0.3, "E": 0.2, "X": 1.0}
    stack = {"R": 20, "W": 20, "B": 50, "V": 20, "E": 20, "X": 20}
    item_rows: list[dict] = []

    def add_item(item_id: str, name: str, kind: str, tier: int) -> None:
        if item_id in existing:
            return
        existing.add(item_id)
        item_rows.append({"id": item_id, "name": name, "category": "material", "tier": tier,
                          "max_stack": stack[kind], "weight_kg": weight[kind]})

    # Allgemeine Teile nur für Stufen, die wirklich fallen (sonst meldet die Erreichbarkeitsprüfung sie als Fehler)
    needed_tiers: dict[str, set[int]] = {fam: set(ranks) for fam, ranks in ctx["generic_ranks"].items()}
    for _aid, _n, home_rank, biome_keys, _hp, _po, _dm, _t, _pk, _w, parts in ANIMALS:
        for part, _count, _chance in parts:
            if isinstance(part, tuple):
                needed_tiers.setdefault(part[1], set()).update(r for k in biome_keys for r in (BIOMES[k][2], min(6, BIOMES[k][2] + 1)) if r >= home_rank)
    for fam, (pattern, names, kind, _title) in GENERIC.items():
        for tier in sorted(needed_tiers.get(fam, ())):
            add_item(pattern.format(tier), names[tier - 1], kind, tier)
    for item_id, (name, kind, tier, _note) in sorted(FIXED.items()):
        add_item(item_id, name, kind, tier)
    for item_id in sorted(ctx["named_used"]):
        name, kind = ITEMS[item_id]
        add_item(item_id, name, kind, tier_of[item_id])
    for item_id, (name, kind, tier) in sorted(ANIMAL_PARTS.items()):
        add_item(item_id, name, kind, tier)
    write_table(JSON_DIR / "items_creature.json", item_rows,
                {"_comment": "Beute-Gegenstände der Kreaturen und Tiere. Erzeugt von tools/data/creatures_from_gemini.py, danach von Hand pflegbar. Icons und Modelle kommen mit den Wellen."})

    # --- Biome und Spawn -----------------------------------------------------------------------------------------
    biomes = [{"id": spec[0], "name": spec[1], "rank": spec[2], "place_rank": min(6, spec[2] + 1), "places": spec[3]}
              for spec in BIOMES.values()]
    spawn_rows: list[dict] = []
    for key in BIOME_ORDER:
        for nr, weight_value, behaviour, only_places in SPAWNS[key]:
            row = {"biome": biome_id[key], "creature": CREATURES[nr][0], "weight": r4(weight_value), "only_places": only_places}
            if behaviour[0] == "mit":
                row.update(scale_to_zone=True, scale_chance=r4(behaviour[1]), max_rank=0, fixed_rank=0)
            elif behaviour[0] == "bis":
                row.update(scale_to_zone=True, scale_chance=r4(behaviour[2]), max_rank=behaviour[1], fixed_rank=0)
            else:
                row.update(scale_to_zone=False, scale_chance=0.0, max_rank=behaviour[1], fixed_rank=behaviour[1])
            spawn_rows.append(row)
    for animal in ANIMALS:
        for key in animal[3]:
            spawn_rows.append({"biome": biome_id[key], "creature": animal[0], "weight": r4(animal[9]), "only_places": False,
                               "scale_to_zone": True, "scale_chance": 1.0, "max_rank": 0, "fixed_rank": 0})
    write_table(JSON_DIR / "spawns.json", spawn_rows,
                {"_comment": "Spawn-Regeln je Biom (docs/21, Abschnitt 6). Tiere wachsen mit dem Biom-Rang.", "biomes": biomes})

    # --- Kreaturen und Tiere ----------------------------------------------------------------------------------------
    creature_rows: list[dict] = []
    for nr in range(1, 49):
        cid, _gemini, role = CREATURES[nr]
        creature_rows.append({
            "id": cid, "name": DISPLAY[nr], "gemini_name": CREATURES[nr][1], "role": role, "kind": "creature",
            "temperament": TEMPERAMENT_OVERRIDE.get(nr, "hostile"), "min_rank": values[nr][1],
            "home_biome": biome_id[home[nr]], "wave": WAVE_OF_BIOME[home[nr]],
            "ranks": [{"rank": rank, "hp": v[0], "poise": v[1], "dmg": v[2]} for rank, v in sorted(values[nr][2].items())]})
    write_table(JSON_DIR / "creatures.json", creature_rows,
                {"_comment": "48 Kreaturen. Werte sind Geminis Startwerte (docs/21, docs/22); Anzeigenamen sind ein Vorschlag."})

    animal_rows: list[dict] = []
    for aid, name, home_rank, biome_keys, hp, poise, dmg, temperament, pack, _w, _loot in ANIMALS:
        # Biom-Rang und Rang der gefährlichen Orte (Biom-Rang + 1, höchstens 6)
        ranks = sorted({r for k in biome_keys for r in (BIOMES[k][2], min(6, BIOMES[k][2] + 1))})
        first = biome_keys[0]
        animal_rows.append({
            "id": aid, "name": name, "kind": "animal", "temperament": temperament, "pack": pack, "min_rank": home_rank,
            "home_biome": biome_id[first], "wave": WAVE_OF_BIOME.get(first, 0),
            "ranks": [{"rank": r, "hp": round(hp * 1.4 ** (r - home_rank)), "poise": round(poise * 1.25 ** (r - home_rank)),
                       "dmg": round(dmg * 1.25 ** (r - home_rank))} for r in ranks if r >= home_rank]})
    write_table(JSON_DIR / "animals.json", animal_rows,
                {"_comment": "12 Tiere (docs/21, Abschnitt 8). In höheren Biomen pro Rang LP x 1,4, Haltung x 1,25, Schaden x 1,25."})

    # --- Beute -----------------------------------------------------------------------------------------------------------
    def entry(row: dict) -> dict:
        out: dict = {}
        if "generic" in row:
            out["item_family"] = family_of(GENERIC[row["generic"]][0])
        else:
            out["item"] = row["item"]
        out.update({"min": row["count"][0], "max": row["count"][1], "chance": r4(row["chance"])})
        if row["kind"] == "biome":
            out["biomes"] = [biome_id[b] for b in row["biomes"]]
        else:
            out["min_rank"] = row["min_rank"]
            out["scale_count"] = bool(row["scale_count"])
        return out

    loot_rows: list[dict] = []
    for nr in range(1, 49):
        rows = clean[nr]
        loot_rows.append({"id": CREATURES[nr][0],
                          "base": [entry(r) for r in rows if r["kind"] == "base"],
                          "rank": [entry(r) for r in rows if r["kind"] == "rank"],
                          "biome": [entry(r) for r in rows if r["kind"] == "biome"]})
    for aid, _n, home_rank, _bk, _hp, _po, _dm, _t, _pk, _w, parts in ANIMALS:
        base = []
        for part, count, chance in parts:
            row = {"min": count[0], "max": count[1], "chance": chance, "min_rank": home_rank, "scale_count": False}
            if isinstance(part, tuple):
                row = {"item_family": family_of(GENERIC[part[1]][0]), **row}
            else:
                row = {"item": part, **row}
            base.append(row)
        loot_rows.append({"id": aid, "base": base, "rank": [], "biome": []})
    write_table(JSON_DIR / "creature_loot.json", loot_rows,
                {"_comment": "Beute je Kreatur und Tier. item_family = Teil je Stufe (Stufe = Rang beim Tod, z. B. item_sinew_t3)."})

    # --- Fortschritt ------------------------------------------------------------------------------------------------------
    per_hour = typical_hour_xp_per_tier()
    # Kurve: Summe je Stufe = Stunden x Erfahrung/Stunde x Stufe. Innerhalb der Stufen läuft der Bedarf je Level linear zwischen den
    # Mittelpunkten der Stufen (kein Sprung nach unten an den Grenzen), dann wird jede Stufe auf ihre Summe skaliert.
    spans = [(lo, min(hi, 49)) for lo, hi in TIER_LEVELS]
    totals = [hours * per_hour * tier for tier, hours in enumerate(TIER_HOURS, start=1)]
    centers = [(lo + hi) / 2 for lo, hi in spans]
    averages = [total / (hi - lo + 1) for total, (lo, hi) in zip(totals, spans)]

    def interp(level: float) -> float:
        for i in range(len(centers) - 1):
            if level <= centers[i + 1] or i == len(centers) - 2:
                x0, x1, y0, y1 = centers[i], centers[i + 1], averages[i], averages[i + 1]
                return y0 + (y1 - y0) * (level - x0) / (x1 - x0)
        return averages[-1]

    xp_to_next: dict[int, int] = {}
    for (lo, hi), total in zip(spans, totals):
        raw = {lv: interp(lv) for lv in range(lo, hi + 1)}
        factor = total / sum(raw.values())
        for lv, value in raw.items():
            xp_to_next[lv] = int(round(value * factor / 5.0)) * 5
    ordered = [xp_to_next[lv] for lv in range(1, 50)]
    if any(b <= a for a, b in zip(ordered, ordered[1:])):
        raise SystemExit("Erfahrungskurve steigt nicht streng: " + str(ordered))
    tier_values = [((14, 24), (12, 19), (15, 37), (20, 35), 0.08), ((26, 39), (21, 30), (41, 66), (37, 53), 0.16),
                   ((41, 60), (32, 45), (71, 110), (56, 80), 0.26), ((61, 86), (46, 62), (111, 156), (81, 108), 0.36),
                   ((89, 111), (64, 79), (162, 199), (112, 136), 0.46), ((115, 145), (83, 105), (208, 260), (142, 175), 0.55)]
    tiers = []
    for tier, ((lo, hi), (wd, wp, ar, pr, rc)) in enumerate(zip(TIER_LEVELS, tier_values), start=1):
        tiers.append({"tier": tier, "biome": list(BIOMES.values())[tier - 1][0], "level_min": lo, "level_max": hi,
                      "hours": TIER_HOURS[tier - 1], "weapon_damage": list(wd), "weapon_poise": list(wp), "armor": list(ar),
                      "poise_resist": list(pr), "resist_cap": rc})
    progression = {
        "schema": 1,
        "_comment": "Level 1-50 (Entscheidung 2026-11-22). Alle Zahlen sind Startwerte. xp_to_next[i] = Erfahrung von Level i+1 zu Level i+2 (Index 0 = Level 1). Aus Stunden je Stufe und der typischen Stunde berechnet (tools/data/creatures_from_gemini.py).",
        "max_level": 50,
        "tiers": tiers,
        "xp_to_next": [xp_to_next[lv] for lv in range(1, 50)],
        "skill_points": {"per_level": 1, "bonus_every": 5, "bonus": 1},
        "xp_sources": {"kill_per_rank": 10, "leader_multipliers": [1.5, 2.0, 3.0], "boss_first_win_per_tier": 300,
                       "quest_stage_per_tier": 50, "discovery_per_tier": 25, "first_craft_per_tier": 20,
                       "repeat_craft_per_tier": 1, "build_piece_per_tier": 2, "gather_per_tier": 0.5},
        "coop": {"radius_cm": 5000, "share_combat": 1.0, "share_work": 0.25},
        "under_level_factor": [1.0, 0.75, 0.25, 0.1],
        "typical_hour": {**TYPICAL_HOUR, "xp_per_tier": per_hour},
    }
    (JSON_DIR / "progression.json").write_text(json.dumps(progression, ensure_ascii=False, indent=2) + "\n", encoding="utf-8", newline="\n")

    # --- Kampf ---------------------------------------------------------------------------------------------------------------------------
    combat = {
        "schema": 1,
        "_comment": "Kampf-Konstanten (docs/21). Alle Zahlen sind Startwerte; creature_hp_scale regelt alle Kreaturen auf einmal.",
        "armor": {"k": 160.0, "max_reduction": 0.75, "min_damage": 1.0},
        "resist": {"min": -0.25, "max": 0.60},
        "poise": {"k_player": 90.0, "stagger_s": 2.5, "boss_stagger_s": 3.0, "stagger_damage_mult": 1.5, "decay_delay_s": 3.0,
                  "decay_fraction_per_s": 0.25, "player_max": 100.0, "player_stagger_s": 0.8, "player_regen_delay_s": 2.0,
                  "player_regen_per_s": 30.0},
        "slots": {"helmet": 0.20, "shoulder": 0.10, "chest": 0.40, "hands": 0.10, "legs": 0.15, "feet": 0.05},
        "zones": {"head": {"slots": ["helmet"], "damage_factor": 1.5},
                  "body": {"slots": ["shoulder", "chest"], "damage_factor": 1.0},
                  "limbs": {"slots": ["hands", "legs", "feet"], "damage_factor": 0.7}},
        "attacks": [{"id": "light", "damage": 1.0, "poise": 1.0}, {"id": "thrust", "damage": 0.9, "poise": 0.8},
                    {"id": "heavy", "damage": 1.4, "poise": 1.6}, {"id": "shield_bash", "damage": 0.0, "poise": 2.0},
                    {"id": "riposte", "damage": 1.2, "poise": 1.5}],
        "weak_point_poise_mult": 1.5,
        "spread": {"min": 0.85, "max": 1.15, "size_follow": 0.5},
        "leaders": [
            {"id": "experienced", "min_rank": 2, "chance": 0.05, "place_chance": 0.10, "hp": 1.13, "poise": 1.07, "dmg": 1.09,
             "size": 1.05, "extra_base_rolls": 1, "extra_rank_rolls": 0, "rank_roll_mult": 1},
            {"id": "veteran", "min_rank": 4, "chance": 0.03, "place_chance": 0.06, "hp": 1.25, "poise": 1.14, "dmg": 1.18,
             "size": 1.10, "extra_base_rolls": 1, "extra_rank_rolls": 1, "rank_roll_mult": 1},
            {"id": "leader", "min_rank": 6, "chance": 0.02, "place_chance": 0.04, "hp": 1.35, "poise": 1.20, "dmg": 1.25,
             "size": 1.15, "extra_base_rolls": 1, "extra_rank_rolls": 1, "rank_roll_mult": 2}],
        "loot": {"count_bonus_cap": 2, "downscale": {"delta1": 0.85, "delta2": 0.70, "delta3": 0.55}},
        "creature_hp_scale": 1.0,
        "player_hp": {"base": 100.0, "per_toughness": 5.0, "toughness_neutral": 5},
    }
    (JSON_DIR / "combat.json").write_text(json.dumps(combat, ensure_ascii=False, indent=2) + "\n", encoding="utf-8", newline="\n")
    print(f"JSON: Gegenstände {len(item_rows)}, Spawn {len(spawn_rows)}, Kreaturen {len(creature_rows)}, Tiere {len(animal_rows)}, "
          f"Beute-Zeilen {len(loot_rows)}, Erfahrung bis Level 50: {sum(xp_to_next.values())}")


if __name__ == "__main__":
    context = main()
    if "--json" in sys.argv:
        write_json(context)
