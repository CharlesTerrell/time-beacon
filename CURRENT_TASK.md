# Task: Document Espressif RISC-V porting scenario in docs/plan/porting.md

## Task Goal
Update `docs/plan/porting.md` with Section 5 analyzing future migration to Espressif RISC-V architectures (e.g. ESP32-C3, ESP32-C6, ESP32-P4):
1. Explain that ~95%+ of code remains unchanged due to unified ESP-IDF HAL.
2. Outline specific required adjustments (~5%): board pinout mapping, ADC calibration scheme differences, and USB-OTG vs USB Serial/JTAG MSC constraints.
3. Run test suite to verify project health.

## Allowed Files to Edit
- CURRENT_TASK.md
- docs/plan/porting.md

## Verification Command
```bash
./build.sh test && test -f docs/plan/porting.md
```

## Checklist
- [x] Add Espressif RISC-V scenario to docs/plan/porting.md.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
