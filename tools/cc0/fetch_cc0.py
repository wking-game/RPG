"""
Download the CC0 source assets for the photorealistic vegetation (decision 2026-11-13) from Poly Haven and
ambientCG into D:/gamer/cc0_quellen/<source>/<id>/ (outside the repository; one empty folder per package).
Only the processed models and textures go into the repository (Assets/Nature, via Git LFS).

Both sites license everything as CC0 1.0 (https://polyhaven.com/license, https://docs.ambientcg.com/license/).
Every downloaded file is listed with its URL, size and the licence in sources.json in the target folder
(needed for design/asset_log.md). Poly Haven files are checked against their MD5; ambientCG zips are
extracted with a path check. Packages that are already complete are skipped.

Usage (from the repository root):  python -I tools/cc0/fetch_cc0.py [--out <folder>] [--dry-run]
"""
import argparse
import datetime
import hashlib
import json
import os
import sys
import urllib.request
import zipfile

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
DEFAULT_OUT = os.path.abspath(os.path.join(REPO, '..', 'cc0_quellen'))
AGENT = 'gamer-asset-pipeline/1.0'
MAX_TOTAL_MB = 140   # per run: the second approved list is about 121 MB; stop if the sites report much more

# Map sets for Poly Haven textures. 'ground' = for the landscape material (DirectX normal, packed ARM, height).
GROUND_MAPS = ('Diffuse', 'nor_dx', 'arm', 'Displacement')
TWIG_MAPS = ('twig_diff', 'twig_alpha', 'twig_nor_dx', 'twig_arm')

# (source, kind, id, resolution, purpose[, maps]) - first list approved by the user on 2026-11-13 (about 61 MB)
PACKAGES = [
    ('polyhaven', 'model', 'rock_moss_set_01', '1k', 'small and medium rocks, loose stones'),
    ('polyhaven', 'model', 'rock_09', '1k', 'large boulder'),
    ('polyhaven', 'model', 'dry_branches_medium_01', '1k', 'fallen branches'),
    ('polyhaven', 'model', 'nettle_plant', '1k', 'stinging nettle (fiber plant)'),
    ('polyhaven', 'texture', 'pine_bark', '2k', 'Scots pine bark'),
    ('polyhaven', 'texture', 'bark_brown_02', '2k', 'bark for apple tree and hazel'),
    ('ambientcg', 'atlas', 'LeafSet010', '2K-JPG', 'broad leaves with opacity'),
    ('ambientcg', 'atlas', 'LeafSet019', '2K-JPG', 'pine needles with opacity'),
    ('ambientcg', 'atlas', 'Foliage006', '2K-JPG', 'grass blades with opacity'),
    # Second list, approved by the user on 2026-11-13 (about 121 MB): ground, dead wood, undergrowth, sandstone,
    # pine twig atlas for the Blender pines. Chosen from the reference image (light Scots pine forest on sand).
    ('ambientcg', 'atlas', 'Ground033', '2K-JPG', 'light beach/coast sand (ground layer)'),
    ('polyhaven', 'texture', 'forrest_sand_01', '2k', 'sand with pine needles (ground layer)', GROUND_MAPS),
    ('polyhaven', 'texture', 'forrest_ground_03', '2k', 'forest floor with pine needle litter (ground layer)', GROUND_MAPS),
    ('polyhaven', 'texture', 'forest_leaves_02', '2k', 'mossy forest floor (ground layer)', GROUND_MAPS),
    ('polyhaven', 'texture', 'dirt_floor', '2k', 'dirt / path (ground layer)', GROUND_MAPS),
    ('polyhaven', 'model', 'tree_stump_01', '2k', 'dead stump with roots'),
    ('polyhaven', 'model', 'dead_tree_trunk', '2k', 'fallen dead trunk'),
    ('polyhaven', 'model', 'fern_02', '2k', 'ferns'),
    ('polyhaven', 'model', 'shrub_02', '2k', 'low green shrub / ground cover'),
    ('polyhaven', 'model', 'shrub_04', '2k', 'shrub twigs with leaves'),
    ('polyhaven', 'model', 'rock_face_02', '2k', 'layered sandstone cliff'),
    ('polyhaven', 'maps', 'pine_tree_01', '2k', 'pine twig atlas only (needles) for the Blender pines', TWIG_MAPS),
    # Third list, approved by the user on 2026-11-14 (46.6 MB): building materials for the Aska-like style test.
    ('polyhaven', 'texture', 'weathered_planks', '2k', 'weathered wooden planks (walls, floors)', GROUND_MAPS),
    ('polyhaven', 'texture', 'wood_planks', '2k', 'wooden planks (doors, furniture)', GROUND_MAPS),
    ('polyhaven', 'texture', 'thatch_roof_angled', '2k', 'thatched roof', GROUND_MAPS),
    ('polyhaven', 'texture', 'stone_wall', '2k', 'stone wall / foundation', GROUND_MAPS),
    ('polyhaven', 'texture', 'clay_plaster', '2k', 'clay plaster (wattle and daub)', GROUND_MAPS),
]
TEXTURE_MAPS = ('Diffuse', 'nor_gl', 'Rough', 'AO')
LICENSES = {'polyhaven': 'CC0 1.0 (https://polyhaven.com/license)',
            'ambientcg': 'CC0 1.0 (https://docs.ambientcg.com/license/)'}


def get_json(url):
    req = urllib.request.Request(url, headers={'User-Agent': AGENT})
    with urllib.request.urlopen(req, timeout=60) as response:
        return json.loads(response.read().decode('utf-8'))


def download(url, path, md5=None):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    req = urllib.request.Request(url, headers={'User-Agent': AGENT})
    digest = hashlib.md5()
    tmp = path + '.part'
    with urllib.request.urlopen(req, timeout=300) as response, open(tmp, 'wb') as handle:
        while True:
            chunk = response.read(1 << 20)
            if not chunk:
                break
            digest.update(chunk)
            handle.write(chunk)
    if md5 and digest.hexdigest() != md5:
        os.remove(tmp)
        raise RuntimeError('MD5 mismatch for ' + url)
    os.replace(tmp, path)
    return os.path.getsize(path)


def safe_path(base, relative):
    target = os.path.abspath(os.path.join(base, relative))
    if not target.startswith(os.path.abspath(base) + os.sep):
        raise RuntimeError('unsafe path in package: ' + relative)
    return target


def polyhaven_files(kind, asset_id, resolution, maps=None):
    """[(relative path, url, size, md5)] for one Poly Haven asset. kind 'maps' = only the named texture maps."""
    files = get_json('https://api.polyhaven.com/files/' + asset_id)
    result = []
    if maps or kind == 'maps':
        for name in maps:
            entry = files.get(name, {}).get(resolution, {}).get('jpg')
            if not entry:
                raise RuntimeError('%s: no %s %s jpg' % (asset_id, name, resolution))
            result.append((os.path.basename(entry['url'].split('?')[0]), entry['url'], entry['size'], entry.get('md5')))
    elif kind == 'model':
        entry = files['gltf'][resolution]['gltf']
        result.append((asset_id + '.gltf', entry['url'], entry['size'], entry.get('md5')))
        for relative, include in entry.get('include', {}).items():
            result.append((relative, include['url'], include['size'], include.get('md5')))
        # Leaf cards (e.g. the nettle) mask with an alpha map that the glTF package does not include.
        alpha = files.get('Alpha', {}).get(resolution, {}).get('png')
        if alpha:
            result.append(('textures/' + os.path.basename(alpha['url']), alpha['url'], alpha['size'], alpha.get('md5')))
    else:
        for name in TEXTURE_MAPS:
            entry = files.get(name, {}).get(resolution, {}).get('jpg')
            if entry:
                result.append((os.path.basename(entry['url'].split('?')[0]), entry['url'], entry['size'], entry.get('md5')))
    return result


def ambientcg_files(asset_id, resolution):
    name = '%s_%s.zip' % (asset_id, resolution)
    return [(name, 'https://ambientcg.com/get?file=' + name, None, None)]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--out', default=DEFAULT_OUT)
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()

    record_path = os.path.join(args.out, 'sources.json')
    records = []
    if os.path.exists(record_path):
        with open(record_path, encoding='utf-8') as handle:
            records = json.load(handle)
    done = {(r['source'], r['id']) for r in records if r.get('complete')}

    plan, total = [], 0
    for package in PACKAGES:
        source, kind, asset_id, resolution, purpose = package[:5]
        maps = package[5] if len(package) > 5 else None
        files = polyhaven_files(kind, asset_id, resolution, maps) if source == 'polyhaven' else ambientcg_files(asset_id, resolution)
        folder = os.path.join(args.out, source, asset_id)
        # Only what is missing (or has another size); zips count as present once the package is complete.
        files = [f for f in files if not (os.path.exists(os.path.join(folder, f[0]))
                                          and (f[2] is None or os.path.getsize(os.path.join(folder, f[0])) == f[2]))]
        if not files and (source, asset_id) in done:
            print('skip (complete): %s/%s' % (source, asset_id))
            continue
        size = sum(f[2] or 0 for f in files)
        total += size
        plan.append((source, kind, asset_id, resolution, purpose, files))
        print('%-10s %-24s %-7s %3d file(s) %6.1f MB%s' % (source, asset_id, resolution, len(files), size / 1e6,
                                                          '' if source == 'polyhaven' else ' (zip, size from download)'))
    print('total known: %.1f MB, target: %s' % (total / 1e6, args.out))
    if total / 1e6 > MAX_TOTAL_MB:
        print('more than %d MB, nothing downloaded' % MAX_TOTAL_MB)
        return 2
    if args.dry_run:
        return 0

    for source, kind, asset_id, resolution, purpose, files in plan:
        folder = os.path.join(args.out, source, asset_id)
        os.makedirs(folder, exist_ok=True)
        fetched = []
        for relative, url, size, md5 in files:
            path = safe_path(folder, relative)
            got = download(url, path, md5)
            fetched.append({'file': relative, 'url': url, 'bytes': got})
            if relative.endswith('.zip'):
                with zipfile.ZipFile(path) as archive:
                    for member in archive.namelist():
                        safe_path(folder, member)
                    archive.extractall(folder)
        previous = next((r for r in records if r['source'] == source and r['id'] == asset_id), None)
        if previous:
            known = {f['file'] for f in fetched}
            fetched = [f for f in previous.get('files', []) if f['file'] not in known] + fetched
        records = [r for r in records if not (r['source'] == source and r['id'] == asset_id)]
        records.append({'source': source, 'id': asset_id, 'kind': kind, 'resolution': resolution, 'purpose': purpose,
                        'license': LICENSES[source], 'date': datetime.date.today().isoformat(), 'files': fetched,
                        'complete': True})
        with open(record_path, 'w', encoding='utf-8') as handle:
            json.dump(records, handle, indent=2, ensure_ascii=False)
        print('done: %s/%s (%.1f MB)' % (source, asset_id, sum(f['bytes'] for f in fetched) / 1e6))
    return 0


if __name__ == '__main__':
    sys.exit(main())
