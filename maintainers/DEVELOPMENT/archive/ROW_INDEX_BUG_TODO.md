# ROW Table Index Corruption Bug – Persistent TODO Tracker

This file tracks actionable investigation and fix tasks for the ROW Table Index Corruption Bug. It is updated after each iteration and serves as a quick entry point for maintainers or AI agents. For deep technical context, see `archive/ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md` (archived, closed investigation).

---

## Current TODOs (2025-07-22)

- **[Completed]** All index corruption and GROUP issues resolved; all table types pass their tests.
- **[Completed]** All DB/BTree edge cases validated; no outstanding issues remain.

---

## Next Steps

- **[Completed]** DB/BTree investigation is closed. No further action required unless new issues are discovered.

---

## Backlog

- Evaluate all instances of null, true, and false handling in Grapa (especially for .get(), .set(), and field assignment) to ensure consistency across all types and operations. Reference the recent RAW/null test as an example.
- Automate running the full regression suite (run_tests_comprehensive.grc and Python integration tests) as part of CI to ensure all features and integrations are validated on every change.

---

## Closing Note

All index corruption, GROUP, and DB/BTree issues are resolved and validated. Investigation is closed. No open DB/BTree bugs remain as of 2025-07-22. 