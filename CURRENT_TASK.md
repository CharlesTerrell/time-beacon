# Task: Set Meshtastic beacon broadcast to off by default

## Task Goal
Disable Meshtastic LoRa beacons by default to prevent unwanted RF emissions:
1. Change default `enable_meshtastic` from `true` to `false` in `main/config_mgr.c`.
2. Update sample `config.ini` in `README.md` and `docs/plan/boot_selector.md` to show `enable = false`.
3. Verify test suite passes cleanly.

## Allowed Files to Edit
- CURRENT_TASK.md
- main/config_mgr.c
- README.md
- docs/plan/boot_selector.md

## Verification Command
```bash
./build.sh test
```

## Checklist
- [x] Set enable_meshtastic default to false in config_mgr.c.
- [x] Update README.md and boot_selector.md configuration docs.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
