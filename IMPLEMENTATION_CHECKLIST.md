# DeadlockLab implementation checklist

- [x] Audit repository, Git status, and installed runtimes
- [x] Define domain model, validation, API contract, and deterministic fixtures
- [x] Implement and test matrix detection and single-instance graph detection
- [x] Implement validated termination, preemption, and checkpoint service transitions
- [ ] Compile and live-test the C/POSIX HTTP server under Linux (WSL access is denied locally; Ubuntu CI is configured)
- [x] Build responsive React/TypeScript dashboard, derived graph, process/resource controls, and cinematic light-theme motion
- [ ] Add checkpoint/preemption UI, strict scenario import, deterministic replay navigation, and optional experiments
- [x] Add CI and project documentation
- [~] Run portable C core tests and frontend checks; Linux server verification remains pending

Audit: initial repository had no project source, only .git. Java 25, Node 24, and MinGW GCC 15 are available. The project is now Linux-first C/POSIX; WSL is installed but cannot enumerate distros in this environment (E_ACCESSDENIED).

Verification before the C pivot: Spring Boot tests/package and live endpoint checks passed. After the C pivot: portable C core tests compile with strict warnings and pass under MinGW GCC; frontend TypeScript check, Vitest, and Vite production build pass. The POSIX HTTP server still needs an Ubuntu/WSL build and live API check; this machine's WSL distro access is denied. Visual polish includes scroll reveals, progress cue, SVG edge/node transitions, trace step transitions, hover states, ambient light motion, and reduced-motion support.
