# Whirl-Matrix

Deterministic simulator for [Whirl](https://gitlab.com/whirl-framework/whirl-frontend)

## Example

[KV](/examples/kv/main.cpp) – simple replicated KV store

- Quorum reads / writes
- Timestamp ordering via wall clocks

Note: this implementation _intentionally_ violates [linearizability](https://jepsen.io/consistency/models/linearizable)!

Use Attiya, Bar-Noy, Dolev (ABD) algorithm for linearizable behaviour:
- [Original paper](https://groups.csail.mit.edu/tds/papers/Attiya/TM-423.pdf)
- [Notes on Theory of Distributed Systems](https://www.cs.yale.edu/homes/aspnes/classes/465/notes.pdf) – Chapter 16 – _Distributed Shared Memory_

## Features

* Determinism (concurrency, time, network, randomness)
* Time compression
* Linearizability checker
* Persistence (via filesystem) and node restarts
* Local clock skew and drift
* Google TrueTime simulation
* Pluggable asynchrony and fault injection strategy

## Inspiration

### Simulation + Fault Injection

#### FoundationDB
- [Testing Distributed Systems w/ Deterministic Simulation](https://www.youtube.com/watch?v=4fFDFbi3toc)
- [FoundationDB or: How I Learned to Stop Worrying and Trust the Database](https://www.youtube.com/watch?v=OJb8A6h9jQQ&list=PLSE8ODhjZXjagqlf1NxuBQwaMkrHXi-iz&index=22)

#### AWS

- [Millions of Tiny Databases](https://www.usenix.org/system/files/nsdi20-paper-brooker.pdf)

#### Jepsen
- https://github.com/jepsen-io/maelstrom

#### Paxi
- [Dissecting the Performance of Strongly-Consistent Replication Protocols](https://cse.buffalo.edu/~demirbas/publications/dissecting.pdf)
- https://github.com/ailidani/paxi

### Model checking

#### Microsoft Coyote

- https://microsoft.github.io/coyote
- https://github.com/microsoft/coyote

#### DSLabs
- [Teaching Rigorous Distributed Systems With Efficient Model Checking](https://ellismichael.com/papers/dslabs-eurosys19.pdf)
- https://github.com/emichael/dslabs  

#### StateRight
- https://github.com/stateright/stateright

### Fuzzing

- [Fuzzing Raft for Fun and Publication](https://colin-scott.github.io/blog/2015/10/07/fuzzing-raft-for-fun-and-profit/)
- [Minimizing Faulty Executions of Distributed Systems](https://www.usenix.org/conference/nsdi16/technical-sessions/presentation/scott)

## Requirements

- x86-64
- Linux or MacOS
- Clang++ (>= 10.0)

## Build

```shell
# Clone repo
git clone https://gitlab.com/whirl-framework/whirl-matrix.git 
cd whirl-matrix
# Generate build files
mkdir build && cd build
cmake -DWHIRL_MATRIX_EXAMPLES=ON ..
# Build kv example
make whirl_example_kv
# Run kv example
# --det - run determinism check
# --sims - number of simulations to run
./examples/kv/whirl_example_kv --det --sims 12345
