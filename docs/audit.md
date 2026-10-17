# DeadlockLab engineering audit

## Baseline

- Commit: `84733745b804c19fa1545ae593f773b8a97644ee` (`main`, `origin/main`). Working tree was clean before audit changes.
- Repository contents were inspected directly, including the C header/core/server/tests, frontend entry point and tests, Makefile, docs, README and CI.
- Local environment: Windows PowerShell, MinGW GCC 15.1, Node 24/npm, no GNU Make, WSL present but `wsl --status` returns `E_ACCESSDENIED`. POSIX HTTP server cannot be run locally.
- C baseline: `gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror -Ibackend-c/include backend-c/src/deadlocklab.c backend-c/tests/test_deadlocklab.c -o $env:TEMP/deadlocklab-audit-c-tests.exe; & $env:TEMP/deadlocklab-audit-c-tests.exe` — passed (`C deadlocklab tests passed`).
- Frontend baseline: `npm.cmd run lint` — passed; `npm.cmd test -- --reporter=dot` — 4 tests passed; `npm.cmd run build` — passed.
- Latest baseline CI: [run 38904649825](https://github.com/karkalashivareddy/Deadlock-Detection-and-Recovery-Simulator/actions/runs/38904649825), success at this SHA. C backend build/test and live API smoke passed on Ubuntu; frontend lint/test/build passed.
- Existing API payload: Scenario fields `id`, `processes`, `resources`, `total`, `available`, `allocation`, `request`, `preemptible`, `terminated`; `/api/detect` returns `deadlocked`, `trace`, `finalWork`; `/api/recover` supports `terminate` and `preempt`.
- Existing fixture assertions: no-deadlock → empty unfinished set; two-process fixture → P1/P2; multi-instance fixture → empty unfinished set; zero-allocation fixture → P2 unfinished. Terminating P1 in the two-process fixture yields an empty unfinished set and Available `[1,0]` (Ubuntu API smoke).
- Existing error shapes are static JSON `{error,message}` for recognized validation and missing-route cases. Several malformed HTTP requests are not classified consistently.

## Findings

### P0 — API binds every network interface without authentication

- **File/evidence:** `backend-c/src/server.c`; listener uses `INADDR_ANY`, logs `0.0.0.0`, and accepts unauthenticated requests. CORS is the only browser-origin restriction.
- **Reproduction/impact:** starting the server on a reachable host exposes scenario mutation/recovery endpoints to network clients. CORS does not restrict command-line clients and is not authentication.
- **Fix:** bind to loopback by default; reject non-loopback configuration unless a separate, tested authenticated remote mode is deliberately implemented.

### P0 — JSON parsing searches text instead of validating JSON structure

- **File/evidence:** `backend-c/src/server.c` helpers `field`, `scenario_json`, and array readers use `strstr`, `strchr`, and `strtol` without a complete syntax/token pass. Duplicate keys, trailing garbage and wrong top-level nesting are not reliably rejected.
- **Reproduction/impact:** semantically malformed JSON can be accepted if expected substrings and arrays are present; duplicate key interpretation depends on occurrence order. Parser assumptions risk invalid reads and ambiguous state.
- **Fix:** use a bounded parser with explicit object/array traversal, duplicate-key detection, schema validation and complete-input consumption. Keep parser and HTTP adapter separate from the C algorithm core.

### P1 — HTTP framing and blocking behavior are not bounded correctly

- **File/evidence:** `server.c` reads until header/body completion with no socket timeout; `Content-Length` is found by case-insensitive substring search; `Transfer-Encoding` and request-line/header grammar are not validated; `send` is called once; response append silently truncates at `RESPONSE_LIMIT`.
- **Reproduction/impact:** slow or incomplete clients can hold the single-threaded service indefinitely; chunked bodies are mishandled; partial sends can truncate otherwise valid responses; oversized traces can produce invalid JSON.
- **Fix:** implement bounded HTTP parsing, deadlines, explicit transfer-encoding rejection, size checks, send-all/read-all loops, and consistent status/error responses.

### P1 — Terminated-process live state is not validated or cleared

- **File/evidence:** `backend-c/src/deadlocklab.c::dl_validate` excludes terminated processes from the accounting sum but does not require their allocation/request rows to be zero. `dl_recover_terminate` zeros Allocation but leaves Request unchanged.
- **Reproduction/impact:** a caller can submit a terminated process that still holds allocations or has outstanding requests; detector marks it finished and silently ignores that live state. This violates the intended canonical-state invariant.
- **Fix:** require zero live Allocation and Request for terminated processes; clear both during termination; keep historical data outside the live scenario.

### P1 — Recovery output aliasing can mutate input on failure

- **File/evidence:** `dl_recover_terminate` writes directly to `out` while processing resources and can return on overflow after partial writes. If `input == output`, the caller's state is mutated despite failure.
- **Reproduction/impact:** construct a state whose later resource addition overflows and call with aliased input/output; earlier resource columns have already changed.
- **Fix:** compute into a local candidate, validate the entire candidate, then copy to output only on success. Test aliased and distinct buffers.

### P1 — Preemption semantics invent a larger pending request

- **File/evidence:** `dl_recover_preempt` adds the reclaimed count directly to `request[victim][resource]`; README/docs call this compensation without defining a restart or rollback state.
- **Reproduction/impact:** preempting an allocation can increase the process's outstanding request while it continues as active, changing the user's scenario without a defined process transition.
- **Fix:** choose and document an explicit model (e.g. rollback to a stored checkpoint or mark victim blocked/restarted) and return a recovery trace. Reject unsupported preemption until the transition is defined.

### P1 — RAG cycle claims exceed implemented graph analysis

- **File/evidence:** the UI derives an SVG RAG, but no C wait-for/single-instance cycle detector exists. Guide/docs explain cycle conditions but no algorithm result is computed.
- **Impact:** a user may infer that a visual cycle is an implemented deadlock verdict.
- **Fix:** label the graph as visualization only, or implement a clearly assumption-gated single-instance analysis and test it.

### P1 — Several documented/planned educational modules are not executable

- **Evidence:** no Max/Need type or Banker endpoint; no checkpoint store/rollback; no scenario import; event history stores display strings rather than state-bearing events; no deterministic replay or experiment generator.
- **Impact:** these capabilities cannot be demonstrated despite their mention in the original project brief.
- **Fix:** implement separately versioned models and tests, or keep them clearly marked unavailable. Prioritize state safety and core correctness first.

### P2 — Limits and identifier handling need explicit contracts

- **File/evidence:** fixed limits are defined in `deadlocklab.h`, but HTTP size and error behavior are not coherently tied to those limits. Scenario `id` may be empty in `dl_validate`; process/resource identifier rules are code-only.
- **Impact:** callers cannot predict accepted input boundaries or distinguish configuration errors from parser errors.
- **Fix:** document limits, require nonempty identifiers, and return field-specific validation errors.

### P2 — API and parser integration tests are too narrow

- **File/evidence:** `tests/test_deadlocklab.c` covers fixtures/recovery basics; `smoke_api.py` covers health, fixture, detection and termination only. No malformed framing, duplicate keys, oversized body, unknown method or recovery failure cases.
- **Impact:** most of the network boundary is untested.
- **Fix:** add table-driven parser/HTTP tests and adversarial cases before new UI features.

### P2 — Current event history is not replayable

- **File/evidence:** `frontend/src/ui/App.tsx` appends timestamped strings; no state snapshots, sequence IDs or replay cursor exist.
- **Impact:** history is a visual log only and cannot reconstruct prior state.
- **Fix:** implement bounded immutable events with before/after snapshots and read-only replay state.

### P3 — Frontend tests omit recovery confirmation and accessibility behavior

- **File/evidence:** four Vitest cases cover scenario load/edit, process addition, API error and trace display. No tests exercise recovery confirmation, mobile layout, reduced motion or keyboard focus.
- **Impact:** important interactions remain manually verified only.
- **Fix:** add user-event tests for confirmation and keyboard flows, then browser-level checks where supported.

## Features implemented versus illustrative/unavailable

- **IMPLEMENTED:** C17 matrix detection, C validation/recovery helpers, POSIX API endpoints, deterministic fixtures, React matrix editing and termination UI, static instructional guide.
- **DERIVED:** graph edges, detection trace, final Work and fixture state summaries.
- **ILLUSTRATIVE:** recovery cost label; no cost is measured.
- **UNAVAILABLE:** actual kernel-process inspection/recovery, event replay and seeded experiments. Banker, checkpoint recovery, and strict scenario import/export were added in later phases below.

## Incremental work after the baseline (uncommitted)

- Added `backend-c/src/json.c` and `include/json.h`: bounded recursive-descent JSON parser, duplicate-key detection (including escaped-equivalent keys), UTF-8 and escape validation, nesting/token limits, and complete-input validation. `tests/test_json.c` covers valid nested input and malformed/ambiguous cases. Strict MinGW C17 compilation and parser tests passed locally.
- Replaced scenario-field substring searches with token-based type and dimension validation. Scenario objects reject unknown keys and must supply the documented fields.
- Changed the POSIX listener to bind only to `127.0.0.1`; non-loopback `BIND_ADDRESS` and invalid `PORT` values are rejected. Client sockets have three-second receive/send timeouts, and responses use a send-all loop. Added deliberate HTTP framing checks for request line, header syntax, duplicate/invalid/missing lengths, bounded size, JSON content type, and rejected transfer encoding. `SIGPIPE` is ignored. These changes are not yet POSIX-compiled or live-tested because this environment is Windows and WSL access is denied; Ubuntu CI must validate them.
- Domain validation now rejects unterminated/invalid identifiers and terminated processes retaining live allocation/request state. Termination clears both matrices and commits a validated candidate. Preemption now requires an explicit compatible checkpoint and restores the victim's saved Allocation/Request rows before recomputing Available. This change, strict versioned import/export, and the new recovery UI still await the next Ubuntu CI run.
- Current local verification for these edits: parser strict compile/test passed; core test initially exposed empty-scenario-ID semantics, which were adjusted to preserve the documented empty-system case. Rerun all C tests after the final patch and use CI for POSIX integration. Frontend lint, 4 Vitest tests and production build passed after retrying outside the sandbox.

### Remaining priority findings

Loopback-only binding, token-based JSON parsing, HTTP framing and checkpoint/import endpoints are verified by Ubuntu CI/live smoke at SHA `40fb4437cdfbf36b407f331c309a3c3f2ce59991`. The Banker phase also passed Ubuntu CI and sanitizer checks at SHA `03340bf97aa88cf14487bcbefc4ad03dbb48e670`. Response serialization still uses a bounded buffer and should report overflow explicitly. Checkpoint recovery and strict scenario import are implemented. Event history/replay has bounded session snapshots, detection results, export, read-only navigation, and strict replay-record import in the current uncommitted phase; local lint and frontend tests pass, with build/CI pending. Deterministic experiments remain unavailable. Existing RAG remains a visualization only.

### Banker implementation audit (subsequent phase)

- **Files:** `backend-c/include/banker.h`, `backend-c/src/banker.c`, `/api/safe-state`, and the frontend Banker panel.
- **Evidence:** separate scenario type carries Max; validation requires Allocation ≤ Max and consistent totals; test covers the canonical safe sequence, unsafe example, invalid claim, and empty boundary. API smoke checks the expected sequence `P1, P3, P4, P0, P2` for the canonical fixture. Frontend sends editable Max claims to this endpoint and labels unsafe as not proving deadlock.
- **Limit:** this module does not participate in matrix deadlock detection and currently has no built-in Banker fixture library; the frontend initializes Max to Allocation and the student edits claims.
