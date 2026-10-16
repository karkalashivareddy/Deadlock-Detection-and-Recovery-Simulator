# Algorithms and model

## Multiple-instance matrix detector

In `backend-c/src/deadlocklab.c`, Work begins as Available. Finish is true for terminated processes and for active processes with a zero allocation row; otherwise it begins false. The detector evaluates unfinished processes in stable input order and records Work, Request, component comparisons, decision, released Allocation, and updated Work. When Request ≤ Work, it simulates completion by returning the allocation to Work. A full scan with no progress ends detection. Remaining unfinished processes form the detector result under this model. The input scenario is never modified.

## Resource allocation graph

The browser derives process-to-resource request edges and resource-to-process allocation edges from the same scenario matrices. A cycle proves deadlock only for the appropriate single-instance model. With multiple resource instances, a cycle alone is not sufficient; matrix detection is used.

## Banker’s safety algorithm

Not implemented. Banker’s algorithm requires Max claims and Need = Max − Allocation and evaluates existence of a safe completion order. It is an avoidance/safety algorithm, not the general detector implemented here.

## Recovery

Termination marks a process terminated, clears its allocation row, and releases those counts to Available. Preemption is limited to resources marked preemptible; the reclaimed allocation is returned to Available, removed from the victim’s Allocation, and added to its outstanding Request to represent rollback/compensation. Each transition validates the resource invariant. The API reruns detection after recovery. No real kernel state or recovery-cost measurement is involved.
