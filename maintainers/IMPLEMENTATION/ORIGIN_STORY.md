# Origin Story: The Real History Behind Grapa

Grapa wasn’t born from academia or a product roadmap. It was born from a question:

> **“Could I build a language that can mutate its own grammar at runtime? And could I build a storage engine to support fragmented, update-efficient columnar access?”**

I didn’t know if it was possible. So I built it to find out.

---

## 🎯 Where It Started

In the early 1990s, while co-running CEL Software, I needed to simulate a mainframe environment to build a scraping solution — but we had no access to the mainframe itself. So I wrote a simple BTree engine to store screen captures and replay the terminal session.

That BTree system:
- Ran on 16-bit systems
- Supported both big-endian and little-endian architectures
- Was eventually upgraded to 32-bit, then 64-bit over time

The core of that early system still lives in Grapa’s BTree implementation today.

---

## 🧱 Column Store and the Weighted BTree

Years later, I needed that engine to support columnar storage. That led to a new question:  
> *How can I handle fragmented data updates without rewriting the entire column?*

The answer became **FREC_DATA** and a **weighted BTree**, where each node tracks a weight (e.g. byte length) rather than just key count. Balancing and lookup are based on fragment offsets, enabling fast partial reads and in-place updates.

This became the basis for GrapaDB’s COL table mode.

---

## 🧠 Building the Language

In 2001, when I began building the DB engine in earnest, I needed a way to test it. I started with hardcoded strings, then basic `if` statements, but it quickly became unmanageable. I considered mimicking an existing language — but couldn’t pick one.

So I built a system to define a language — *and decided I’d define the language later.*

That system became the original version of Grapa:  
> A language for building languages — driven by runtime-defined grammar rules.

The syntax system was inspired by what I remembered from a compiler design class at the University of Alberta in the 1980s, taught by Professor Jonathan Schaeffer. He also taught the OS class that helped inspire the BTree internals.

---

## 🧪 What Grapa Is Today

Grapa is now:
- A grammar-mutable, self-hosting language defined via `$RULE` global variables
- An execution tree interpreter (`$OP` structures) that supports optimized and direct evaluation
- A data engine (GrapaDB) supporting COL, ROW, and GROUP tables
- A fragmented column-store engine built around a weighted BTree

It’s open source, documented, and under review as part of a broader academic exploration.

---

## 📚 Why This Matters

This project is grounded in:
- Real-world engineering needs
- Curiosity-driven exploration
- Technical persistence over decades

And now it’s a platform for:
- Research in language mutation, DSLs, and runtime syntax systems
- Storage research on update-in-place columnar databases
- Practical experimentation for system-level developers and students

---

## 🔗 Links

- [Grapa GitHub Repository](https://github.com/grapa-dev/grapa)
- [Project Documentation](https://grapa-dev.github.io/grapa/about/)
