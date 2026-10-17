# DeadlockLab implementation checklist

## Verified complete

- [x] Audited repository, branch, remote, source tree, baseline tests, frontend entry points, and existing CI; see [docs/audit.md](docs/audit.md).
- [x] Kept the C17/POSIX backend and React/TypeScript frontend; no Java backend substitution.
- [x] Implemented the multiple-instance matrix detector with deterministic Work/Request trace and fixed fixtures.
- [x] Validated termination using a candidate state; terminated processes have zero allocation and request.
- [x] Added bounded JSON parsing, syntax/escape/duplicate-key validation, exact schema keys, and parser tests.
- [x] Restricted the listener to loopback; bounded HTTP header/body sizes and socket waits; rejected transfer-encoding; added send-all.
- [x] Added independent C Banker’s safety model, deterministic sequence/trace, `/api/safe-state`, frontend Max editor, and a clear unsafe-state explanation.
- [x] Kept the light responsive dashboard and derived graph; frontend lint, tests, and build pass locally.
- [x] Added CI phases and adversarial HTTP smoke checks.

## Remaining work

- [x] Model resource preemption using a compatible saved checkpoint; restore victim allocation/request rows, recompute availability, validate, and rerun detection.
- [x] Add browser-session immutable checkpoint snapshots and full state rollback; no instruction-level process state is modeled.
- [x] Add strict versioned scenario import/export; C API rejects duplicates, unknown fields, malformed syntax, NUL/trailing bytes, and invalid state.
- [x] Add bounded session-scoped event history with before/after snapshots, detection results, export, and read-only replay navigation.
- [x] Add strict version 1 replay import with duplicate-key checks, event sequence/state validation, detection trace validation, and bounded file size.
- [ ] Add bounded seeded experiment mode and independent generated-state reference checks.
- [ ] Add wait-for graph cycle analysis if needed; current RAG is a visualization only.
- [ ] Expand API and frontend verification for mobile/keyboard/reduced-motion and recovery confirmations.
- [ ] Review response-size truncation behavior and return a structured error if serialization exceeds the response cap.
- [ ] Continue phase-wise commits and rerun CI after every backend or UI integration phase.

## Verification record

- Baseline SHA: `84733745b804c19fa1545ae593f773b8a97644ee`.
- Security/domain phase SHA: `299fbbf04d5eb101621fa1a2c5d68bf0d9ea9eb9`.
- CI run [38910721706](https://github.com/karkalashivareddy/Deadlock-Detection-and-Recovery-Simulator/actions/runs/38910721706) passed backend build, C tests, live POSIX API smoke, frontend lint, Vitest (4 tests at that revision), and production build.
- MinGW strict C17 core, parser, and Banker tests pass locally. Sanitizer linking is unavailable in this MinGW installation (`-lasan`/`-lubsan` missing); Ubuntu CI sanitizer run is being added.
- Current development environment is Windows PowerShell; WSL returns `E_ACCESSDENIED`. Linux/POSIX validation must therefore come from the Ubuntu workflow.
- Current local frontend verification after adding Banker, checkpoint controls, and import UI: `npm.cmd run lint`, `npm.cmd test -- --reporter=dot` (7 tests), and `npm.cmd run build` pass.
- Replay phase local verification: `npm.cmd run lint`, `npm.cmd test -- --reporter=dot` (8 tests), and `npm.cmd run build` pass. The test/build commands required execution outside the sandbox because esbuild could not read the workspace parent directory.
- Replay import local verification: `npm.cmd run lint` and `npm.cmd test -- --reporter=dot` (14 tests) pass. Production build and CI for this change remain to be run.
- Current local portable C tests cover 250 fixed-seed generated scenarios against an independent matrix detector, Banker fixture cases, and checkpoint recovery invariants. POSIX checkpoint/import HTTP integration awaits Ubuntu CI at its next commit.
