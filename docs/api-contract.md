# C API contract

Base URL: `http://localhost:8080/api`. JSON uses camelCase. Scenario properties are `id`, `processes`, `resources`, `total`, `available`, `allocation`, `request`, `preemptible`, and `terminated`.

```json
{"id":"case","processes":["P1"],"resources":["R1"],"total":[1],"available":[0],"allocation":[[1]],"request":[[1]],"preemptible":[false],"terminated":[]}
```

- `GET /health` returns status, version, and C runtime.
- `GET /scenarios` and `GET /scenarios/{id}` return deterministic fixtures.
- `POST /detect` accepts the complete scenario and returns `deadlocked`, ordered `trace` steps, and `finalWork`. A trace step contains process, workBefore, request, componentFits, eligible, releasedAllocation, workAfter, and decision.
- `POST /recover` accepts `{ "action":"terminate", "scenario":{...}, "process":"P1" }` or the `preempt` action with a resource identifier. It returns the validated updated scenario and a fresh detection result.
- `POST /safe-state` accepts a separate Banker schema with `id`, `processes`, `resources`, `total`, `available`, `allocation`, and `maximum`. `Need` is derived internally; it is never inferred from or substituted for the detection `request` matrix. The response includes `algorithm: "banker_safety"`, `safe`, deterministic `sequence`, `unfinished`, Available and a Work/Need trace. An unsafe state is not reported as a current deadlock.

Scenario objects reject unknown and duplicate keys and require every documented property. The parser validates complete JSON syntax, nesting depth and bounded token count. HTTP requires HTTP/1.1, rejects duplicate/malformed Content-Length and all Transfer-Encoding, and expects JSON Content-Type on POST. Bodies are limited to 256 KiB; headers to 16 KiB; the socket read/write timeout is three seconds. Requests are sequential. HTTP 400 returns a JSON error for malformed framing, JSON, state, or recovery; 404 is unknown path/fixture, 405 unsupported method, 413 oversized body, 415 unsupported content type, and 501 unsupported transfer encoding. The listener binds only `127.0.0.1`; non-loopback `BIND_ADDRESS` is refused. CORS permits the Vite development origin `http://localhost:5173`, but CORS is not authentication. This remains a local educational server, not a production service.

Example Banker payload:

```json
{"id":"banker-safe","processes":["P0","P1"],"resources":["A"],"total":[3],"available":[1],"allocation":[[1],[1]],"maximum":[[2],[2]]}
```
