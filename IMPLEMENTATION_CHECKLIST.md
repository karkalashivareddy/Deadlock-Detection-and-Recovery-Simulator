# DeadlockLab implementation checklist

- [x] Audit repository, Git status, and installed runtimes
- [x] Define domain model, validation, API contract, and deterministic fixtures
- [x] Implement and test matrix detection and single-instance graph detection
- [x] Implement validated termination, preemption, and checkpoint service transitions
- [x] Compile and live-test the C/POSIX HTTP server under Linux (verified by Ubuntu CI)
- [x] Build responsive React/TypeScript dashboard, derived graph, process/resource controls, and cinematic light-theme motion
- [ ] Add checkpoint/preemption UI, strict scenario import, deterministic replay navigation, and optional experiments
- [x] Add CI and project documentation
- [~] Run portable C core tests and frontend checks; Linux server verification remains pending

Audit: initial repository had no project source, only .git. Java 25, Node 24, and MinGW GCC 15 are available. The project is now Linux-first C/POSIX; WSL is installed but cannot enumerate distros in this environment (E_ACCESSDENIED).

Verification: portable C core compiles with strict warnings and passes under MinGW GCC. GitHub Actions run 38903609539 completed successfully for commit 6d9d0748: Ubuntu C server build/core tests/live API smoke passed; frontend npm ci, TypeScript check, Vitest, and Vite production build passed. Local WSL access is denied, so Linux verification was performed by CI. Visual polish includes scroll reveals, progress cue, SVG edge/node transitions, trace step transitions, hover states, ambient light motion, and reduced-motion support.
