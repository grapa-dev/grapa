# Origin Story: The Real History Behind Grapa

Grapa wasn’t born from academia or a product roadmap. It was born from a question:

> “Could I build a language that can mutate its own grammar at runtime? And could I build a storage engine to support fragmented, update-efficient columnar access?”

I didn’t know if it was possible. So I built it to find out.

---

## 🎯 Where It Started

In the early 1990s, I needed to simulate a mainframe environment to build a scraping solution — without access to the mainframe. So I created a cross-platform BTree-based system to store and simulate terminal screen captures.

That early BTree system had to run on **Mac**, **Windows**, and **OS/2**, and its files had to be **cross-platform compatible**. As a result, it was designed with support for both **BigEndian and LittleEndian** formats — a feature still present in Grapa today.

---

## 🧱 Column Store and the Weighted BTree

As needs grew, I asked a harder question:

> *How can I store fragmented columnar data and update it in place, without rewriting entire blocks?*

This led to the creation of **FREC_DATA** and the **weighted BTree** model:

- Nodes carry a “weight” (e.g., byte length) instead of just a count
- Balancing and traversal are guided by weights and fragment offsets
- Enables fast partial reads, inserts, and in-place updates

This architecture powers the COL table format in GrapaDB.

---

## 🧠 Building the Language

In 2001, I needed a better way to test the DB engine. I started with hardcoded logic, but soon built a language to define the syntax — without choosing a specific language to mimic.

That became Grapa: a **language for building languages**, using `$RULE` variables to define syntax at runtime.

The concept was inspired by what I remembered from using **YACC** in a compiler design class at the University of Alberta in the 1980s. Grapa goes beyond YACC: it is an **executable runtime syntax-mutable grammar language** — to my knowledge, the first of its kind.

At the time, I didn’t know if there would be a real use for it. I did it because I was curious if it could be done.

---

## 🧪 What Grapa Is Today

- A **runtime-mutable grammar system** defined by `$RULE` global variables
- A **bootstrapped parser** compiling to `$OP` execution trees
- A **fragment-aware weighted BTree engine** powering COL, ROW, and GROUP tables
- An open-source system used in production-grade ETL and AWS Lambda workflows

---

## 📚 Why This Matters

Grapa is the result of long-term engineering, curiosity, and persistence. It is:

- A real-world alternative to traditional language and storage design
- A research-grade system for DSLs, storage, and runtime composition
- A foundation for academic and production-grade exploration

---

## 🔗 Links

- [Grapa GitHub Repository](https://github.com/grapa-dev/grapa)
- [Project Documentation](https://grapa-dev.github.io/grapa/about/)
