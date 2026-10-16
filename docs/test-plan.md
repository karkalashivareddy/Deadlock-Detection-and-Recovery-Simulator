# Test plan

Backend JUnit coverage includes no-deadlock and deadlock fixtures, multi-instance completion, deterministic repeat detection, no input mutation, termination release and post-recovery verification, wait-for cycle/no-cycle behavior, and invalid accounting rejection. Expand with randomized reference comparisons, malformed dimensions/identifiers, zero-allocation request behavior, invalid preemption, and checkpoint tests.

Frontend Vitest/Testing Library covers fixture loading, matrix edit followed by real API detection, API error visibility, and trace navigation. Manual acceptance: load two-process fixture, run detector, inspect trace, terminate P1, confirm, and verify rerun has no deadlocked processes. Graph edges should reflect edited matrix values. Use keyboard Tab/Enter and narrow mobile viewport checks.

CI runs Maven tests and frontend npm ci, TypeScript lint, Vitest, and Vite production build.
