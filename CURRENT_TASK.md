# Task: Improve LoRa Frequency Configuration and Country Clarity in config.ini

## Task Goal
1. In `main/config_mgr.c`:
   - Add `lora_freq`, `lora_freq_hz`, and `lora_frequency` as aliases for `frequency` under `[lora]`.
   - Also allow `frequency` / `lora_freq` under `[device]` or `[meshtastic]`.
   - Support `country` under `[device]`, `[regulatory]`, or `[lora]` in addition to `[wifi]`.
   - In `config_mgr_write_default_ini()`:
     - Clarify the relationship between `country` and `lora_freq`.
     - Output `lora_freq = <Hz>` commented out, with clear instructions that commenting it out auto-selects frequency based on `country`.
     - Document regional presets (US915, EU868, AU915, IN865, JP920, KR920, CN470).
2. In `tests/test_harness.c`:
   - Add tests for `lora_freq` alias parsing and multi-section `country` parsing.
3. Verify build and native tests pass with zero errors and zero warnings.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/config_mgr.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Support `lora_freq` / `lora_freq_hz` aliases in `main/config_mgr.c`.
- [x] Support `country` in `[device]` / `[regulatory]` / `[lora]` in `main/config_mgr.c`.
- [x] Update `config_mgr_write_default_ini()` with clear country-to-frequency documentation and `lora_freq` naming.
- [x] Add unit tests in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
