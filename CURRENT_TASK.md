# Task: Add Ethernet zero-radio porting plan to docs/plan/porting/

## Task Goal
Document Ethernet and zero-radio deployment scenario in `docs/plan/porting/ethernet_no_radio.md`:
1. Detail hardware options (SPI W5500 on ESP32-S3 and RMII PoE on ESP32 with external GNSS).
2. Describe software architecture: native `esp_eth`, LwIP socket reuse, and complete RF disabling.
3. Update `docs/plan/porting/README.md` index table with the new scenario.
4. Verify tests and markdown link integrity pass cleanly.

## Allowed Files to Edit
- CURRENT_TASK.md
- docs/plan/porting/README.md
- docs/plan/porting/ethernet_no_radio.md

## Verification Command
```bash
./build.sh test && test -f docs/plan/porting/ethernet_no_radio.md && python3 -c "
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
- [x] Create docs/plan/porting/ethernet_no_radio.md.
- [x] Update docs/plan/porting/README.md index.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
