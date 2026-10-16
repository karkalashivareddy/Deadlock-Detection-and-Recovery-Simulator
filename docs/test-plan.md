# Test plan

## Executed coverage

The current C suite is split into `test_deadlocklab.c`, `test_json.c`, and `test_banker.c`; `make test` compiles with strict C17 warnings. `tests/smoke_api.py` launches the real POSIX server and exercises health, fixtures, detection, termination, checkpoint rollback/preemption, Banker’s known safe sequence, versioned import round trip, unknown routes, unsupported methods, missing/invalid/duplicate Content-Length, rejected Transfer-Encoding, wrong content type, payload-size rejection, embedded NUL, malformed escapes and duplicate JSON keys. GitHub Actions runs the C suite and live smoke on Ubuntu. `make sanitize` runs core, parser and Banker tests with AddressSanitizer and UndefinedBehaviorSanitizer.

The current Windows environment cannot link sanitizers or compile POSIX sockets: MinGW lacks ASan/UBSan runtime libraries and POSIX headers; WSL access returns `E_ACCESSDENIED`. Sanitizer and live server verification are delegated to Ubuntu CI.

Frontend checks currently cover scenario loading/editing, adding processes, API error display, trace navigation, and sending editable Banker Max claims. Mobile, keyboard-only, reduced-motion, import/replay, recovery confirmation and checkpoint UI still need dedicated tests.

`make test` compiles the C implementation with strict warnings and runs deterministic fixtures for empty/zero-allocation behavior, no deadlock, two-process deadlock, multiple instances, termination recovery, valid and invalid preemption, and nonnegative counts. Recovery tests check post-transition resource accounting and rerun detection. GitHub Actions runs this on Ubuntu, which also compiles the POSIX server.

Frontend Vitest covers live API fixture loading, canonical matrix editing, process creation with correctly sized rows, API error display, and trace navigation. TypeScript checking and Vite production build run in CI.

Manual demo check: start C API, load the two-process fixture, run detection, inspect trace, terminate P1, confirm the updated state and empty deadlocked set, then load the multiple-instance fixture. Check browser keyboard navigation and a mobile viewport. Future extensions should add JSON fuzzing, malformed HTTP request tests, recovery confirmation tests, deterministic history replay tests, and seeded reference comparisons.
