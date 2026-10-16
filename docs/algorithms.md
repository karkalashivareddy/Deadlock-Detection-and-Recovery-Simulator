# Algorithms and model

## Multiple-instance detection

For each resource type, Work begins at Available. Finish is initialized true for zero-allocation processes and false for processes holding resources. The detector scans processes in stable input order; for every unfinished process it records the vector comparison Request ≤ Work. If eligible, it simulates completion, adds Allocation to Work, marks Finish true, and continues scanning. It stops after a full pass makes no progress. Remaining unfinished processes are reported as deadlocked under this detection model. This is deterministic and does not mutate the input.

## Graph model

The UI draws request edges P → R and allocation edges R → P, annotated with instance counts. Under a single-instance-per-resource model, a cycle proves deadlock. With multiple instances, a cycle is not sufficient; use matrix detection. WaitForGraphDetector exposes cycle evaluation for an explicitly constructed wait-for graph.

## Banker’s algorithm

Not implemented. Banker’s safety testing requires Max claims and Need = Max − Allocation, and checks for a safe completion sequence. It is an avoidance/safety algorithm and must not be reported as general deadlock detection.

## Recovery

Termination records the process as ended, releases every allocated instance to Available, and clears its allocation row. Preemption is allowed only for a configured preemptible resource. The process allocation is removed and its outstanding request is increased by the reclaimed amount, representing rollback/compensation. Checkpoint rollback returns the supplied complete saved scenario after validation. The API reruns detection after each action. The current interface exposes termination; preemption and rollback are available to backend callers. No real operating-system cost is measured.
