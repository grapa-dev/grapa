# 🏁 Agent Handoff Checklist & Build/Deploy Quick Reference

**For all maintainers and AI assistants:**
- Always ingest and review this file (`CURRENT_STATUS.md`) for the latest project status, priorities, and progress.
- For directory lookup, navigation, and folder purposes, see [`maintainers/index.md`](../index.md).
- For detailed technical plans and implementation docs, see the [Reference Documents](#reference-documents) section below.
- Review `maintainers/README.md` for onboarding and maintainer guidance.
- Update this file at logical breakpoints or after major steps.

**Build Grapa binaries:**
- `./build.sh` (wraps `python3 build.py`)
**Build & deploy documentation:**
- On Linux/Mac: `./scripts/deploy_docs.sh`
- On Windows:   `.\scripts\deploy_docs.ps1`
(Both scripts build docs and push to GitHub Pages)
- For more details or troubleshooting, see the deployment guide in `BUILD_AND_DEPLOYMENT/`.

---

# 🚨 IMMEDIATE PRIORITY TASKS

## 1. ROW Table Index Corruption Bug
- **Status:** CRITICAL, in progress (debug in main project)
- **Reference:** [`ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md`](ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md)
- **Test Script:** `test_row_bug_demo.grc`
- **Workaround:** Use COL tables instead of ROW tables

---

## 🚀 Next-Up Tasks (Ready to Start)

1. **Fix $file (and $TABLE) .get() Return Types:**  
   ~~Ensure that INT and FLOAT fields from .get() return the correct type instead of $STR/raw bytes. (Current workaround: use `.raw().int()` or `.raw().float()`. See known issue.)~~ **[COMPLETED]**
2. **Language System Enhancements:**  
   Design and implement loop constructs, exception handling, and a module system.

---

# 📋 MEDIUM/LONG-TERM ROADMAP
- See [BACKLOG.md](BACKLOG.md) for additional future and long-term tasks to pull from.

---

# ✅ COMPLETED ITEMS

- **Operator System:** All 26 operators fully functional and tested
- **All critical operator bugs resolved and tested**
- **All 5 missing operators documented**
- **CLI executable enhancement completed**
- **Core BTree functionality validated**
- **Documentation search bug resolved**
- **Documentation site reorganization and navigation updates**
- **Test organization and logical subdirectory structure**
- **Grapheme cluster regression fixed**
- **Float comparison system improvements**
- **Main branch replacement and branch cleanup**
- **Windows ARM64 support explicitly excluded**

---

# 🔧 MAINTENANCE TASKS
- **Documentation link fixes:** Ongoing
- **Test organization documentation:** Pending

---

# 🛠️ TECHNICAL DEBT & QUALITY
- **Debug Output:** Disable `GRAPA_DEBUG_PRINTF` before committing
- **Test Monitoring:** Run full test suite after any changes
- **Documentation:** Keep docs in sync with implementation changes
- **Error Handling:** Ensure proper error handling in all operations
- **Memory Management:** Verify no memory leaks
- **Edge Cases:** Test with edge cases and malformed data

---

# 📚 REFERENCE DOCUMENTS
- **[CLI Redesign Plan](../CLI_REDESIGN_PLAN.md)**
<!-- [Language Enhancement Roadmap](../INTERNAL_NOTES/ARCHIVED/LANGUAGE_ENHANCEMENT_ROADMAP.md) (archived; see CURRENT_STATUS.md for all language priorities) -->
- **[Operator Bug Fixes Status](../INTERNAL_NOTES/OPERATOR_BUG_FIXES_STATUS.md)**
- **[Optimization Backlog](../INTERNAL_NOTES/GRAPA_OPTIMIZATION_BACKLOG.md)**
- **[Documentation Deployment Guide](../BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md)**
- **[Archived Files](../INTERNAL_NOTES/ARCHIVED/)**

---

# 📞 CONTACT & COORDINATION
- Update this file whenever build system, platform, or major architectural changes are made.
- All maintainers: keep this file up to date for agent handoff and onboarding.

---

# 📝 DOCUMENT SUMMARY

This document provides a single authoritative source for:
- Immediate, next-up, and medium/long-term tasks (including all language, cryptographic, optimization, and CLI priorities)
- Completed items and major project milestones
- Maintenance and technical debt tracking
- Build/deploy quick reference for maintainers and agents
- Reference links to detailed plans, roadmaps, and implementation docs
- Contact and coordination guidance for maintainers and AI agents

**Last Updated:** January 2025
**Current Focus:** 🚨 ROW Table Index Corruption Bug (IMMEDIATE PRIORITY)
**Status:** CLI Redesign Complete, ROW Bug Critical, Operator System Complete 