# Grapa & GrapaPy Persistent TODO (Internal)

This file tracks all ongoing, planned, and recently completed tasks for Grapa and GrapaPy. It is updated regularly to ensure easy recovery after a crash or restart.

---

## High-Priority Goals
- Make Grapa and GrapaPy as inviting and usable as possible for the broadest audience (Python users, educators, researchers, data engineers, etc.)

---

## Current TODOs

### 1. Documentation & Examples
- [ ] Fill in detailed examples for each use case in `docs/USE_CASES.md` and `docs/PYTHON_USE_CASES.md` (ETL, BNF/compiler, math, parallelism, scraping, file/db, education)
- [ ] Update and modernize the webscrape example for current Grapa and GrapaPy best practices
- [ ] Add migration tips and quickstarts for Python users in all relevant docs
- [ ] Cross-link all new docs and examples for discoverability

### 2. Python Integration
- [ ] Highlight and document Grapa’s parallelism features ($thread, $net, map/reduce/filter) for Python users
- [ ] Add examples showing unlimited precision math from Python
- [ ] Expand ETL/data science examples with Pandas/NumPy integration

### 3. Education & Research
- [ ] Add BNF/compiler learning examples for professors and students
- [ ] Document Grapa’s design influences (Lisp, Prolog) and unique strengths for educators

### 4. Advanced Data Interchange & Serialization
- [ ] Document actual return type mapping from Grapa to Python as implemented in mainpy.cpp (not always string; depends on type and precision)
- [ ] Clarify strategies for unlimited precision types and complex structures: use $global, serialization (.str()), and deserialization as needed
- [ ] Explain limitations and workarounds for $XML/$HTML and other non-native types (e.g., return as string or dict, but not native Python XML/HTML)
- [ ] Confirm and document Grapa’s ability to import any standard JSON/XML/HTML, and identify any gaps or edge cases
- [ ] Document Grapa’s extended syntax for JSON/XML/HTML (e.g., $OP in $ARRAY/$LIST) and implications for round-tripping to Python
- [ ] Provide examples of serialization/deserialization workflows for advanced users
- [ ] Note that users can construct serialized Grapa data types in Python and have Grapa deserialize them

### 5. General Improvements
- [ ] Review and update all test files for consistency with new standards
- [ ] Solicit and incorporate user feedback for further improvements

---

## Recently Completed
- Created new use case documentation for both Grapa and GrapaPy
- Added "Design Influences" sections to core type docs
- Established persistent TODO tracking for crash recovery
- Persisted advanced context on data interchange and serialization for robust recovery

---

*Update this file as tasks are completed, reprioritized, or added. This ensures easy recovery and continuity after any crash or restart.* 