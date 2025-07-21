# Documentation Structure Improvement Plan

**Last Updated:** $(date '+%Y-%m-%d')

## Overview

This document outlines the current state of the Grapa documentation system, its strengths, and a plan for further improvements to achieve best-in-class usability and discoverability.

---

## Current Structure: Strengths

- **Top-Level Clarity:**
  - Main entry points (index, quickstart, API Reference, Python integration, etc.) are at the top level, making them easy to find for new users.
- **Logical Subfolders:**
  - `type/`, `operators/`, `syntax/`, `use_cases/`, `examples/`, `database/`, `obj/`, `sys/`—all group related content, keeping the top level clean.
- **Central API Reference:**
  - The new `api_reference.md` provides a single, intuitive entry point for all types, operators, and core functions.
- **Language Reference:**
  - The `syntax/` directory contains canonical, empirically validated language rules and patterns.
- **Use Cases and Examples:**
  - Real-world scenarios and code samples are easy to find and separated from reference material.
- **Python Integration:**
  - Dedicated guide for Python users, which is a major feature for many.

---

## Next-Level Improvement Plan

1. **Navigation Consistency**
   - Ensure the sidebar/navigation menu in MkDocs matches the logical structure (API Reference, Language Reference, Use Cases, Examples, Python Integration).
   - Group advanced/maintainer docs under a clear “Maintainers” or “Advanced” section (if not already).

2. **Landing Page Enhancements**
   - Add a visual “map” or diagram of the documentation structure for new users on `index.md`.

3. **Cross-Linking**
   - Ensure all major sections (API Reference, Language Reference, Use Cases, Examples) cross-link to each other where relevant.
   - Type/operator pages should link to related use cases or examples.

4. **Search and Tagging**
   - Add “See also” or “Related topics” at the bottom of key pages for better discoverability.

5. **Advanced/Hidden Features**
   - If advanced features exist (meta-programming, custom grammar), create a dedicated “Advanced Topics” or “Meta-Programming” section.

6. **Maintainer/Internal Docs**
   - Periodically review to ensure no internal details leak into user-facing docs.

7. **Onboarding/Quick Links**
   - Consider a “Start Here” or “Onboarding” section for new users, with links to the most important pages (Quickstart, API Reference, Examples).

---

## Summary Table

| Section             | Placement/Structure | Intuitive? | Notes/Improvements                |
|---------------------|--------------------|:----------:|-----------------------------------|
| Landing Page        | index.md           | ✅         | Add visual map if desired         |
| Quickstart          | cli_quickstart.md  | ✅         |                                   |
| API Reference       | api_reference.md   | ✅         | Central index, well-linked        |
| Language Reference  | syntax/            | ✅         | Canonical, clear                  |
| Types/Operators     | type/, operators/  | ✅         | Linked from API Reference         |
| Use Cases/Examples  | use_cases/, examples/ | ✅      | Real-world, discoverable          |
| Python Integration  | python_integration.md | ✅      | Major feature, top-level          |
| Maintainer Docs     | maintainers/       | ✅         | Not indexed in user docs          |

---

## Action Items

- [ ] Review and update MkDocs navigation for consistency
- [ ] Add a visual map/diagram to `index.md`
- [ ] Audit and improve cross-linking between major sections
- [ ] Add “See also”/“Related topics” to key pages
- [ ] Create/expand “Advanced Topics” section if needed
- [ ] Review maintainer/internal docs for separation
- [ ] Add onboarding/quick links section for new users

---

**Reference:** This plan is tracked as the current top priority in `maintainers/DEVELOPMENT/CURRENT_STATUS.md` (before the ROW bug fix). 