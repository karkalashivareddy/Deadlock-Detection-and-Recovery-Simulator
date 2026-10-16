# DeadlockLab implementation checklist

- [x] Audit repository, Git status, and installed runtimes
- [x] Define domain model, validation, API contract, and deterministic fixtures
- [x] Implement and test matrix detection and single-instance graph detection
- [x] Implement validated termination, preemption, and checkpoint service transitions
- [x] Expose Spring Boot REST API and integration tests
- [x] Build responsive React/TypeScript dashboard, derived graph, and cinematic light-theme motion
- [ ] Complete scenario authoring (process/resource add/remove, totals/available editing), all recovery UI actions, import/replay, and optional experiments
- [x] Add CI and project documentation
- [x] Run backend tests, frontend tests/build, and verify startup/health

Audit: empty Git repository on main (origin configured but no commits); Java 25 and Node 24 available; Maven and Maven wrapper absent; npm.cmd available (PowerShell blocks npm.ps1).

Verification: Maven 3.9.11 was downloaded to the system temporary directory; backend verify passed (12 tests). Frontend TypeScript check passed, Vitest passed (3 tests), and Vite production build passed. Live health and deadlock API checks passed. Visual polish includes scroll reveals, progress cue, SVG edge/node transitions, trace step transitions, hover states, ambient light motion, and reduced-motion support.
