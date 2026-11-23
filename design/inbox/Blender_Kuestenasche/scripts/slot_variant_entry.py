"""Small shared launcher imported by the 18 individual slot scripts."""
from slot_variants_03_08 import generate_variant


def run(variant_id):
    return generate_variant(variant_id)
