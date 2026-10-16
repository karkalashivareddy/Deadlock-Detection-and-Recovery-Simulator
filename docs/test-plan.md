# Test plan

`make test` compiles the C implementation with strict warnings and runs deterministic fixtures for empty/zero-allocation behavior, no deadlock, two-process deadlock, multiple instances, termination recovery, valid and invalid preemption, and nonnegative counts. Recovery tests check post-transition resource accounting and rerun detection. GitHub Actions runs this on Ubuntu, which also compiles the POSIX server.

Frontend Vitest covers live API fixture loading, canonical matrix editing, process creation with correctly sized rows, API error display, and trace navigation. TypeScript checking and Vite production build run in CI.

Manual demo check: start C API, load the two-process fixture, run detection, inspect trace, terminate P1, confirm the updated state and empty deadlocked set, then load the multiple-instance fixture. Check browser keyboard navigation and a mobile viewport. Future extensions should add JSON fuzzing, malformed HTTP request tests, recovery confirmation tests, deterministic history replay tests, and seeded reference comparisons.
