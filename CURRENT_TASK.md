# Task: Make ESP-IDF Environment Activation Portable and Self-Caching

## Task Goal
Make `tools/idf_env.sh` and `tools/cache_esp_env` portable across IDF versions and user home directories:
1. Remove all hardcoded `/home/charlie/` paths and hardcoded version numbers from tracked files.
2. Combine caching logic:
   - Detect `IDF_PATH` dynamically from environment or by finding latest `~/.espressif/v*/esp-idf` or `~/esp/esp-idf`.
   - Use `$HOME` and variable expansions instead of hardcoded absolute paths.
   - Cache active environment in `.env.esp` using `idf_tools.py export --format key-value`.
   - Invalidate and refresh cache automatically if `IDF_PATH` or version changes.
   - Sourcing `tools/idf_env.sh` is instantaneous (<5ms) when `.env.esp` is up-to-date.
3. Update `tools/cache_esp_env` to use the same portable detection logic and refresh `.env.esp`.
4. Ensure `.gitignore` ignores `.env.esp`.
5. Verify build and test suite pass with zero errors and zero warnings.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `tools/idf_env.sh`
- `tools/cache_esp_env`
- `.gitignore`

## Verification Command
```bash
./build_tracker.sh all && ./tests/run_tests.sh
```

## Checklist
- [x] Update `tools/idf_env.sh` with portable detection and `.env.esp` caching.
- [x] Update `tools/cache_esp_env` with portable path discovery.
- [x] Ensure `.gitignore` covers local cache files.
- [x] Run verification command with zero errors and zero warnings.
- [x] Automatically commit verified changes per `AGENTS.md`.
