# Operating System

Code I wrote for OS class assignment, which focus on concurrency problems.

## `cas-incr`

Demonstration of importance of atomic operation; Here, Compare-and-swap.

Here we increment a single integer from multiple threads, either using
atomic operation(`atomic`) or non-atomic operation(`racey`). The latter has
race condition, which results in some of the increments getting lost.

## `cas-list`

Implementation of lock-free stack using Compare-and-swap.

ABA problem mae this a hard one, especially because the language of choice
(Rust) lacks automatic garbage collection. I ended up pulling in a dependency
for easy epoch-based reclamation.