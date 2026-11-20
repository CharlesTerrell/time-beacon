# Task: Modularize porting scenarios into docs/plan/porting/*.md

## Task Goal
Restructure `docs/plan/porting.md` into modular directory `docs/plan/porting/`:
1. Create `docs/plan/porting/README.md` index of porting scenarios.
2. Create `docs/plan/porting/platformio_nrf52.md` for PlatformIO/Zephyr non-Espressif plan.
3. Create `docs/plan/porting/espressif_riscv.md` for Espressif RISC-V migration plan.
4. Remove monolithic `docs/plan/porting.md` and update `README.md` link.
5. Verify all links and test suite pass cleanly.

## Allowed Files to Edit
- CURRENT_TASK.md
- README.md
- docs/plan/porting.md
- docs/plan/porting/README.md
- docs/plan/porting/platformio_nrf52.md
- docs/plan/porting/espressif_riscv.md

## Verification Command
```bash
./build.sh test && test -f docs/plan/porting/README.md && test -f docs/plan/porting/platformio_nrf52.md && test -f docs/plan/porting/espressif_riscv.md && python3 -c "
import glob, re, os
for path in glob.glob('docs/plan/**/*.md', recursive=True):
    content = open(path).read()
    if 'file:///home/' in content:
        raise ValueError(f'Hardcoded absolute path found in {path}')
    links = re.findall(r'\[.*?\]\((.*?)\)', content)
    for l in links:
        if l.startswith('http') or l.startswith('#'): continue
        target = os.path.normpath(os.path.join(os.path.dirname(path), l.split('#')[0]))
        if not os.path.exists(target):
            raise FileNotFoundError(f'Broken link in {path}: {l} -> {target}')
print('Link check clean!')
"
```

## Checklist
- [x] Create docs/plan/porting/ directory with modular scenario documents.
- [x] Remove docs/plan/porting.md and update README.md link.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
