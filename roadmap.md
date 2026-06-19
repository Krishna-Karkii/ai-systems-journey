# The Two-Year Build (Claude)

### Master architecture: math + ML theory, alongside scalable systems

This is the document sitting above the calculus-level roadmaps. It explains why two parallel lanes, what's in each, how they meet, and exactly what's being deferred to its own deep-dive later. At 25+ hrs/week for ~104 weeks, the budget is roughly 2,600+ hours — large enough to do this properly, but only with deliberate sequencing. Nothing here is padded for the sake of looking thorough; every phase has a reason for its position.

---

## Why two lanes instead of one queue

Running calculus → linear algebra → probability → ML → systems as a strict sequence would work, but it wastes the time budget: linear algebra doesn't depend on calculus, and the entire systems-engineering track (shell, KV store, Raft, vector DB, LLM gateway — already in motion) doesn't depend on *any* of the math. Splitting into two concurrent lanes means neither one sits idle waiting on the other, and 25+ hrs/week is enough to run both without starving either one of focus.

**Lane A — Math foundations → ML theory.** Mostly sequential internally, because each piece is a real prerequisite for the next: calculus feeds linear algebra's analytic tools, both feed probability and multivariable calculus, all three feed classical ML, which feeds deep learning.

**Lane B — Systems → scalable ML infrastructure.** The existing build sequence (shell → persistent KV store → Raft → HNSW vector DB → LLM gateway) runs almost entirely independently of Lane A, extended in year two with the pieces that specifically require Lane A's ML theory to be ready: model serving, distributed training, and the final capstone.

Rough hours split: **~60% Lane A / 40% Lane B in year one**, flipping to **~40% Lane A / 60% Lane B in year two** as the math matures into something there's actually enough of to build with.

---

## Lane A — Math foundations → ML theory

### A1 — Calculus + Linear Algebra (Months 1–4)
Calculus runs as the hybrid described above: Stewart for problem speed, Abbott and Spivak for the proofs, logic and number-construction compressed to the minimum needed to read those two books. Linear algebra starts in parallel from week one, since it shares no prerequisite with calculus.

- **Primary (proof-first):** Axler — *Linear Algebra Done Right*. Builds vector spaces, linear maps, and eigenvalues abstractly, deliberately avoiding the determinant-heavy approach most courses lean on — closer in spirit to Abbott/Spivak than to a computational linear algebra course.
- **Primary (computational + applications):** Strang — *Introduction to Linear Algebra*, paired with the MIT 18.06 OCW lectures. Builds the intuition for matrix decompositions (LU, QR, eigendecomposition, SVD) that ML actually leans on.
- **Visualization:** 3Blue1Brown — *Essence of Linear Algebra*.
- **ML bridge:** Boyd & Vandenberghe — *Introduction to Applied Linear Algebra* (free), for the specifically data-oriented treatment.

**Milestone, end of Month 4:** prove a basic property of eigenvalues from the definition (not by citing it), derive the SVD's existence for a real matrix, and explain — having just proved both parts of FTC in the calculus track — why "rate of change" and "rank" turn out to be the two ideas everything downstream is built from.

### A2 — Multivariable Calculus + Probability & Statistics (Months 4–7)
Multivariable calculus now has its prerequisite (linear algebra) in place, so the Jacobian, the Hessian, and the implicit function theorem land as direct generalizations rather than new notation to memorize. Probability starts once integration is solid, since continuous distributions need it immediately.

- **Multivariable:** Hubbard & Hubbard (as already chosen) — total derivative as a linear map, multivariable Taylor, implicit/inverse function theorems.
- **Probability, primary:** Blitzstein & Hwang — *Introduction to Probability* (Harvard Stat 110), chosen for the same reason Abbott was chosen for analysis: it motivates every definition with the problem it solves, while still proving everything. Free lecture videos exist alongside it.
- **Probability, rigor companion:** Casella & Berger — *Statistical Inference*, for the mathematical-statistics depth (estimators, sufficiency, hypothesis testing) Blitzstein & Hwang doesn't fully cover.
- **Deferred, not skipped:** measure-theoretic probability. Full rigor on continuous random variables (σ-algebras, Lebesgue integration) is real first-principles territory, but it's the same kind of trade-off as the Dedekind cuts — valuable, not load-bearing for the 2-year goal. Optional enrichment, not a gate.

**Milestone, end of Month 7:** derive the maximum likelihood estimator for a specific distribution from first principles, and connect "likelihood" back to the calculus track's optimization machinery (it's a function you're finding a maximum of, by the exact same Fermat/Rolle/MVT logic).

### A3 — Classical ML + Deep Learning (Months 7–12)
Everything upstream now exists: calculus for optimization, linear algebra for representations, probability for loss functions and uncertainty. This is where it gets used, not just referenced.

- **Classical ML:** Hastie, Tibshirani, Friedman — *The Elements of Statistical Learning* (free PDF) for the rigorous treatment of linear/logistic regression, SVMs, trees and ensembles, and dimensionality reduction, each derived rather than asserted.
- **Deep learning theory:** Goodfellow, Bengio, Courville — *Deep Learning* (already chosen for the ML-calculus phase), now read in full rather than just Ch. 4 and 6 — backprop, CNNs, RNNs, regularization, generalization.
- **Implementation:** Karpathy's micrograd (already chosen), extended into a small autograd-backed MLP, then a minimal Transformer implementation once attention is covered — this is the bridge from "I can derive this" to "I can build this."

**Milestone, end of Month 12:** train a small neural network using an optimizer you implemented yourself (not a framework's), and explain, citing the descent lemma by name, why your choice of learning rate either does or doesn't satisfy the convergence guarantee.

### A4 — Applied ML Theory (Months 13–24, as needed)
Once Lane B's later blocks demand specific theory (efficient inference needs quantization theory; a recommender system needs matrix factorization and ranking losses; an LLM-serving system needs attention variants and KV-cache theory), Lane A becomes demand-driven rather than running on its own fixed schedule. The math foundation from year one is what makes this possible — reading a new paper's background section stops being the bottleneck.

---

## Lane B — Systems → scalable ML infrastructure

### B1 — Unix Shell + Persistent KV Store with WAL (Months 1–5)
Already defined in the existing roadmap. Builds the systems fundamentals (processes, file I/O, durability) that every later project assumes without re-explaining.

### B2 — Raft-Based Distributed KV Store (Months 5–9)
Already defined. The largest single project in year one — consensus, leader election, log replication — and the direct prerequisite for understanding why distributed training and serving systems are hard for reasons that have nothing to do with ML itself.

### B3 — HNSW Vector Database + LLM Gateway with Semantic Caching (Months 9–13)
Already defined. This is where Lane B starts becoming ML-specific rather than general distributed-systems — embeddings, approximate nearest-neighbor search, and a caching layer in front of model inference.

### B4 — Model Serving + Distributed Training + Capstone (Months 13–24) — new
The extension that makes "scalable systems with ML models" concrete rather than aspirational.

- **Model serving:** request batching, GPU scheduling, quantized inference, autoscaling under load — building a serving layer for a real model, then load-testing it.
- **Distributed training:** data parallelism and model parallelism, gradient synchronization (all-reduce), fault tolerance when a worker dies mid-epoch — implemented at small scale (a handful of processes or GPUs) so the mechanics are visible rather than hidden inside a framework.
- **Capstone (Months 19–24):** one project that has to use all of it — Lane A's theory (loss functions, model architecture, optimization), B1–B3's infrastructure (storage, consensus, caching, retrieval), and B4's serving/training work, deployed somewhere that can take real load. Candidates: a self-hosted LLM-serving platform with caching, batching, and autoscaling, or a recommendation system trained and served at scale. The choice can wait until Lane A's deep learning phase clarifies which one is more interesting.

---

## How the lanes meet

Lane B's B1–B3 never wait on Lane A — they're general distributed-systems skills that stand on their own. B4 is the deliberate seam: model serving and distributed training are systems problems, but designing them well requires knowing what you're serving and training, which is exactly what A3 (classical ML + deep learning) supplies. That's why B4 starts at Month 13, right after A3 finishes.

---