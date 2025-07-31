# Documentation Update Guide

This guide ensures proper documentation updates and prevents clutter, duplication, and disorganization.

## 🚨 CRITICAL: Preserve Documentation Structure

**The documentation structure is carefully organized. Respect it.**

## When User Says "Update Relevant Docs"

### Step 1: Determine Content Type

**What type of content are you updating?**

- **Investigation Results** → `maintainers/RESEARCH_AND_ANALYSIS/`
- **Technical Implementation** → `maintainers/IMPLEMENTATION/`
- **Analysis & Planning** → `maintainers/DEVELOPMENT/`
- **Status Changes** → `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **Bug Reports** → `maintainers/RESEARCH_AND_ANALYSIS/`
- **New Features** → `maintainers/IMPLEMENTATION/` or `maintainers/DEVELOPMENT/`

### Step 2: Search for Existing Documentation

**ALWAYS search before creating new docs:**

1. **Search by topic** in the appropriate directory
2. **Check for similar files** that might cover the same topic
3. **Look for existing analysis** that could be updated
4. **Check for related investigations** that might be relevant

### Step 3: Update vs Create Decision

**Update existing doc if:**
- Topic is already covered
- Content is related to existing analysis
- Investigation builds on previous work
- Technical details extend existing implementation

**Create new doc only if:**
- No existing doc covers the topic
- Content is completely new and distinct
- Investigation is unrelated to existing work

### Step 4: Proper Placement

#### For Investigation Results
- **File Location**: `maintainers/RESEARCH_AND_ANALYSIS/`
- **Naming**: `TOPIC_INVESTIGATION.md` or `TOPIC_ANALYSIS.md`
- **Content**: Findings, evidence, conclusions, recommendations

#### For Technical Implementation
- **File Location**: `maintainers/IMPLEMENTATION/`
- **Naming**: `TOPIC_IMPLEMENTATION.md` or `TOPIC_GUIDE.md`
- **Content**: Technical details, implementation notes, architecture

#### For Analysis & Planning
- **File Location**: `maintainers/DEVELOPMENT/`
- **Naming**: `TOPIC_ANALYSIS.md` or `TOPIC_PLAN.md`
- **Content**: Analysis, planning, recommendations, roadmaps

#### For Status Updates
- **File Location**: `maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md`
- **Content**: ONLY status changes, priority updates, completion notes

## ❌ Common Mistakes to Avoid

### 1. **Dumping Everything in CURRENT_STATUS.md**
- **NEVER** add investigation details to `CURRENT_STATUS.md`
- **NEVER** add implementation details to `CURRENT_STATUS.md`
- **NEVER** add analysis results to `CURRENT_STATUS.md`
- **ONLY** add status changes, priority updates, completion notes

### 2. **Creating Duplicate Documentation**
- **NEVER** create a new doc if topic is already covered
- **ALWAYS** search for existing docs first
- **ALWAYS** update existing docs rather than creating new ones

### 3. **Wrong Directory Placement**
- **NEVER** put investigation results in `IMPLEMENTATION/`
- **NEVER** put technical details in `RESEARCH_AND_ANALYSIS/`
- **NEVER** put status updates in `DEVELOPMENT/`

### 4. **Ignoring Documentation Hierarchy**
- **ALWAYS** respect the directory structure
- **ALWAYS** use the proper directory for content type
- **ALWAYS** update index files when adding new docs

## ✅ Best Practices

### 1. **Search First, Create Second**
```markdown
1. Search for existing docs on the topic
2. Check if content should be added to existing doc
3. Create new doc only if no existing doc covers the topic
4. Update index files to include new docs
```

### 2. **Preserve CURRENT_STATUS.md**
```markdown
CURRENT_STATUS.md should ONLY contain:
- Active work item status changes
- Priority changes
- Completion of work items
- New critical issues discovered
```

### 3. **Cross-Reference Documents**
```markdown
- Link related documents together
- Reference relevant analysis in implementation docs
- Reference implementation details in analysis docs
- Keep documentation network connected
```

### 4. **Update Index Files**
```markdown
- Update directory index.md files when adding new docs
- Maintain proper navigation structure
- Keep documentation discoverable
```

## 📋 Documentation Update Checklist

**Before updating documentation:**

- [ ] **Determined content type** (investigation, implementation, analysis, status)
- [ ] **Searched for existing docs** on the topic
- [ ] **Decided whether to update existing or create new**
- [ ] **Chose correct directory** for content type
- [ ] **Preserved CURRENT_STATUS.md** (only status changes)
- [ ] **Cross-referenced related documents**
- [ ] **Updated index files** if adding new docs

## 🔄 Periodic Cleanup

**Even with these guidelines, periodic cleanup is still needed:**

1. **Monthly Review**: Check for documentation drift
2. **Quarterly Audit**: Review for duplicates and inconsistencies
3. **Annual Restructure**: Major reorganization if needed

**This is normal and expected. The guidelines reduce the frequency and severity of cleanup needed.**

---

**This guide helps maintain documentation quality and reduces cleanup work.** 