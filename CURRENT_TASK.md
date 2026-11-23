# Task: Update Tracker v1.1 PPS definition and documentation

## Task Goal
Update `main/boards/board_tracker_v1_1.h` (`BOARD_HAS_GNSS_PPS 1`), `README.md`, `docs/plan/gps_time_position.md`, `web/index.html`, and `tests/test_harness.c` to accurately reflect that hardware testing verified 1PPS is wired to GPIO 36 on Heltec Wireless Tracker v1.1.

## Allowed Files to Edit
- CURRENT_TASK.md
- main/boards/board_tracker_v1_1.h
- README.md
- docs/plan/gps_time_position.md
- web/index.html
- tests/test_harness.c

## Verification Command
```bash
./build.sh test && ./build.sh v1.1
```

## Checklist
- [x] Set `BOARD_HAS_GNSS_PPS 1` and remove unrouted comments in `main/boards/board_tracker_v1_1.h`.
- [x] Update README.md hardware and binary descriptions.
- [x] Update `docs/plan/gps_time_position.md` PPS hardware routing section.
- [x] Update `web/index.html` board descriptions.
- [x] Update `tests/test_harness.c` comment.
- [x] Run verification command with zero errors and zero warnings.
- [x] Mark checklist items complete and commit changes per AGENTS.md rules.
