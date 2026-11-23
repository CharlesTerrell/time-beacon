# Task: Merged board-specific firmware binaries, GitHub Releases, and Web Installer

## Task Goal
1. Update `build.sh` and `.github/workflows/ci.yml` to generate single merged `.bin` files clearly named by hardware model (`time-beacon-heltec-tracker-v1.1.bin` and `time-beacon-heltec-tracker-v2.bin`).
2. Add automated GitHub Releases publishing in `.github/workflows/ci.yml`.
3. Add a browser-based Web Serial flasher (`web/index.html` and manifest JSONs) and automatic GitHub Pages deployment in `.github/workflows/ci.yml`.
4. Update `README.md` with clear installation options for non-developers (Web Installer, Pre-built binaries, Web Flasher).

## Allowed Files to Edit
- CURRENT_TASK.md
- build.sh
- .github/workflows/ci.yml
- web/index.html
- web/manifest-tracker-v1.1.json
- web/manifest-tracker-v2.json
- README.md

## Verification Command
```bash
./build.sh test && ./build.sh all && test -f build_v1/time-beacon-heltec-tracker-v1.1.bin && test -f build_v2/time-beacon-heltec-tracker-v2.bin
```

## Checklist
- [x] Update `build.sh` to generate merged board binaries via `esptool.py merge_bin`.
- [x] Create `web/index.html` and manifest files for ESP Web Tools.
- [x] Update `.github/workflows/ci.yml` with merged binary generation, release publishing, and GitHub Pages deployment.
- [x] Update `README.md` with Web Installer and Pre-built binary instructions.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
