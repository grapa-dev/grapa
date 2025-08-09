# Documentation Updates Needed

**Purpose:** Track required updates to user-facing documentation (`docs-src`) that need to be implemented after features are completed.

**Note:** This file tracks changes needed to user-facing docs. Do NOT make changes to `docs-src` until after implementation is complete.

---

## Session-Specific Environment Variables

### **Status:** 📋 **PENDING IMPLEMENTATION**
- **Feature:** Session-specific environment variables for `$sys().getenv()` and `$sys().putenv()`
- **Implementation:** [`SESSION_SPECIFIC_ENVIRONMENT_VARIABLES_PLAN.md`](../RESEARCH_AND_ANALYSIS/SESSION_SPECIFIC_ENVIRONMENT_VARIABLES_PLAN.md)
- **Priority:** High

### **Required Documentation Updates**

#### **1. `docs-src/docs/sys/sys.md`**
**Section:** `getenv(type)` and `putenv(type, value)`

**Current Content:**
```markdown
### getenv(type)
Gets environment variables and system information.

**Note:** Any value not starting with `$` will be directed to the native OS `getenv()` function.
```

**Required Updates:**
- Add section on session-specific environment variables
- Document that session variables take precedence over OS environment variables
- Add examples showing session isolation
- Update note to mention session variable behavior

**New Content Needed:**
```markdown
### getenv(type)
Gets environment variables and system information.

**Session-Specific Variables:** Session-specific environment variables (set via `putenv()`) take precedence over OS environment variables. Each Grapa session maintains its own isolated set of environment variables.

**Note:** Any value not starting with `$` will first check session-specific variables, then fall back to the native OS `getenv()` function.
```

**Section:** `putenv(type, value)`

**Current Content:**
```markdown
### putenv(type, value)
Sets environment variables and system information.

**Note:** Like `getenv()`, any value not starting with `$` will be directed to the native OS `putenv()` function.
```

**Required Updates:**
- Document that non-system variables are stored session-specifically
- Add examples showing session isolation
- Update note to mention session-specific storage

**New Content Needed:**
```markdown
### putenv(type, value)
Sets environment variables and system information.

**Session-Specific Storage:** Environment variables not starting with `$` (non-system variables) are stored session-specifically. These variables are isolated to the current Grapa session and do not affect other sessions or the global OS environment.

**Note:** Like `getenv()`, any value not starting with `$` will be stored session-specifically (not directed to native OS `putenv()`).

**Examples:**
```grapa
/* Set session-specific debug flags */
$sys().putenv("DEBUG_MODE", "true");
$sys().putenv("LOG_LEVEL", "verbose");

/* These are isolated to this session only */
if ($sys().getenv("DEBUG_MODE") == "true") {
    "Session debug mode enabled".echo();
}

/* Parallel sessions can have different values */
/* Session A: */ $sys().putenv("SESSION_ID", "A");
/* Session B: */ $sys().putenv("SESSION_ID", "B");
/* Each session sees only its own variables */
```

#### **2. `docs-src/docs/cli_quickstart.md`**
**Section:** Environment variables and debugging

**Required Updates:**
- Add examples of using session-specific environment variables for debugging
- Show how to set debug flags per session
- Demonstrate parallel session debugging

**New Content Needed:**
```markdown
## Session-Specific Debugging

Grapa supports session-specific environment variables for isolated debugging and configuration:

```grapa
/* Set session-specific debug flags */
$sys().putenv("DEBUG_MODE", "true");
$sys().putenv("LOG_LEVEL", "verbose");

/* These settings only apply to the current session */
```

This allows multiple developers or scripts to run with different debug configurations simultaneously without interference.
```

### **Implementation Notes**
- ✅ **Technical Plan Complete:** [`SESSION_SPECIFIC_ENVIRONMENT_VARIABLES_PLAN.md`](../RESEARCH_AND_ANALYSIS/SESSION_SPECIFIC_ENVIRONMENT_VARIABLES_PLAN.md)
- 📋 **Documentation Updates Tracked:** This file
- ⏳ **User Docs Update:** Pending implementation completion
- 🎯 **Target:** Update user-facing docs after C++ implementation and testing

---

## Future Documentation Updates

### **Template for New Features**
```markdown
## [Feature Name]

### **Status:** 📋 **PENDING IMPLEMENTATION**
- **Feature:** [Description]
- **Implementation:** [Link to plan]
- **Priority:** [Priority level]

### **Required Documentation Updates**

#### **1. `docs-src/docs/[relevant-file].md`**
**Section:** [Specific section]

**Current Content:**
[Current content that needs updating]

**Required Updates:**
[Specific updates needed]

**New Content Needed:**
[New content to add]

### **Implementation Notes**
- ✅ **Technical Plan Complete:** [Link to plan]
- 📋 **Documentation Updates Tracked:** This file
- ⏳ **User Docs Update:** Pending implementation completion
``` 