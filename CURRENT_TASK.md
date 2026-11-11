# Task: Configure MeshCore to Local Standard (910.525 MHz, 62.5 kHz BW, SF7, CR5, 22 dBm, 2-byte path hash)

## Task Goal
Update MeshCore configuration and packet encoding in `main/lora_time_beacons.c`, `main/include/sx1262.h`, and `main/sx1262.c`:
1. In `main/include/sx1262.h`:
   - Add `#define SX1262_BW_62_5_KHZ 0x03`.
   - Add function prototype `esp_err_t sx1262_set_preamble_length(uint16_t preamble_len)`.
2. In `main/sx1262.c`:
   - Implement `sx1262_set_preamble_length()` to dynamically configure preamble length (16 for Meshtastic, 32 for MeshCore SF7).
   - Use dynamic preamble length in `sx1262_transmit()`.
3. In `main/lora_time_beacons.c`:
   - For MeshCore transmissions:
     - Tune to 910.525 MHz (`910525000` Hz) by default when on standard US915 frequency.
     - Set bandwidth to 62.5 kHz (`SX1262_BW_62_5_KHZ`).
     - Set spreading factor to SF7 (`sf = 7`).
     - Set coding rate to CR 4/5 (`SX1262_CR_4_5`).
     - Set TX power to 22 dBm.
     - Set preamble to 32 symbols.
     - Set packet header to `0x11` (`(PAYLOAD_TYPE_ADVERT << 2) | ROUTE_TYPE_FLOOD`).
     - Set path length to `0x40` (2-byte path hashing mode, 0 hops).
   - Reset preamble to 16 symbols when switching to Meshtastic.
4. In `tests/test_harness.c`:
   - Update `test_meshcore_packet_layout()` to verify the 2-byte path hash mode (`0x40`) and `0x11` flood header.
   - Add test assertions verifying MeshCore 910.525 MHz, SF7, 62.5 kHz BW parameters.
5. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
6. Verify build and tests pass with zero errors and zero warnings, mark task complete, and auto-commit.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `main/include/sx1262.h`
- `main/sx1262.c`
- `main/lora_time_beacons.c`
- `tests/test_harness.c`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Add 62.5 kHz bandwidth and preamble length control to `main/include/sx1262.h` and `main/sx1262.c`.
- [x] Update MeshCore radio params and packet header/path format in `main/lora_time_beacons.c`.
- [x] Update tests in `tests/test_harness.c`.
- [x] Verify build and tests pass with zero errors and zero warnings.
- [x] Automatically commit verified changes per AGENTS.md.
