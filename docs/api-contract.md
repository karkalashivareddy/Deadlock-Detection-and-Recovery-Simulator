# API contract

Base URL: `http://localhost:8080/api`. JSON uses camelCase. Scenario includes `id`, `processes`, `resources`, `total`, `available`, `allocation`, `request`, `preemptible`, and `terminated`.

```json
{"id":"example","processes":["P1"],"resources":["R1"],"total":[1],"available":[0],"allocation":[[1]],"request":[[1]],"preemptible":[true],"terminated":[]}
```

- `GET /health` → `{ "status": "ok", "version": "1.0.0" }`
- `GET /scenarios` and `GET /scenarios/{id}` → deterministic fixture(s)
- `POST /detect` with Scenario → `{ "deadlocked": [...], "trace": [...], "finalWork": [...] }`. Each trace step contains process, workBefore, request, componentFits, eligible, releasedAllocation, workAfter, decision.
- `POST /recover` accepts `{ "action":"terminate", "scenario":{...}, "process":"P1" }` and returns `{ "scenario":{...}, "detection":{...} }`. Actions accepted by the service are `terminate`, `preempt` (requires `resource`), and `rollback` (requires `checkpoint`).

Invalid dimensions, negative counts, inconsistent totals, invalid victims, and forbidden preemption return HTTP 400 with `{ "error":"validation_error", "message":"..." }`. Missing scenario IDs return 404. Detection is stateless; include the complete state on every request.
