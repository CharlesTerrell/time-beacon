# Task: Consolidate Environment Caching and Unify with mise.toml

## Task Goal
Combine `tools/cache_esp_env` and `tools/idf_env.sh` into a single unified system and align `mise.toml`:
1. Consolidate `tools/idf_env.sh`:
   - Dual-mode operation:
     - Sourced (`source tools/idf_env.sh`): fast activation (<5ms via cache), auto-generates cache if missing/stale.
     - Executed directly (`./tools/idf_env.sh` or with `--refresh`): updates/refreshes `.env.esp` cache on demand.
   - Output portable `.env.esp` (standard key-value format) compatible with both bash `source` and `mise` `_.file = ".env.esp"`.
2. Replace `tools/cache_esp_env`:
   - Turn `tools/cache_esp_env` into a simple 1-line wrapper that delegates to `tools/idf_env.sh --refresh`.
3. Update `mise.toml`:
   - Remove hardcoded `IDF_VER = "6.1"` and hardcoded version paths.
   - Connect `[tasks.update-esp-env]` to `./tools/idf_env.sh --refresh`.
4. Verification command:
   ```bash
   ./build_tracker.sh all && ./tests/run_tests.sh
   ```
5. Auto-commit on clean build and tests per `AGENTS.md`.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `tools/idf_env.sh`
- `tools/cache_esp_env`
- `mise.toml`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Implement dual-mode (source vs. refresh execution) in `tools/idf_env.sh`.
- [x] Simplify `tools/cache_esp_env` to call `tools/idf_env.sh`.
- [x] Update `mise.toml` to remove hardcoded versions and wire to `tools/idf_env.sh`.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
