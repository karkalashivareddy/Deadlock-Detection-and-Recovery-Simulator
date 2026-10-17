import json
import copy
import os
import subprocess
import time
import socket
from urllib.request import Request, urlopen

refused = subprocess.run(["./build/deadlocklab-server"], env={**os.environ, "BIND_ADDRESS": "0.0.0.0"}, capture_output=True, text=True)
assert refused.returncode == 2 and "Only loopback binding" in refused.stderr
server = subprocess.Popen(["./build/deadlocklab-server"])
base = "http://127.0.0.1:8080/api"

def get(path):
    with urlopen(base + path, timeout=2) as response:
        return json.load(response)

def post(path, payload):
    request = Request(base + path, data=json.dumps(payload).encode(), headers={"Content-Type": "application/json"})
    with urlopen(request, timeout=4) as response:
        return json.load(response)

def raw_request(data):
    with socket.create_connection(("127.0.0.1", 8080), timeout=2) as sock:
        sock.sendall(data)
        sock.shutdown(socket.SHUT_WR)
        chunks = []
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            chunks.append(chunk)
    return b"".join(chunks).split(b"\r\n", 1)[0]

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
    banker = {
        "id": "banker-safe", "processes": ["P0", "P1", "P2", "P3", "P4"],
        "resources": ["A", "B", "C"], "total": [10, 5, 7], "available": [3, 3, 2],
        "allocation": [[0, 1, 0], [2, 0, 0], [3, 0, 2], [2, 1, 1], [0, 0, 2]],
        "maximum": [[7, 5, 3], [3, 2, 2], [9, 0, 2], [2, 2, 2], [4, 3, 3]],
    }
    safe = post("/safe-state", banker)
    assert safe["safe"] is True
    assert safe["sequence"] == ["P1", "P3", "P4", "P0", "P2"]
    experiment_input = {"seed": 73012, "processCount": 5, "resourceCount": 3}
    experiment = post("/experiments", experiment_input)
    repeated = post("/experiments", experiment_input)
    assert experiment == repeated
    assert experiment["algorithmVersion"] == "xorshift32-v1"
    assert experiment["metrics"]["detectionEvaluations"] == len(experiment["detection"]["trace"])
    assert experiment["metrics"]["unfinishedProcesses"] == len(experiment["detection"]["deadlocked"])
    for j, total in enumerate(experiment["scenario"]["total"]):
        allocated = sum(row[j] for row in experiment["scenario"]["allocation"])
        assert total == experiment["scenario"]["available"][j] + allocated
    try:
        post("/experiments", {"seed": 1, "processCount": 13, "resourceCount": 1})
        raise AssertionError("unbounded experiment unexpectedly accepted")
    except Exception as error:
        assert "400" in str(error)
    current = get("/scenarios/no-deadlock")
    assert post("/import", {"schemaVersion": 1, "scenario": current}) == current
    checkpoint = copy.deepcopy(current)
    checkpoint["allocation"][0][0] = 0
    checkpoint["available"][0] = 2
    preempted = post("/recover", {"action": "preempt", "process": "P1", "resource": "R1", "scenario": current, "checkpoint": checkpoint})
    assert preempted["scenario"]["allocation"][0][0] == 0
    assert preempted["scenario"]["available"] == [2, 0]
    restored = post("/recover", {"action": "rollback", "scenario": current, "checkpoint": checkpoint})
    assert restored["scenario"] == checkpoint
    assert raw_request(b"GET /api/nope HTTP/1.1\r\nHost: localhost\r\n\r\n").startswith(b"HTTP/1.1 404")
    assert raw_request(b"BREW /api/health HTTP/1.1\r\nHost: localhost\r\n\r\n").startswith(b"HTTP/1.1 405")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 400")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: nope\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 400")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nContent-Length: 2\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 400")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 501")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nContent-Type: text/plain\r\n\r\n{}").startswith(b"HTTP/1.1 415")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 999999999\r\nContent-Type: application/json\r\n\r\n").startswith(b"HTTP/1.1 413")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 300000\r\nContent-Type: application/json\r\n\r\n").startswith(b"HTTP/1.1 413")
    invalid_escape = b'{"bad":"\\x"}'
    rejected_escape = raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: " + str(len(invalid_escape)).encode() + b"\r\nContent-Type: application/json\r\n\r\n" + invalid_escape)
    assert rejected_escape.startswith(b"HTTP/1.1 400")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nContent-Type: application/json\r\n\r\n{} ").startswith(b"HTTP/1.1 200") is False
    malformed = b'{"id":"x","id":"y"}'
    duplicate = raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: " + str(len(malformed)).encode() + b"\r\nContent-Type: application/json\r\n\r\n" + malformed)
    assert duplicate.startswith(b"HTTP/1.1 400")
    duplicate_import = b'{"schemaVersion":1,"schemaVersion":1,"scenario":{}}'
    duplicate_import_response = raw_request(b"POST /api/import HTTP/1.1\r\nHost: localhost\r\nContent-Length: " + str(len(duplicate_import)).encode() + b"\r\nContent-Type: application/json\r\n\r\n" + duplicate_import)
    assert duplicate_import_response.startswith(b"HTTP/1.1 400")
    nul_body = json.dumps({"schemaVersion": 1, "scenario": current}).encode() + b"\x00ignored"
    nul_response = raw_request(b"POST /api/import HTTP/1.1\r\nHost: localhost\r\nContent-Length: " + str(len(nul_body)).encode() + b"\r\nContent-Type: application/json\r\n\r\n" + nul_body)
    assert nul_response.startswith(b"HTTP/1.1 400")
    print("C POSIX API smoke tests passed")
finally:
    server.terminate()
    server.wait(timeout=5)
