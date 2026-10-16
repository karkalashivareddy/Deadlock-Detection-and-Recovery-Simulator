import json
import subprocess
import time
import socket
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
    assert raw_request(b"GET /api/nope HTTP/1.1\r\nHost: localhost\r\n\r\n").startswith(b"HTTP/1.1 404")
    assert raw_request(b"BREW /api/health HTTP/1.1\r\nHost: localhost\r\n\r\n").startswith(b"HTTP/1.1 405")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 400")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: nope\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 400")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\n\r\n{}").startswith(b"HTTP/1.1 501")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nContent-Type: text/plain\r\n\r\n{}").startswith(b"HTTP/1.1 415")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 999999999\r\nContent-Type: application/json\r\n\r\n").startswith(b"HTTP/1.1 413")
    assert raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\nContent-Type: application/json\r\n\r\n{} ").startswith(b"HTTP/1.1 200") is False
    malformed = b'{"id":"x","id":"y"}'
    duplicate = raw_request(b"POST /api/detect HTTP/1.1\r\nHost: localhost\r\nContent-Length: " + str(len(malformed)).encode() + b"\r\nContent-Type: application/json\r\n\r\n" + malformed)
    assert duplicate.startswith(b"HTTP/1.1 400")
    print("C POSIX API smoke tests passed")
finally:
    server.terminate()
    server.wait(timeout=5)
