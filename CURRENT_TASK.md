# Task: Revise docs/plan/*.md for clarity, correctness, and conciseness

## Task Goal
Refine subsystem architecture specifications in `docs/plan/`:
1. Fix broken GitHub link in `docs/plan/meshtastic_beacons.md`.
2. Add explicit status notices to `docs/plan/meshcore_integration_spec.md` (MeshCore security difficulties and archived status) and `docs/plan/porting.md` (PlatformIO & Zephyr RTOS non-Espressif investigation).
3. Replace raw LaTeX math formatting across all files with clean Unicode notation.
4. Harmonize duplicated power scaling documentation between `docs/plan/power_management.md` and `docs/plan/wifi_ntp.md`.
5. Run test suite to verify project health.

## Allowed Files to Edit
- CURRENT_TASK.md
- docs/plan/ble_cts.md
- docs/plan/boot_selector.md
- docs/plan/display_management.md
- docs/plan/gps_time_position.md
- docs/plan/meshcore_integration_spec.md
- docs/plan/meshtastic_beacons.md
- docs/plan/porting.md
- docs/plan/power_management.md
- docs/plan/wifi_ntp.md

## Verification Command
```bash
./build.sh test && python3 -c "
import glob, re, os
for path in glob.glob('docs/plan/*.md'):
    content = open(path).read()
    if 'file:///home/' in content:
        raise ValueError(f'Hardcoded absolute path found in {path}')
    links = re.findall(r'\[.*?\]\((.*?)\)', content)
    for l in links:
        if l.startswith('http') or l.startswith('#'): continue
        target = os.path.normpath(os.path.join(os.path.dirname(path), l.split('#')[0]))
        if not os.path.exists(target):
            raise FileNotFoundError(f'Broken link in {path}: {l} -> {target}')
print('Link and path check clean!')
"
```

## Checklist
- [x] Revise all 9 files in docs/plan/.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
