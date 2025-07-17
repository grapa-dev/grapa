# Grapa Grep Debugging Workflow Reminder

## Debug Output Management

### Before Debugging:
1. **Enable debug output** by uncommenting the define in `source/grep/grapa_grep_unicode.hpp`:
   ```cpp
   #define GRAPA_DEBUG_PRINTF
   ```

### After Debugging:
1. **Disable debug output** by commenting out the define:
   ```cpp
   //#define GRAPA_DEBUG_PRINTF
   ```

## Current Status
- **Debug output**: ENABLED (uncommented)
- **Last debug session**: Started - investigating invert_match issue

## Debug Workflow
1. Enable debug output
2. Rebuild Grapa
3. Run tests with debug output
4. Analyze debug output
5. Make fixes
6. Test fixes
7. Disable debug output
8. Rebuild for production

## Critical Issues to Fix
1. **invert_match** (v flag) - BROKEN
2. **count_only** (c flag) - FIXED (documented behavior)

## Notes
- Debug output should only be enabled during debugging sessions
- Always remember to disable before committing changes
- Debug output can significantly impact performance
- Count-only grep behavior is now documented in:
  - `docs-site/docs/obj/grep.md` (main documentation)
  - `docs-site/docs/PYTHON_TO_GRAPA_MIGRATION.md` (Python migration)
  - `docs-site/docs/JS_TO_GRAPA_MIGRATION.md` (JavaScript migration) 