# Grapa & GrapaPy Persistent TODO (Internal)

This file tracks all ongoing, planned, and recently completed tasks for Grapa and GrapaPy. It is updated regularly to ensure easy recovery after a crash or restart.

---

## High-Priority Goals
- Make Grapa and GrapaPy as inviting and usable as possible for the broadest audience (Python users, educators, researchers, data engineers, etc.)
- **Ensure robust crash recovery by frequently persisting all in-progress work to disk (in docs/internal/WIP/), updating the persistent TODO with WIP references, and always resuming from the last save point after a crash. This is a core assistant responsibility to minimize lost work and maintain continuity, regardless of crash frequency.**
- **CRITICAL: Due to Cursor app crash frequency, update files in docs/internal every 5 minutes (not 15 minutes) to ensure minimal work loss. This includes WIP files, TODO.md, and any other internal tracking documents.**

---

## Current TODOs

### 0. Immediate Next Priority
- [x] **CRITICAL: Fix Grapa code examples in `docs/USE_CASES.md` - current examples use incorrect syntax that is not actual Grapa code. Need to replace with proper Grapa syntax, functions, and comment style.**
- [ ] **Revise the `description` and `long_description` in `setup.py` to be inviting, accurate, and up to date with the README and Python docs.** This is in preparation for an official new release of Grapa and GrapaPy.

### 1. Documentation & Examples
- [x] Fill in detailed examples for each use case in `docs/USE_CASES.md` and `docs/PYTHON_USE_CASES.md` (ETL, BNF/compiler, math, parallelism, scraping, file/db, education)
- [ ] Update and modernize the webscrape example for current Grapa and GrapaPy best practices
- [ ] Add migration tips and quickstarts for Python users in all relevant docs
- [ ] Cross-link all new docs and examples for discoverability
- [ ] **Keep the `description` and `long_description` in `setup.py` up to date and in sync with the README and Python docs.** This metadata is shown on PyPI and is important for discoverability and onboarding. Update whenever docs or project focus changes.
- [ ] Clarify Grapa's thread safety model, shared state design considerations, and the roles of `static` and `const` in the documentation. 
    - Clearly state that Grapa is fully thread safe by design (all variable and data structure updates are internally synchronized; no user locking needed for stability).
    - Add guidance on logical race conditions when sharing mutable state between threads (design issue, not a stability issue).
    - Clarify that `static` is for compile-time evaluation, not thread safety or immutability.
    - Clarify that `const` is for performance (caching/optimization), not for locking, though it prevents modification unless `setconst` is used.
    - Confirm and document (after C++ review) that all datatypes are lockable/unlockable, and update thread.md and relevant docs accordingly.
    - Add best practices and examples for safe shared state and parallelism.

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
- [ ] **Explore and document options for distributing the Grapa command-line tool (`grapa`) for easy installation and use on all supported platforms, including system setup and dependency management.** Research cross-platform packaging, dependency installation, and best practices for CLI tools.
- [ ] **Design and implement a universal, simplified build process for Grapa binaries as described in BUILD.md.** The process should support separate commands for building Grapa, cleaning, and building GrapaPy, to streamline iterative development and testing.

---

## Recently Completed
- ✅ **COMPLETED: Fixed all Grapa code examples in `docs/USE_CASES.md` with correct syntax, functions, and comment style**
- ✅ **COMPLETED: Added comprehensive detailed examples for all 7 use cases in both `docs/USE_CASES.md` and `docs/PYTHON_USE_CASES.md`** (ETL, BNF/compiler, math, parallelism, scraping, file/db, education)
- Created new use case documentation for both Grapa and GrapaPy
- Added "Design Influences" sections to core type docs
- Established persistent TODO tracking for crash recovery
- Persisted advanced context on data interchange and serialization for robust recovery

---

*Update this file as tasks are completed, reprioritized, or added. This ensures easy recovery and continuity after any crash or restart.* 