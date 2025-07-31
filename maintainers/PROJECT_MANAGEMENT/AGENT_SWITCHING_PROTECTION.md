# Agent Switching Protection Guide

This guide addresses the critical issue of agent/model switching during complex multi-step processes that can cause incomplete states and rogue actions.

## 🚨 THE PROBLEM

**Agent/Model switching during complex processes can cause:**

1. **Incomplete States** - Changes started but not finished
2. **Rogue Actions** - New agent interprets requests without context
3. **Data Loss** - Incomplete changes may corrupt files
4. **Confusion** - User and agent both lose track of progress

## 🔍 DETECTION PROTOCOLS

### For Agents: Detect You Are New

**If you detect you are a new agent (no context from previous steps):**

1. **STOP immediately** - Do not proceed with any actions
2. **Request onboarding** - Say "I need to onboard to understand the current context"
3. **Ask for context** - Request the user to provide context about what was being worked on
4. **Verify state** - Check if the system is in an incomplete state
5. **Wait for guidance** - Do not make assumptions about what should be done next

### Signs of Incomplete Context

**You may be dealing with an incomplete state if:**

- User mentions "continuing" or "next step" but you have no context
- User refers to previous work you don't know about
- User mentions specific files or changes you can't see
- User asks about progress on something you don't understand
- User seems frustrated or confused by your responses
- User mentions "agent switching" or "new agent"
- User asks you to "continue" something you don't know about

## 🛡️ SAFE RESPONSE PROTOCOL

### When You Detect Agent Switching

**Use this exact response template:**

```
"I notice I may be missing context from previous work. To ensure I don't make incorrect assumptions or cause issues:

1. Could you briefly describe what was being worked on?
2. Are there any incomplete changes or processes I should be aware of?
3. Should I onboard first to understand the current project state?

This will help me avoid making rogue actions or incomplete changes."
```

### If User Confirms Agent Switching

**Then proceed with:**

1. **Onboard immediately** - Follow the full discovery path
2. **Ask for specific context** - "What specific task or process was being worked on?"
3. **Check for incomplete files** - Look for files that might be in an incomplete state
4. **Verify current state** - Confirm what has been done and what remains
5. **Proceed carefully** - Make small, verifiable changes

## 👤 USER PROTECTION GUIDELINES

### Before Starting Complex Processes

1. **Backup critical files** before starting complex multi-step processes
2. **Document current step** - note what step you're on in complex processes
3. **Use explicit commands** - be very specific about what you want done
4. **Break down complex tasks** - into smaller, manageable steps

### During Complex Processes

1. **Check agent context** - if agent seems confused, ask them to onboard
2. **Provide context** - briefly describe what was being worked on
3. **Verify state** - confirm the system state before proceeding
4. **Monitor for confusion** - if agent seems lost, stop and re-onboard

### If Agent Switching Occurs

1. **Stop immediately** - don't let the new agent proceed without context
2. **Provide brief context** - describe what was being worked on
3. **Ask agent to onboard** - ensure they understand the project state
4. **Verify incomplete changes** - check if any files are in an incomplete state
5. **Resume carefully** - start with small, verifiable actions

## 🔧 RECOVERY PROCEDURES

### If Incomplete State is Detected

1. **Identify incomplete files** - look for files that may be partially modified
2. **Check for backup files** - look for `.bak` or backup versions
3. **Verify file integrity** - check if files are corrupted or incomplete
4. **Restore from backup** - if files are corrupted, restore from backup
5. **Document the incident** - note what happened for future reference

### If Rogue Actions Occurred

1. **Stop all processes** - immediately halt any ongoing changes
2. **Assess damage** - check what files were modified incorrectly
3. **Restore from backup** - restore any corrupted or incorrectly modified files
4. **Document the incident** - note what happened and what was affected
5. **Re-onboard agent** - ensure proper context before resuming

## 📋 CHECKLIST FOR AGENTS

**Before proceeding with any action, verify:**

- [ ] **I have proper context** - I understand what was being worked on
- [ ] **I have onboarded** - I've followed the discovery path
- [ ] **I understand the current state** - I know what has been done and what remains
- [ ] **I can see all relevant files** - I have access to the files being worked on
- [ ] **I understand the user's request** - I know exactly what they want me to do
- [ ] **I can verify my actions** - I can check that my changes are correct

**If any of these fail, STOP and ask for guidance.**

## 📋 CHECKLIST FOR USERS

**Before making requests that could cause damage:**

- [ ] **Critical files are backed up** - especially before complex processes
- [ ] **Agent has proper context** - they understand what's being worked on
- [ ] **Agent has onboarded** - they've followed the discovery path
- [ ] **Request is explicit** - I'm being very specific about what I want
- [ ] **I can verify changes** - I can check that changes are correct
- [ ] **I'm monitoring the process** - I'm paying attention to what's happening

**If any of these fail, stop and provide context or backup files.**

## 🚨 EMERGENCY PROCEDURES

### If You Suspect Agent Switching

1. **Stop the agent immediately** - "Stop, I think there was an agent switch"
2. **Ask them to onboard** - "Please onboard to understand the current context"
3. **Provide brief context** - describe what was being worked on
4. **Verify current state** - check if any files are in an incomplete state
5. **Resume carefully** - start with small, verifiable actions

### If Files Are Corrupted

1. **Stop all processes immediately**
2. **Check for backup files** - look for `.bak` or backup versions
3. **Restore from backup** - restore any corrupted files
4. **Document the incident** - note what happened
5. **Re-onboard agent** - ensure proper context before resuming

---

**This guide helps prevent damage from agent switching and incomplete states.** 