"""
Looks of the MetaHuman figures (shared by metahuman_variants.py and metahuman_assemble.py).
Style: design/stil_norden_gezeitenhall.md (braids, beards, weathered skin; hair blond, red, brown or grey).
Seeress = npc_seeress from docs/14_saga_quests.md. Clothing: later, own assets (design/kleidung_kandidaten_kostenlos.md).
"""
ROOT = '/Game/Gamer/Characters/MetaHuman'
GROOMS = '/MetaHumanCharacter/Optional/Grooms/Bindings'
FOLDER = {'Hair': 'Hair', 'Eyebrows': 'Eyebrows', 'Beard': 'Beards', 'Mustache': 'Mustaches'}

# Hair colours (melanin 0 = blond .. 1 = black, redness 0..1, whiteness/grey 0..1). Unknown names are skipped.
BLOND = {'Melanin': 0.18, 'Redness': 0.22}
RED = {'Melanin': 0.35, 'Redness': 0.85}
BROWN = {'Melanin': 0.55, 'Redness': 0.35}
DARK = {'Melanin': 0.8, 'Redness': 0.2}
GREY = {'Melanin': 0.4, 'Redness': 0.1, 'Whiteness': 0.75}

# name: (base figure for a copy or None, grooms, hair colour, skin (u, v) or None, quality)
# Body/height is not changed: a body change after auto-rig may invalidate the face rig.
# Matched to the user's reference sheet (2026-11-16, see design/stil_norden_gezeitenhall.md, "Referenzbild Figuren"):
# women with long braids (blond, red), men with tied-back hair / undercut / long hair and full beards,
# one grey elder, mostly light weathered skin, one darker.
FIGURES = {
    'MHC_Kraeftig': (None, {'Hair': 'WI_Hair_M_Layered', 'Beard': 'WI_Beard_L_Full',
                            'Eyebrows': 'WI_Eyebrows_M_Thick'}, BLOND, None, 'Both'),
    'MHC_Schmal': (None, {'Hair': 'WI_Hair_M_TwistedBraids', 'Eyebrows': 'WI_Eyebrows_M_Natural'}, BLOND, None, 'Both'),
    'MHC_Bewohner_K1': ('MHC_Kraeftig', {'Hair': 'WI_Hair_M_FauxMohawk', 'Beard': 'WI_Beard_M_Curly',
                                         'Eyebrows': 'WI_Eyebrows_M_Full'}, RED, (0.34, 0.58), 'Low'),
    'MHC_Bewohner_K2': ('MHC_Kraeftig', {'Hair': 'WI_Hair_L_Straight', 'Beard': 'WI_Beard_L_Full',
                                         'Eyebrows': 'WI_Eyebrows_L_Scraggly'}, GREY, (0.40, 0.55), 'Low'),
    'MHC_Bewohner_K3': ('MHC_Kraeftig', {'Hair': 'WI_Hair_S_PulledBack', 'Beard': 'WI_Beard_S_Full',
                                         'Eyebrows': 'WI_Eyebrows_M_Unruly'}, DARK, (0.66, 0.52), 'Low'),
    'MHC_Bewohner_S1': ('MHC_Schmal', {'Hair': 'WI_Hair_S_UpdoBraids', 'Eyebrows': 'WI_Eyebrows_M_Fine'},
                        BLOND, (0.30, 0.55), 'Low'),
    'MHC_Bewohner_S2': ('MHC_Schmal', {'Hair': 'WI_Hair_L_MessyClumps', 'Eyebrows': 'WI_Eyebrows_M_Natural'},
                        RED, (0.28, 0.60), 'Low'),
    'MHC_Bewohner_S3': ('MHC_Schmal', {'Hair': 'WI_Hair_S_LowPonytail', 'Eyebrows': 'WI_Eyebrows_M_Shaded'},
                        BROWN, (0.38, 0.53), 'Low'),
    # Second round after the user's reference sheets design/referenz/figuren/*.jpg: more skin tones and braids.
    'MHC_Bewohner_K4': ('MHC_Kraeftig', {'Hair': 'WI_Hair_S_Cornrows', 'Beard': 'WI_Beard_L_Full',
                                         'Eyebrows': 'WI_Eyebrows_M_Thick'}, DARK, (0.86, 0.48), 'Low'),
    'MHC_Bewohner_K5': ('MHC_Kraeftig', {'Hair': 'WI_Hair_M_TwistedBraids', 'Beard': 'WI_Beard_L_Messy',
                                         'Eyebrows': 'WI_Eyebrows_M_Full'}, BLOND, (0.32, 0.56), 'Low'),
    'MHC_Bewohner_S4': ('MHC_Schmal', {'Hair': 'WI_Hair_S_Cornrows', 'Eyebrows': 'WI_Eyebrows_M_Natural'},
                        DARK, (0.88, 0.50), 'Low'),
    'MHC_Bewohner_S5': ('MHC_Schmal', {'Hair': 'WI_Hair_M_TwistedBraids', 'Eyebrows': 'WI_Eyebrows_M_Fine'},
                        DARK, (0.62, 0.52), 'Low'),
    'MHC_Bewohner_S6': ('MHC_Schmal', {'Hair': 'WI_Hair_S_Updo', 'Eyebrows': 'WI_Eyebrows_S_Shaded'},
                        GREY, (0.40, 0.55), 'Low'),
    'MHC_Seherin': ('MHC_Schmal', {'Hair': 'WI_Hair_L_Straight', 'Eyebrows': 'WI_Eyebrows_S_Shaded'},
                    GREY, (0.30, 0.50), 'Low'),
    # Hair kits for the character menu (look_hair / look_beard, characters.json): copies of the player figures with
    # other grooms, assembled in Medium quality (simulated cards). Only their grooms are used (GamerFigures picks
    # them by look option, see HAIR_KITS); same face topology, so the bindings fit the player face.
    'MHC_Kraeftig_HK1': ('MHC_Kraeftig', {'Hair': 'WI_Hair_S_Messy', 'Beard': 'WI_Beard_S_Stubble',
                                          'Eyebrows': 'WI_Eyebrows_M_Thick'}, BLOND, None, 'Mid'),
    'MHC_Kraeftig_HK2': ('MHC_Kraeftig', {'Hair': 'WI_Hair_L_MessyClumps', 'Beard': 'WI_Beard_L_Messy',
                                          'Eyebrows': 'WI_Eyebrows_M_Thick'}, BLOND, None, 'Mid'),
    'MHC_Kraeftig_HK3': ('MHC_Kraeftig', {'Hair': 'WI_Hair_M_TwistedBraids', 'Eyebrows': 'WI_Eyebrows_M_Thick'},
                         BLOND, None, 'Mid'),
    'MHC_Kraeftig_HK4': ('MHC_Kraeftig', {'Hair': 'WI_Hair_M_FauxMohawk', 'Eyebrows': 'WI_Eyebrows_M_Thick'},
                         BLOND, None, 'Mid'),
    'MHC_Schmal_HK1': ('MHC_Schmal', {'Hair': 'WI_Hair_S_Messy', 'Beard': 'WI_Beard_S_Stubble',
                                      'Eyebrows': 'WI_Eyebrows_M_Natural'}, BLOND, None, 'Mid'),
    'MHC_Schmal_HK2': ('MHC_Schmal', {'Hair': 'WI_Hair_L_MessyClumps', 'Beard': 'WI_Beard_L_Messy',
                                      'Eyebrows': 'WI_Eyebrows_M_Natural'}, BLOND, None, 'Mid'),
    'MHC_Schmal_HK3': ('MHC_Schmal', {'Hair': 'WI_Hair_M_FauxMohawk', 'Beard': 'WI_Beard_L_Full',
                                      'Eyebrows': 'WI_Eyebrows_M_Natural'}, BLOND, None, 'Mid'),
}

# look option (characters.json) -> wardrobe groom used for it. hair_none / beard_none: no groom.
# beard_braided: MetaHuman has no braided beard; the long messy beard stands in for it.
HAIR_KITS = {
    'hair_short': 'Hair_S_Messy', 'hair_long': 'Hair_L_MessyClumps', 'hair_braids': 'Hair_M_TwistedBraids',
    'hair_undercut': 'Hair_M_FauxMohawk',
    'beard_stubble': 'Beard_S_Stubble', 'beard_full': 'Beard_L_Full', 'beard_braided': 'Beard_L_Messy',
}


def set_hair_colour(unreal, character, log):
    """Sets the hair colour parameters on all groom slots. Needs a built collection (after build_meta_human).
    Returns the number of parameters set."""
    look = FIGURES.get(character.get_name())
    if not look:
        return 0
    colour = look[2]
    instance = character.get_editor_property('internal_collection').get_editor_property('default_instance')
    count = 0
    for data in instance.get_slot_selection_data():
        selection = data.get_editor_property('selection')
        key = selection.get_editor_property('selected_item')
        params = instance.get_instance_parameters(item_path=unreal.MetaHumanPaletteItemPath(item_key=key))
        names = [str(p.name) for p in params]
        done = []
        for p in params:
            if str(p.name) in colour:
                p.set_float(value=colour[str(p.name)])
                done.append(str(p.name))
        count += len(done)
        log('%s: %s params [%s], set [%s]' % (character.get_name(), selection.get_editor_property('slot_name'),
                                              ', '.join(names), ', '.join(done)))
    return count
