# ROW Table Index Corruption Bug – Persistent TODO Tracker

This file tracks actionable investigation and fix tasks for the ROW Table Index Corruption Bug. It is updated after each iteration and serves as a quick entry point for maintainers or AI agents. For deep technical context, see `archive/ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md` (archived, closed investigation).

---

## Current TODOs (2024-07-22)

- **[REOPENED]** ROW Table Index Corruption Bug is NOT fixed; bug still reproduces as of latest test. Investigation is active.
- Revalidate the fix (recCursor.mValue for index entries), add debug output to SetRecordField and index update logic, and rerun test/test_row_bug_demo.grc.
- **[Completed]** All index corruption and GROUP issues resolved; all table types pass their tests.
- **[Completed]** All DB/BTree edge cases validated; no outstanding issues remain.

---

## Next Steps

- Continue investigation and debugging until the bug is resolved and validated by passing tests.

---

## Backlog

- Evaluate all instances of null, true, and false handling in Grapa (especially for .get(), .set(), and field assignment) to ensure consistency across all types and operations. Reference the recent RAW/null test as an example.
- Automate running the full regression suite (run_tests_comprehensive.grc and Python integration tests) as part of CI to ensure all features and integrations are validated on every change.

---

## Closing Note

The ROW Table Index Corruption Bug is NOT fixed as of 2024-07-22. Investigation is active and ongoing. See the main investigation log for details and next steps. 