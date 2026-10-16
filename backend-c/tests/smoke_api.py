import json
import subprocess
import time
from urllib.request import Request, urlopen

server = subprocess.Popen(["./build/deadlocklab-server"])
base = "http://127.0.0.1:8080/api"

def get(path):
    with urlopen(base + path, timeout=2) as response:
        return json.load(response)

def post(path, payload):
    request = Request(base + path, data=json.dumps(payload).encode(), headers={"Content-Type": "application/json"})
    with urlopen(request, timeout=4) as response:
        return json.load(response)

try:
    for _ in range(40):
        try:
            assert get("/health")["runtime"] == "C17/POSIX"
            break
        except Exception:
            time.sleep(0.1)
    else:
        raise RuntimeError("C API did not become healthy")

    fixtures = get("/scenarios")
    assert len(fixtures) == 5
    deadlock = get("/scenarios/two-process-deadlock")
    result = post("/detect", deadlock)
    assert result["deadlocked"] == ["P1", "P2"]
    recovered = post("/recover", {"action": "terminate", "process": "P1", "scenario": deadlock})
    assert recovered["detection"]["deadlocked"] == []
    assert recovered["scenario"]["available"] == [1, 0]
    print("C POSIX API smoke tests passed")
finally:
    server.terminate()
    server.wait(timeout=5)
