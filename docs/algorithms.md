# Algorithms and model

## Multiple-instance matrix detector

In `backend-c/src/deadlocklab.c`, Work begins as Available. Finish is true for terminated processes and for active processes with a zero allocation row; otherwise it begins false. The detector evaluates unfinished processes in stable input order and records Work, Request, component comparisons, decision, released Allocation, and updated Work. When Request ≤ Work, it simulates completion by returning the allocation to Work. A full scan with no progress ends detection. Remaining unfinished processes form the detector result under this model. The input scenario is never modified.

## Resource allocation graph

The browser derives process-to-resource request edges and resource-to-process allocation edges from the same scenario matrices. A cycle proves deadlock only for the appropriate single-instance model. With multiple resource instances, a cycle alone is not sufficient; matrix detection is used.

## Banker’s safety algorithm

Implemented independently in `backend-c/src/banker.c` with a separate `DlBankerScenario`, so the deadlock detector's Request matrix retains its original meaning. Need is computed as Max − Allocation after validating Allocation ≤ Max and the resource totals. A deterministic input-order scan records Work, Need, eligibility and Work after completion, returning a safe sequence when one exists. An unsafe result means no completion order was found under the declared maximum claims; it does **not** prove a current deadlock. The endpoint is `POST /api/safe-state`.

## Recovery

Termination marks a process terminated, clears both live allocation and request rows, and releases its allocations exactly once. Browser checkpoints are immutable copies of a validated scenario. Full rollback restores the entire compatible snapshot. Resource preemption requires a compatible snapshot and a resource configured as preemptible; it restores that victim's saved allocation and request rows, recomputes Available from Total and all active allocations, validates the result, and reruns detection. Checkpoint compatibility requires the same scenario ID, process/resource ordering, resource totals, and preemptibility configuration. These transitions model state rollback, not process instruction state, kernel scheduling, or measured recovery cost.
