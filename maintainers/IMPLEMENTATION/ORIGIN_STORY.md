# Origin Story: The Real History Behind Grapa

Grapa wasn’t born from academia or a product roadmap. It was born from a question:

> **“Could I build a language that can mutate its own grammar at runtime? And could I build a storage engine to support fragmented, update-efficient columnar access?”**

I didn’t know if it was possible. So I built it to find out.

---

## 🎯 Where It Started

In the early 1990s, while co-running CEL Software, I needed to simulate a mainframe environment in order to build a screen-scraping solution — but we didn’t have access to the actual mainframe. So I created a BTree-based system to store snapshots of mainframe screens and simulate interactions.

That BTree engine had to:
- Run on **Mac**, **Windows**, and **OS/2** (very different platforms at the time)
- Be fully **cross-platform** at the file level
- Handle both **BigEndian and LittleEndian** formats to make files portable across systems

The core of that early implementation still exists today in the Grapa BTree engine — including its endian-handling logic.

---

## 🧱 Column Store and the Weighted BTree

As the use cases grew, I needed a way to store large datasets column-wise and still support efficient updates. That led to a new question:

> *How can I store fragmented columnar data and update it in place, without rewriting entire blocks?*

The result was **FREC_DATA** and a **weighted BTree**:
- Nodes carry a “weight” (e.g., byte length or number of items) instead of just a count
- Balancing and traversal are done using those weights
- This allows efficient partial reads, fast inserts and deletes, and update-in-place semantics

This model became the backbone of the COL table support in GrapaDB and is inspired more by file system design than by traditional database index structures.

---

## 🧠 Building the Language

In 2001, as I further developed the DB engine, I needed a better way to **test** it. At first I used hardcoded strings and `if` statements to drive commands — but it quickly became unwieldy. I considered mimicking an existing language, but couldn’t decide which direction to take.

So I created a system to define a language — and figured I’d decide on the high-level syntax later.

That became the original version of Grapa:
> A language for **building languages** — using a self-hosted syntax system defined in `$RULE` variables.

The Grapa language bootstraps itself by compiling its own syntax rules. It was inspired in part by what I remembered from a compiler design class taught by **Professor Jonathan Schaeffer** at the University of Alberta in the 1980s — the same class that gave me the foundational understanding I later applied to both the execution tree and grammar mutation model. His OS course similarly influenced the BTree internals.

---

## 🧪 What Grapa Is Today

Grapa today includes:
- A **runtime-mutable grammar system** driven by `$RULE` global variables
- A **bootstrapped parser** that compiles high-level code into `$OP` execution trees
- A flexible **interpreter** that can optimize or directly execute those trees
- A full **data engine (GrapaDB)** supporting COL, ROW, and GROUP table types
- A **fragment-aware, weighted BTree engine** designed for fast, update-in-place columnar data access

It’s open source, documented, and under review for academic research consideration.

---

## 📚 Why This Matters

Grapa is the product of:
- Engineering persistence
- Real-world systems design
- Curiosity-driven exploration — not compliance with current trends

It now provides a foundation for:
- Language prototyping and DSL composition
- Runtime-evolving syntax systems
- Research in hybrid execution models
- Fragmented column-store storage research

---

## 🔗 Links

- [Grapa GitHub Repository](https://github.com/grapa-dev/grapa)
- [Project Documentation](https://grapa-dev.github.io/grapa/about/)
