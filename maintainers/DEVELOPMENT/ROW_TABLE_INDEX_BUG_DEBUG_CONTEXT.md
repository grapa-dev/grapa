# ROW Table Index Corruption Bug – Debug Context

**Purpose:**
This document provides context for the ongoing effort to debug the "ROW Table Index Corruption Bug" in GrapaDB.

## Summary
- **Bug:** Adding a third record to a ROW table in GrapaDB causes the first record to become unreadable (index corruption).
- **Goal:** Debug and fix the ROW table index corruption issue.

## What Was Attempted
- Created `debug/debug_row/` directory as an isolated minimal C++ project.
- Copied all required GrapaDB source files, headers, and third-party dependencies.
- Attempted to build a standalone debug environment.
- Encountered linker errors due to missing external libraries (OpenSSL, FLTK, BLST, PCRE2, UTF8PROC).

## Decision Made
- **Isolated debug project approach ABANDONED** due to impractical external library dependencies.
- The GrapaDB codebase is tightly integrated with OpenSSL (crypto operations), FLTK (GUI), and other fundamental libraries.
- Stripping these dependencies would require significant refactoring and likely break the code being debugged.
- **Solution:** Return to debugging within the main project environment.

## Current Status
- Debugging will continue in the main Grapa project where all dependencies are properly configured.
- The isolated debug project (`debug/debug_row/`) can be removed or kept for reference.

## Next Steps
- Debug the ROW table index corruption bug directly in the main project.
- Use existing test scripts and debugging tools within the main codebase.
- Focus on the core BTree and database logic where the bug likely resides.

---
**Note:** This isolated approach was not viable due to deep library integration. Main project debugging is the practical solution. 