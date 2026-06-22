# The Complete Two-Year Build
### Master reference: roadmap, resources, and timeline — math lane and systems lane, in full

Everything from the planning so far, consolidated into one document. Lane A (math → ML theory) is summarized at the month level, since the calculus track already has its own detailed weekly files. Lane B (systems → scalable ML infrastructure) is written out in full — every phase, every build/break/fix cycle, every resource — since this is the part that's never existed as a single written document until now.

**Time budget:** 25+ hrs/week × ~104 weeks ≈ 2,600+ hours. Roughly 60% Lane A / 40% Lane B in year one, flipping to 40% Lane A / 60% Lane B in year two.

**Methodology, stated once, applied throughout:** for every systems phase below — build the simplest version that could possibly work, measure it, form a specific hypothesis about how it will fail, run the experiment that breaks it on purpose, then fix it and understand precisely why the fix is necessary. Write a one-page explanation after each project. This is not a formality; the "break it" step is where the actual learning happens, and skipping straight to the "correct" design defeats the point.

---

## Part 0 — Timeline at a glance

| Months | Lane A (Math → ML Theory) | Lane B (Systems → Scalable Infra) |
|---|---|---|
| 1–4 | Calculus (hybrid Stewart + Abbott/Spivak) + Linear Algebra | Unix Shell + Persistent KV Store with WAL |
| 4–7 | Multivariable Calculus + Probability & Statistics | (Lane B continues into Month 5–9 below) |
| 5–9 | — | Networking Fundamentals + Raft-Based Distributed KV Store |
| 7–12 | Classical ML + Deep Learning Theory | — |
| 9–13 | — | Vector Database + RAG Infrastructure + LLM Gateway |
| 13–24 | Applied ML Theory, as needed | Model Serving + Distributed Training + Deployment + Capstone |

---

## Part 1 — Lane A: Math Foundations → ML Theory (summary)

### A1 — Calculus + Linear Algebra (Months 1–4)
Calculus runs as a hybrid: Stewart's chapters set the pace for problem-solving speed, Abbott and Spivak attach the proof Stewart skips, and the original logic/proof and Dedekind-cut phases are compressed to the minimum needed to read those two books rather than run as standalone units.

| Weeks | Stewart (speed) | Rigor layer | Gate |
|---|---|---|---|
| 1 | Ch. 1–2.3 (functions, limit intuition) | Hammack — minimum proof literacy (quantifiers, induction, contradiction) | Write a contrapositive proof unaided |
| 2 | Ch. 2.4–2.8 (precise limit, continuity, derivative intro) | Abbott Ch. 1–2 (ℝ, sequences, ε–δ) | Full ε–δ proof of a specific limit |
| 3–4 | Ch. 3–4 (differentiation rules, MVT) | Spivak Ch. 9–11 (rules proved, Rolle's/MVT from EVT) | Prove MVT from Rolle's + EVT, unaided |
| 5–6 | Ch. 5–8 (integrals, techniques, applications) | Spivak Ch. 13–14 (Riemann sums, both parts of FTC) | Derive both parts of FTC back to back |
| 7–8 | Ch. 11 (sequences and series, incl. Taylor) | Rudin Ch. 3, 7, 8 / Spivak Ch. 20–24 (convergence, Taylor's theorem) | State the e^(−1/x²) counterexample from memory |

*(Full weekly capability checklists and the dependency-map reasoning already exist in the calculus milestone and dependency-map files from earlier in this plan.)*

Linear algebra runs in parallel across the full 16 weeks of A1, independent of calculus:

| Resource | Role |
|---|---|
| Axler — *Linear Algebra Done Right* | Primary, proof-first: vector spaces, linear maps, eigenvalues built abstractly |
| Strang — *Introduction to Linear Algebra* + MIT 18.06 OCW | Computational intuition: LU, QR, eigendecomposition, SVD |
| 3Blue1Brown — *Essence of Linear Algebra* | Visualization |
| Boyd & Vandenberghe — *Introduction to Applied Linear Algebra* (free) | ML-specific bridge |

**Gate, end of Month 4:** prove a basic eigenvalue property from the definition, derive the SVD's existence for a real matrix, and connect "rate of change" (calculus) and "rank" (linear algebra) as the two ideas everything downstream depends on.

### A2 — Multivariable Calculus + Probability & Statistics (Months 4–7)
- **Multivariable:** Hubbard & Hubbard — total derivative as a linear map, Jacobian, multivariable Taylor, implicit/inverse function theorems.
- **Probability, primary:** Blitzstein & Hwang — *Introduction to Probability* (Stat 110).
- **Probability, rigor companion:** Casella & Berger — *Statistical Inference*.
- **Deferred, not skipped:** measure-theoretic probability — same trade-off as the Dedekind cuts, optional enrichment rather than a gate.

**Gate:** derive the MLE for a specific distribution from first principles, and connect "likelihood" back to the Fermat/Rolle/MVT optimization logic from A1.

### A3 — Classical ML + Deep Learning (Months 7–12)
- Hastie, Tibshirani, Friedman — *The Elements of Statistical Learning* (free) for classical ML, derived rather than asserted.
- Goodfellow, Bengio, Courville — *Deep Learning*, read in full (backprop, CNNs, RNNs, regularization, generalization).
- Karpathy's micrograd extended into a small autograd-backed MLP, then a minimal Transformer once attention is covered.

**Gate:** train a network with an optimizer you implemented yourself, and justify the learning rate against the descent lemma by name.

### A4 — Applied ML Theory (Months 13–24, as needed)
Demand-driven once Lane B's later phases need specific theory — quantization theory for efficient inference, matrix factorization for a recommender, attention variants and KV-cache theory for an LLM-serving system.

---

## Part 2 — Lane B: Systems → Scalable ML Infrastructure (full detail)

### B1 — Unix Shell + Persistent KV Store with WAL (Weeks 1–20 / Months 1–5)

#### Project 1: Unix Shell (Weeks 1–4)

**Week 1 — Process execution.** Build a REPL: read a line, split on whitespace (quoting comes later), `fork()`, `execvp()` in the child, `waitpid()` in the parent, decode the exit status with `WIFEXITED`/`WEXITSTATUS`/`WIFSIGNALED`/`WTERMSIG`.
*Break it:* run a nonexistent command without checking `execvp`'s return value — since `execvp` only returns on failure, an unchecked failure lets the child fall through and keep executing the shell's own code, duplicating the REPL. Separately, never call `waitpid()` and watch zombie processes accumulate in `ps`.
*Fix:* always check `execvp`'s return, `_exit(127)` on failure in the child; always reap children.
*Resources:* OSTEP (processes, process API chapters, free); man pages `fork(2)`, `execve(2)`, `wait(2)`.

**Week 2 — Parsing & redirection.** Replace whitespace-splitting with a real tokenizer handling single quotes (fully literal), double quotes (a few escapes), and operators (`<`, `>`, `>>`, `2>`) as distinct tokens even without surrounding spaces. Implement redirection: in the **child**, after `fork()` and before `exec`, `open()` the target file and `dup2()` it onto the right FD.
*Break it:* do the `dup2()` calls in the **parent** instead — the shell's own stdout silently redirects into the file and the prompt vanishes.
*Fix:* redirection strictly between `fork()` and `exec()`, in the child only.
*Resources:* Kerrisk, *The Linux Programming Interface* (FD chapters); man pages `open(2)`, `dup2(2)`.

**Week 3 — Pipes & built-ins.** Generalize to an N-stage pipeline: create N−1 pipes, fork N children, each child `dup2`s the correct read/write ends onto stdin/stdout, then **closes every pipe FD it isn't using**, then execs. Implement `cd`, `exit`, `pwd`, `export` as built-ins recognized **before** the fork/exec path, run directly in the shell's own process.
*Break it (pipes):* leave one unused pipe FD open in a child — `yes | head -3 | cat` hangs forever instead of terminating, because some process still holds a writable reference to the pipe the last stage is reading.
*Break it (builtins):* implement `cd` via fork/exec calling `chdir()` in a child — `cd /tmp` followed by `pwd` shows the old directory, because the child's directory change never reaches the parent.
*Resources:* man pages `pipe(2)`, `chdir(2)`; "Write a Shell in C" style tutorials.

**Week 4 — Job control.** Background execution with `&`: track PID + command in a jobs list, reap asynchronously via a `SIGCHLD` handler calling `waitpid(-1, &status, WNOHANG)`. Process groups: the **first** process in a pipeline calls `setpgid(0,0)`; every **subsequent** process in that *same* pipeline calls `setpgid(0, leader_pgid)` — all stages of one pipeline share one group. Use `tcsetpgrp()` to hand terminal control to the foreground job's group, and back to the shell when it finishes. Implement `jobs`, `fg`, `bg`.
*Break it (no groups):* skip `setpgid` entirely — Ctrl-C kills the shell itself, since by default every forked child inherits the shell's own process group.
*Break it (wrong grouping):* give every pipeline **stage** its own group instead of sharing one — Ctrl-C now only kills one stage of `cmd1 | cmd2 | cmd3`, and `jobs`/`fg` get confused about what "the job" even is.
*Resources:* man pages `setpgid(2)`, `tcsetpgrp(3)`, `sigaction(2)`; Kerrisk's job-control chapter.

**Shell gate:** explain why an unguarded Ctrl-C kills the whole shell, and demonstrate `cmd1 | cmd2 | cmd3 &` → `jobs` → `fg` working correctly.

#### Project 2: Persistent KV Store with WAL (Weeks 5–20)

**Weeks 5–6 — In-memory store + naive persistence.** Hashmap-backed get/put/delete. Naive persistence: rewrite the entire dataset to disk on every write.
*Break it (perf):* measure write latency at 1K/10K/100K/1M keys — confirm it grows roughly linearly, since every write re-serializes everything.
*Break it (atomicity):* `kill -9` mid-rewrite, restart, observe corruption/loss extending backward beyond just the in-flight write.
*Resources:* Kleppmann, *Designing Data-Intensive Applications*, Ch. 3 opening.

**Weeks 7–9 — Append-only WAL.** Redesign as: every put/delete is a record appended to a log; the hashmap is rebuilt by replaying the log at startup. Record format: `[4-byte length][1-byte op][4-byte key length][key][4-byte value length][value]`.
*Break it:* `kill -9` mid-append — a naive replay trusting the length prefix reads garbage or crashes on the truncated tail record.
*Fix:* recovery validates as it replays — if the file doesn't actually contain as many bytes as a length prefix claims, stop and discard that partial record, keep everything before it.
*Resources:* Kleppmann Ch. 3 (log-structured storage); the Bitcask design paper (Riak).

**Weeks 10–11 — Durability.** Add a CRC32 checksum per record. Replace bare `write()` with explicit `fsync()`/`fdatasync()`, either every write or batched ("group commit").
*Break it (corruption):* flip one byte in the middle of an otherwise complete, correctly-sized record — a length-only check trusts it; a checksum catches it.
*Break it (the fsync gap):* `kill -9` does **not** drop the OS page cache, so it won't demonstrate this — you need a hard VM reset. Without `fsync()`, writes the app considered "done" can vanish on hard reset; with `fsync()` after every write, none do.
*Resources:* man pages `fsync(2)`, `fdatasync(2)`; SQLite's WAL-mode documentation; the well-known 2018 PostgreSQL fsync-error-handling issue, as a real-world cautionary read.

**Weeks 12–14 — Compaction & checkpointing.** Periodically snapshot current state to a file, start a fresh empty log, and make recovery mean "load latest snapshot, replay only the log since it" rather than replaying full history. Snapshot writes must themselves be atomic: write to a temp file, `fsync()` it, then `rename()` it into place (`rename` is atomic on POSIX for same-filesystem files).
*Break it:* write the same 100 keys a million times — confirm both log size and replay time grow unboundedly pre-compaction, and stabilize post-compaction near the size of the *distinct live keys*, not the operation count.
*Resources:* Kleppmann Ch. 3; LevelDB's design docs; Redis's RDB/AOF documentation.

**Weeks 15–17 — Indexing & scaling beyond memory.** Redesign the in-memory structure to hold only `key → (segment file, offset, length)` rather than full values — reads become an index lookup plus one seek. Stretch: sort a segment by key into an immutable SSTable and merge multiple SSTables in a background compaction pass (the LSM-tree shape).
*Break it:* measure in-memory footprint of "index with full values" vs. "index with offset+length only" at realistic value sizes, and measure the added per-read latency from the extra seek — quantify the actual trade-off.
*Resources:* Kleppmann Ch. 3 (SSTables/LSM); Petrov, *Database Internals*, for the harder stretch goal.

**Weeks 18–20 — Concurrency & network interface.** A single mutex around the store first (correct, slow), then a reader-writer lock (concurrent reads). Wrap the engine in a TCP server with a simple line protocol (`PUT key value`, `GET key`, `DELETE key`).
*Break it:* hammer the store with concurrent unlocked writers — interleaved writes splice bytes from two operations together, and recovery's checksum rejects records that were never actually corrupted by a crash, only by concurrency. Quantify the corruption rate.
*Why this sets up B2:* Raft replaces "trust the single writer" with "commit only once a majority durably logs it" — which only makes sense once you've felt exactly what a single node's log does and doesn't guarantee.
*Resources:* man pages `pthread_rwlock(3)`, `socket(2)`; the Raft paper's section on the replicated log, read now.

**KV store gate:** state exactly what your store guarantees about a write that returned "success" — when is it actually crash-safe? — citing your own crash experiments, not the theory alone.

---

### B2 — Networking Fundamentals + Raft-Based Distributed KV Store (Months 5–9)

**Phase 2.0 — Networking fundamentals (~2–3 weeks).** Raw sockets (`socket`/`bind`/`listen`/`accept`/`connect`), then a length-prefixed framing protocol, then a minimal HTTP/1.1 request parser (method/path/headers, `Content-Length` framing).
*Break it:* `send()` a payload larger than a typical recv buffer in one call from the client; a server doing a single `recv()` gets fewer bytes than were sent — because **TCP is a byte stream, not a message protocol**, and nothing guarantees one `send()` maps to one `recv()`.
*Fix:* a `read_exactly(fd, n)` helper that loops on `recv()` until it has all `n` bytes — mandatory, not defensive.
*Resources:* Beej's Guide to Network Programming (free); Kerrisk's sockets chapters; RFC 7230, read selectively.

**Phase 2.1 — RPC layer.** A small request/response mechanism on top of the framing protocol: message types, request/response correlation for concurrent in-flight calls, and timeouts.
*Concept to internalize:* "no response" and "explicit failure" are genuinely different and a network partition is indistinguishable from a dead peer to the caller — this ambiguity is fundamental, not a bug.

**Phase 2.2 — Leader election.** `RequestVote` RPC, randomized election timeouts, term-based elections, majority-vote-to-win.
*Break it:* give every node the **same** election timeout — repeated split votes, no stable leader. Randomize within a range large compared to network round-trip time — stable leader emerges quickly.
*Resources:* Ongaro & Ousterhout, "In Search of an Understandable Consensus Algorithm" (read it in full here); raft.github.io's visualization.

**Phase 2.3 — Log replication.** `AppendEntries` RPC; leader tracks each follower's `nextIndex`/`matchIndex`; `commitIndex` advances once an entry is on a **majority**, not all, of nodes.
*Break it:* implement "commit as soon as the leader itself has written it" (skip the majority check), crash the leader immediately after telling a client "success," force a new election — the new leader's log doesn't contain that entry. An acknowledged write silently disappeared.
*Fix:* only acknowledge success once a majority durably holds the entry — B1's fsync discipline, now applied cluster-wide.

**Phase 2.4 — Safety + wiring into B1.** Implement the election restriction (a candidate can only win if its log is at least as up-to-date as a majority of voters). Then replace B1's "trust the local fsync" model with "commit only once Raft reports majority replication," making B1's storage engine the state machine Raft's committed entries apply to.
*Break it:* deliberately **omit** the election restriction first — a node with a stale log can still win an election via a lucky timeout and overwrite already-committed entries elsewhere, losing "durable" data despite every individual node's fsync being correct. This is the single most important correctness property in the project.
*Resources:* Kleppmann DDIA Ch. 9 (consensus, linearizability).

**Raft gate:** kill the current leader mid-write-burst; confirm bounded-time re-election, zero loss of any *acknowledged* write, and explain why an *unacknowledged* write may or may not survive — and why that's correct, not a bug.

---

### B3 — Vector Database + RAG Infrastructure + LLM Gateway (Months 9–13)

**Phase 3.1 — Vector indexing fundamentals.** Brute-force k-NN baseline first (linear scan, cosine/L2 distance) — this is both the required "naive version" and the ground truth for checking the approximate index later. Then a simplified HNSW: layered proximity graphs, sparser at the top, search proceeding greedily downward.
*Break it:* measure recall@10 against the brute-force baseline at different construction/search parameters (M, efSearch) — confirm a real, measurable accuracy/speed trade-off, not a free approximation.
*Resources:* the original HNSW paper (Malkov & Yashunin); Weaviate's and Pinecone's engineering blogs on vector index internals.

**Phase 3.2 — Persistence, reusing B1.** Persist the index with the *same* WAL+snapshot discipline already built — deliberate reuse, not new machinery.

**Phase 3.3 — Hybrid search + reranking.** Combine the vector index with a keyword index (BM25, implementable directly from its formula) via score fusion.
*Break it:* build a small test set of exact-token queries (error codes, SKUs) and semantically-rephrased queries — measure pure-vector search failing on the first kind and pure-keyword search failing on the second.

**Phase 3.4 — GraphRAG + agent infrastructure.** A basic knowledge-graph retrieval layer (entity/relation extraction, even a simple co-occurrence version, stored as a graph) supporting multi-hop queries that no single chunk answers alone. A minimal agent loop: the gateway can issue a retrieval call, inspect the result, and decide whether to issue another before answering — "reason, act, observe."
*Concept to internalize:* an "agent" here is just retrieval where the LLM decides when to stop — not a fundamentally different technology.

**Phase 3.5 — LLM gateway + semantic caching.** Request routing, rate limiting, and embedding-similarity-based caching (not exact string match).
*Break it:* set the similarity threshold too loose — construct similar-but-distinct query pairs and measure the false-cache-hit rate (wrong answers served silently, not failures). Tighten it and measure the resulting drop in hit rate — a real trade-off, not a free lunch.

**Phase 3.6 — Diff against Weaviate.** No new code: read Weaviate's actual architecture (module system, HNSW choices, hybrid search approach, replication/multi-tenancy) and write up exactly where your version differs and why theirs is built that way.
*Resources:* Weaviate's architecture docs and engineering blog; Microsoft Research's GraphRAG writeups; a basic ReAct paper for the agent-loop framing.

**Gate:** show your GraphRAG layer correctly answering a multi-hop question your pure-vector baseline gets wrong, and name exactly which retrieval step the flat index was missing.

---

### B4 — Model Serving + Distributed Training + Deployment + Capstone (Months 13–24)

**Phase 4.1 — Model serving.** Naive (one request at a time) → request batching → a KV-cache for autoregressive generation (reuse cached attention keys/values instead of recomputing over the whole prefix each step).
*Break it:* measure per-token generation latency vs. sequence length with and without the cache — confirm the gap *grows*, it isn't constant. Study conceptually how vLLM's PagedAttention fixes KV-cache memory fragmentation (treating the cache like virtual-memory pages).
*Resources:* the vLLM paper/blog; conceptual reading on tensor parallelism even without multi-GPU hardware to build it on directly.

**Phase 4.2 — Distributed training.** Data parallelism at small scale (multiple processes, can simulate without multiple GPUs): full model copy per worker, gradient sync via all-reduce before each optimizer step.
*Break it:* artificially slow one worker — confirm a naive synchronous all-reduce makes the *entire system's* throughput bound by the slowest worker (the straggler problem). Kill a worker mid-epoch and handle the failure (restart from checkpoint, or proceed without it).
*Concept:* checkpointing here is the same WAL-derived idea from B1, applied to model weights instead of key-value records.

**Phase 4.3 — Containerization & orchestration.** Dockerize serving and training (understand layers and build cache, not just "it works"). Docker Compose first for multi-service networking, then a minimal single-node Kubernetes (`kind`/`minikube`) for pods/deployments/services as concepts.

**Phase 4.4 — CI/CD + observability.** A pipeline that runs tests, builds an image, and deploys on every commit. Metrics, logs, and basic distributed tracing across the gateway → retrieval → serving chain from B3.
*Break it:* inject an artificial slowdown into one service in that chain, and time how long it takes to find with logs alone vs. with tracing — a concrete demonstration of why observability isn't optional once a system has more than one moving part.
*Resources:* Docker's own docs; Kubernetes' official concepts guide (not a certification course); Prometheus + Grafana for metrics, OpenTelemetry for tracing.

**Phase 4.5 — Capstone (Months 19–24).** Deploy one integrated system using all of it: Lane A's theory for model/loss/optimization choices, B1's storage discipline, B2's replication where it applies, B3's RAG stack, B4's serving/training/deployment substrate. Load-test it and answer, with real measurements: where's the latency, where's the bottleneck, what happens when traffic doubles, what fails first.

**Capstone gate:** name your actual bottleneck from real load-test numbers, and state whether fixing it needs better code, more hardware, or a different architecture — and how you know which.

---

## Part 3 — Complete resource list

**Math (Lane A):** Stewart *Calculus*; Hammack *Book of Proof*; Abbott *Understanding Analysis*; Spivak *Calculus*; Rudin *Principles of Mathematical Analysis*; Hubbard & Hubbard *Vector Calculus, Linear Algebra, and Differential Forms*; Axler *Linear Algebra Done Right*; Strang *Introduction to Linear Algebra* + MIT 18.06; 3Blue1Brown; Boyd & Vandenberghe *Introduction to Applied Linear Algebra*; Blitzstein & Hwang *Introduction to Probability*; Casella & Berger *Statistical Inference*; Hastie/Tibshirani/Friedman *The Elements of Statistical Learning*; Goodfellow/Bengio/Courville *Deep Learning*; Karpathy's micrograd.

**Systems (Lane B):** OSTEP (*Operating Systems: Three Easy Pieces*); Kerrisk *The Linux Programming Interface*; Beej's Guide to Network Programming; Kleppmann *Designing Data-Intensive Applications*; Petrov *Database Internals*; the Bitcask design paper; LevelDB design docs; Redis persistence docs; Ongaro & Ousterhout's Raft paper; the HNSW paper (Malkov & Yashunin); Weaviate's architecture docs; Microsoft Research's GraphRAG writeups; the vLLM paper; Docker and Kubernetes official docs; Prometheus/Grafana and OpenTelemetry docs.

---

This is the whole plan as it stands. The calculus track's full weekly checklists and dependency-map reasoning live in their own files from earlier; everything else — the month-level math summary and the complete systems build — is now here in one place.