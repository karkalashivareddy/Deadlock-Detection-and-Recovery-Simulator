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

- [ ] Model resource preemption with explicit rollback/restart state; current API behavior is a limited compensation simulation and must not be described as OS rollback.
- [ ] Add immutable checkpoint snapshots and deterministic rollback.
- [ ] Add versioned strict scenario import/export and replayable event history.
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
- Current local frontend verification after adding the Banker panel: `npm.cmd run lint`, `npm.cmd test -- --reporter=dot` (5 tests), and `npm.cmd run build` all pass.
