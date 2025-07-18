# Current Investigation Status

## Completed Investigations

### 1. Database String Key Retrieval Issue (RESOLVED)
**Status:** RESOLVED - Workaround implemented
**Date:** July 2024
**Issue:** String key retrieval fails in ROW tables after first record
**Root Cause:** Index corruption bug in `SetRecordField` when updating indexes
**Workaround:** Use COL tables instead of ROW tables for string key operations
**Documentation:** Added warning in `docs-site/docs/TROUBLESHOOTING.md`

### 2. Documentation Site Deployment (RESOLVED)
**Status:** RESOLVED - All pages working
**Date:** July 18, 2024
**Issue:** GitHub Pages deployment issues, EXAMPLES page returning 404
**Root Cause:** GitHub Pages configured to serve from `/docs` folder in gh-pages branch, but files were in wrong location
**Solution:** 
- Moved EXAMPLES directory to root of gh-pages branch
- Cleaned up gh-pages branch (removed development files)
- Cleaned up main branch (removed docs-old backup)
**Result:** All documentation pages now accessible at https://grapa-dev.github.io/grapa/

## Active Investigations

None currently.

## Backlog Items

### 1. $KEY Field Datatype Investigation
**Priority:** Medium
**Description:** Investigate allowing any datatype for the $KEY field instead of hardcoded string type
**Status:** Pending
**Notes:** Current workaround is to use .raw().int() or .raw().float() for numeric keys

### 2. .match Method Implementation
**Priority:** Low
**Description:** Add .match method for strings/regex to replace .grep(pattern, "x").len() > 0 workaround
**Status:** Pending
**Notes:** Current workaround works but is not ergonomic

## Notes

- All test files have been organized into logical subdirectories
- Documentation system is fully functional with user/maintainer separation
- GitHub Pages deployment is working correctly
- Database workaround is documented for users 