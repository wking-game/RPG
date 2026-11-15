"""
Generate the 3D models listed in tools/ki3d/flora.json with the Meshy API (text to 3D).

Per asset: one preview task (the shape, mesh only) and one refine task per state (texture), all from the
same preview, so e.g. a bush "full" and "picked" have exactly the same shape. Downloads every refined
model as GLB plus a thumbnail into the raw folder (outside the repository, default D:/gamer/ki3d_rohdaten/meshy)
and appends every task to meshy_log.json there (prompts, task ids, credits: needed for the AI disclosure).
Meshy deletes API tasks after 3 days, so files are downloaded right away.

The API key comes from the environment variable MESHY_API_KEY (also read from the Windows user environment).
It is never printed or written anywhere.

Usage (from the repository root):
  python tools/ki3d/meshy_generate.py --dry-run             plan and credit estimate, no API calls
  python tools/ki3d/meshy_generate.py                       generate everything still missing
  python tools/ki3d/meshy_generate.py --only rock_small,nettle
  python tools/ki3d/meshy_generate.py --max-credits 600     refuse to start above this estimate
"""
import argparse
import concurrent.futures
import datetime
import json
import os
import sys
import threading
import time
import urllib.error
import urllib.request

API = 'https://api.meshy.ai/openapi/v2/text-to-3d'
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
SPEC = os.path.join(REPO, 'tools', 'ki3d', 'flora.json')
DEFAULT_OUT = os.path.abspath(os.path.join(REPO, '..', 'ki3d_rohdaten', 'meshy'))
CREDITS_PREVIEW = 20   # meshy-7.1 / latest, standard geometry (docs.meshy.ai/en/api/pricing, 2026-11-13)
CREDITS_REFINE = 10    # 2k texture
POLL_SECONDS = 6
TASK_TIMEOUT = 25 * 60

log_lock = threading.Lock()
print_lock = threading.Lock()


def say(message):
    with print_lock:
        print(message, flush=True)


def api_key():
    key = os.environ.get('MESHY_API_KEY', '').strip()
    if not key and sys.platform == 'win32':
        # Set after this process started: read the user, then the system environment from the registry.
        import winreg
        for root, path in ((winreg.HKEY_CURRENT_USER, 'Environment'),
                           (winreg.HKEY_LOCAL_MACHINE, r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment')):
            try:
                with winreg.OpenKey(root, path) as env:
                    key = str(winreg.QueryValueEx(env, 'MESHY_API_KEY')[0]).strip()
            except OSError:
                key = ''
            if key:
                break
    return key


def request(method, url, key, body=None):
    data = json.dumps(body).encode('utf-8') if body is not None else None
    for attempt in range(8):
        req = urllib.request.Request(url, data=data, method=method)
        req.add_header('Authorization', 'Bearer ' + key)
        req.add_header('User-Agent', 'gamer-asset-pipeline/1.0')
        if data is not None:
            req.add_header('Content-Type', 'application/json')
        try:
            with urllib.request.urlopen(req, timeout=60) as response:
                return json.loads(response.read().decode('utf-8'))
        except urllib.error.HTTPError as error:
            text = error.read().decode('utf-8', 'replace')[:300]
            if error.code == 429:
                wait = float(error.headers.get('Retry-After') or 10)
                say('  rate limit (%s), waiting %.0f s' % (text[:60], wait))
                time.sleep(wait)
                continue
            if error.code in (500, 502, 503, 504) and attempt < 4:
                time.sleep(5 * (attempt + 1))
                continue
            raise RuntimeError('HTTP %d on %s %s: %s' % (error.code, method, url.replace(API, ''), text))
        except urllib.error.URLError as error:
            if attempt < 4:
                time.sleep(5 * (attempt + 1))
                continue
            raise RuntimeError('network error on %s: %s' % (url, error))
    raise RuntimeError('too many retries on ' + url)


def wait_for(task_id, key, label):
    start = time.time()
    last = -1
    while time.time() - start < TASK_TIMEOUT:
        task = request('GET', API + '/' + task_id, key)
        status = task.get('status')
        progress = task.get('progress', 0)
        if status == 'SUCCEEDED':
            return task
        if status in ('FAILED', 'CANCELED'):
            raise RuntimeError('%s %s: %s' % (label, status, task.get('task_error')))
        if progress != last and progress % 25 == 0:
            say('  %s: %s %d %%' % (label, status, progress))
            last = progress
        time.sleep(POLL_SECONDS)
    raise RuntimeError('%s: no result after %d min' % (label, TASK_TIMEOUT // 60))


def download(url, path):
    tmp = path + '.part'
    req = urllib.request.Request(url, headers={'User-Agent': 'gamer-asset-pipeline/1.0'})
    with urllib.request.urlopen(req, timeout=300) as response, open(tmp, 'wb') as handle:
        while True:
            chunk = response.read(1 << 20)
            if not chunk:
                break
            handle.write(chunk)
    os.replace(tmp, path)


def append_log(out, record):
    path = os.path.join(out, 'meshy_log.json')
    with log_lock:
        records = []
        if os.path.exists(path):
            with open(path, encoding='utf-8') as handle:
                records = json.load(handle)
        records.append(record)
        with open(path, 'w', encoding='utf-8') as handle:
            json.dump(records, handle, indent=2, ensure_ascii=False)


def plan(spec, out, only, force):
    """Assets with the states still missing (no GLB yet)."""
    todo = []
    for asset in spec['assets']:
        if only and asset['id'] not in only:
            continue
        missing = [s for s in asset['states'] if force or not os.path.exists(os.path.join(out, '%s__%s.glb' % (asset['id'], s)))]
        if missing:
            todo.append((asset, missing))
    return todo


def make_asset(spec, asset, states, key, out):
    preview_prompt = (spec['style_prompt'] + ' ' + asset['prompt']).strip()
    if len(preview_prompt) > 800:
        raise RuntimeError('%s: prompt longer than 800 characters' % asset['id'])
    body = dict(spec['preview'])
    body.update({'mode': 'preview', 'prompt': preview_prompt, 'target_polycount': int(asset['tris']), 'target_formats': ['glb']})
    preview_id = request('POST', API, key, body)['result']
    say('%s: preview %s started' % (asset['id'], preview_id))
    preview = wait_for(preview_id, key, asset['id'] + ' preview')
    append_log(out, {'asset': asset['id'], 'task': 'preview', 'task_id': preview_id, 'prompt': preview_prompt,
                     'ai_model': body.get('ai_model'), 'target_polycount': body['target_polycount'],
                     'consumed_credits': preview.get('consumed_credits'), 'date': datetime.date.today().isoformat()})
    for state in states:
        texture_prompt = (spec['texture_style'] + ' ' + asset['states'][state]).strip()
        body = dict(spec['refine'])
        body.update({'mode': 'refine', 'preview_task_id': preview_id, 'texture_prompt': texture_prompt, 'target_formats': ['glb']})
        refine_id = request('POST', API, key, body)['result']
        say('%s/%s: refine %s started' % (asset['id'], state, refine_id))
        task = wait_for(refine_id, key, '%s/%s refine' % (asset['id'], state))
        glb = (task.get('model_urls') or {}).get('glb')
        if not glb:
            raise RuntimeError('%s/%s: no GLB in the result' % (asset['id'], state))
        base = os.path.join(out, '%s__%s' % (asset['id'], state))
        download(glb, base + '.glb')
        if task.get('thumbnail_url'):
            try:
                download(task['thumbnail_url'], base + '.png')
            except Exception as error:  # a missing thumbnail is not worth failing for
                say('  thumbnail not saved: %s' % error)
        append_log(out, {'asset': asset['id'], 'state': state, 'task': 'refine', 'task_id': refine_id,
                         'preview_task_id': preview_id, 'texture_prompt': texture_prompt,
                         'consumed_credits': task.get('consumed_credits'), 'file': os.path.basename(base) + '.glb',
                         'date': datetime.date.today().isoformat()})
        say('%s/%s: saved %s.glb (%s credits)' % (asset['id'], state, os.path.basename(base), task.get('consumed_credits')))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--dry-run', action='store_true')
    parser.add_argument('--only', default='', help='comma separated asset ids')
    parser.add_argument('--force', action='store_true', help='generate again even if the GLB exists')
    parser.add_argument('--max-credits', type=int, default=0, help='do not start if the estimate is higher')
    parser.add_argument('--out', default=DEFAULT_OUT)
    parser.add_argument('--workers', type=int, default=4)
    args = parser.parse_args()

    with open(SPEC, encoding='utf-8') as handle:
        spec = json.load(handle)
    only = {s.strip() for s in args.only.split(',') if s.strip()}
    os.makedirs(args.out, exist_ok=True)
    todo = plan(spec, args.out, only, args.force)
    estimate = sum(CREDITS_PREVIEW + CREDITS_REFINE * len(states) for _, states in todo)
    for asset, states in todo:
        say('to do: %-18s states %s' % (asset['id'], ', '.join(states)))
    say('assets: %d, refines: %d, estimated credits: %d (preview %d, refine %d each)'
        % (len(todo), sum(len(s) for _, s in todo), estimate, CREDITS_PREVIEW, CREDITS_REFINE))
    say('raw folder: ' + args.out)
    if args.dry_run or not todo:
        return 0
    if args.max_credits and estimate > args.max_credits:
        say('estimate %d is above --max-credits %d, nothing started' % (estimate, args.max_credits))
        return 2
    key = api_key()
    if not key:
        say('MESHY_API_KEY is not set (user environment variable). Nothing started.')
        return 2

    failed = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, min(args.workers, 8))) as pool:
        futures = {pool.submit(make_asset, spec, asset, states, key, args.out): asset['id'] for asset, states in todo}
        for future in concurrent.futures.as_completed(futures):
            try:
                future.result()
            except Exception as error:
                failed.append(futures[future])
                say('FAILED %s: %s' % (futures[future], error))
    say('done: %d ok, %d failed%s' % (len(todo) - len(failed), len(failed), (' (' + ', '.join(failed) + ')') if failed else ''))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
