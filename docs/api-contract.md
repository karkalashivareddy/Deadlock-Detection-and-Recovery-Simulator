# C API contract

Base URL: `http://localhost:8080/api`. JSON uses camelCase. Scenario properties are `id`, `processes`, `resources`, `total`, `available`, `allocation`, `request`, `preemptible`, and `terminated`.

```json
{"id":"case","processes":["P1"],"resources":["R1"],"total":[1],"available":[0],"allocation":[[1]],"request":[[1]],"preemptible":[false],"terminated":[]}
```

- `GET /health` returns status, version, and C runtime.
- `GET /scenarios` and `GET /scenarios/{id}` return deterministic fixtures.
- `POST /detect` accepts the complete scenario and returns `deadlocked`, ordered `trace` steps, and `finalWork`. A trace step contains process, workBefore, request, componentFits, eligible, releasedAllocation, workAfter, and decision.
- `POST /recover` accepts `{ "action":"terminate", "scenario":{...}, "process":"P1" }` or the `preempt` action with a resource identifier. It returns the validated updated scenario and a fresh detection result.

HTTP 400 returns a JSON validation error for malformed state, invalid accounting, or invalid recovery. Unknown fixture IDs and paths return 404. CORS permits the Vite development origin `http://localhost:5173`. Requests and responses are capped for this teaching server; its single-threaded request loop is not intended for production traffic.
