# Onboarding Process Safeguards

This document protects the working onboarding process that has been established for the Grapa project.

## 🚨 CRITICAL: DO NOT BREAK THE ONBOARDING PROCESS

The onboarding process is **CRITICAL INFRASTRUCTURE** that enables efficient agent handoffs. Any changes that break this process will cause significant disruption.

## Protected Files

**NEVER modify these files without explicit user permission:**

1. `maintainers/PROJECT_MANAGEMENT/ONBOARD.md` - The main onboarding guide
2. `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md` - Authoritative project status
3. `maintainers/PROJECT_MANAGEMENT/BACKLOG.md` - Development priorities
4. `maintainers/index.md` - Directory navigation
5. `maintainers/README.md` - Maintainer guidance
6. `docs-src/docs/syntax/basic_syntax.md` - Canonical syntax reference

## Protected Processes

**NEVER change these working processes:**

1. **Discovery Path Order**: The 6-step discovery path must remain in this exact order
2. **"onboard" Command**: Must continue to work with a simple "onboard" command
3. **Build Commands**: `python build.py --exe-only` and `python3 build.py --exe-only` must work
4. **CLI Functionality**: `.\grapa.exe -h` and `./grapa -h` must show correct options
5. **Cross-Platform Support**: Both Windows and Mac/Linux commands must be documented

## Validation Requirements

**Before making ANY changes to onboarding files:**

1. **Get explicit user permission** for the specific change
2. **Test the build system** - verify `python build.py --exe-only` works
3. **Test the CLI** - verify `.\grapa.exe -h` works
4. **Test the discovery path** - verify all 6 steps are accessible
5. **Test cross-platform commands** - verify both Windows and Mac/Linux versions
6. **Test the "onboard" command** - verify it still works in a new session

## Common Pitfalls to Avoid

**DO NOT:**

- Remove or modify the safeguards in `ONBOARD.md`
- Change the discovery path order or structure
- Remove cross-platform command documentation
- Claim build commands work without testing them
- Claim CLI features work without verifying them
- Modify canonical syntax without empirical testing
- Break internal documentation links
- Remove the "onboard" command functionality

## 🚨 AGENT SWITCHING PROTECTION

**CRITICAL: Agent/Model switching during complex processes can cause incomplete states and rogue actions.**

### Agent Switching Detection
**If you detect you are a new agent (no context from previous steps):**

1. **STOP immediately** - Do not proceed with any actions
2. **Request onboarding** - Say "I need to onboard to understand the current context"
3. **Ask for context** - Request the user to provide context about what was being worked on
4. **Verify state** - Check if the system is in an incomplete state
5. **Wait for guidance** - Do not make assumptions about what should be done next

### Incomplete State Detection
**Signs you may be dealing with an incomplete state:**

- User mentions "continuing" or "next step" but you have no context
- User refers to previous work you don't know about
- User mentions specific files or changes you can't see
- User asks about progress on something you don't understand
- User seems frustrated or confused by your responses

### Safe Response Protocol
**When you detect agent switching or incomplete context:**

```
"I notice I may be missing context from previous work. To ensure I don't make incorrect assumptions or cause issues:

1. Could you briefly describe what was being worked on?
2. Are there any incomplete changes or processes I should be aware of?
3. Should I onboard first to understand the current project state?

This will help me avoid making rogue actions or incomplete changes."
```

### User Protection Guidelines
**For users to protect against agent switching issues:**

1. **Backup critical files** before starting complex multi-step processes
2. **Document current step** - note what step you're on in complex processes
3. **Check agent context** - if agent seems confused, ask them to onboard
4. **Provide context** - briefly describe what was being worked on
5. **Verify state** - confirm the system state before proceeding
6. **Use explicit commands** - be very specific about what you want done

## 📝 Documentation Update Guidelines

**When user says "update relevant docs", agents MUST:**

### 1. **Preserve `CURRENT_STATUS.md`**
- **NEVER** add investigation details to `CURRENT_STATUS.md`
- **NEVER** add implementation details to `CURRENT_STATUS.md`
- **NEVER** add analysis results to `CURRENT_STATUS.md`
- **ONLY** update `CURRENT_STATUS.md` for:
  - Active work item status changes
  - Priority changes
  - Completion of work items
  - New critical issues discovered

### 2. **Find Existing Documentation**
- **ALWAYS** search for existing docs before creating new ones
- **ALWAYS** update existing docs rather than creating duplicates
- **ALWAYS** check the documentation hierarchy:
  - `maintainers/IMPLEMENTATION/` for technical details
  - `maintainers/RESEARCH_AND_ANALYSIS/` for investigation results
  - `maintainers/DEVELOPMENT/` for analysis and planning
  - `maintainers/PROJECT_MANAGEMENT/` for status and priorities

### 3. **Proper Documentation Placement**
- **Investigation Results**: `maintainers/RESEARCH_AND_ANALYSIS/`
- **Technical Implementation**: `maintainers/IMPLEMENTATION/`
- **Analysis and Planning**: `maintainers/DEVELOPMENT/`
- **Status Updates**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **New Features**: `maintainers/IMPLEMENTATION/` or `maintainers/DEVELOPMENT/`
- **Bug Reports**: `maintainers/RESEARCH_AND_ANALYSIS/`

### 4. **Documentation Update Process**
1. **Search existing docs** for the topic
2. **Update existing doc** if found
3. **Create new doc** only if no existing doc covers the topic
4. **Update `CURRENT_STATUS.md`** only for status changes
5. **Cross-reference** related documents
6. **Update index files** if adding new documents

## Emergency Recovery

**If the onboarding process is broken:**

1. **STOP** all changes immediately
2. **Revert** to the last known working state
3. **Test** the "onboard" command in a new session
4. **Verify** all discovery path steps work
5. **Document** what caused the break
6. **Add safeguards** to prevent future breaks

## Success Criteria

The onboarding process is working correctly when:

- ✅ User can say "onboard" and agent follows discovery path
- ✅ Agent loads `CURRENT_STATUS.md` first, then `ONBOARD.md`
- ✅ Agent can build executable with correct commands
- ✅ Agent can test CLI functionality
- ✅ Agent understands cross-platform differences
- ✅ Agent follows canonical syntax rules
- ✅ Agent can access all key documentation files

## Maintenance Guidelines

**When updating onboarding documentation:**

1. **Preserve the discovery path** - don't change the order
2. **Test everything** - don't assume it works
3. **Verify cross-platform** - don't break Mac/Linux support
4. **Update safeguards** - add new protections as needed
5. **Document changes** - explain what was modified and why

---

**This document is part of the critical onboarding infrastructure. Protect it.** 