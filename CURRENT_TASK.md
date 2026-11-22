# Task: Edit prose in README.md to be more concise

## Task Goal
Streamline prose in README.md to be more concise while preserving all technical facts, hardware targets, configuration details, and documentation links.

## Allowed Files to Edit
- CURRENT_TASK.md
- README.md

## Verification Command
```bash
./build.sh test && python3 -c "
import re, os
content = open('README.md').read()
if 'file:///home/' in content:
    raise ValueError('Hardcoded absolute path found in README.md')
links = re.findall(r'\[.*?\]\((.*?)\)', content)
for l in links:
    if l.startswith('http') or l.startswith('#'): continue
    target = os.path.normpath(os.path.join('.', l.split('#')[0]))
    if not os.path.exists(target):
        raise FileNotFoundError(f'Broken link in README.md: {l} -> {target}')
print('README link check clean!')
"
```

## Checklist
- [x] Edit README.md prose to be concise while preserving all technical substance.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
