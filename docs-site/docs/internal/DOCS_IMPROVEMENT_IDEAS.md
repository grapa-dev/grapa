# Grapa Docs Improvement Ideas (Internal)

This document tracks possible advanced improvements and features for the Grapa documentation site, inspired by best-in-class language projects.

---

## Current Top Priorities (as of 2024-06)

### 1. Searchable API Reference
- **Goal:** Create a dedicated, searchable, and filterable API reference page for all Grapa built-in functions, types, and methods.
- **Action Plan:**
  - Review and expand `FUNCTION_QUICK_REFERENCE.md` to ensure all functions/types are included and categorized.
  - Set up a static site generator (e.g., MkDocs Material) with a "Reference" or API plugin.
  - Add search/filter UI, examples, and cross-links for each entry.

### 2. Advanced Search and Tagging
- **Goal:** Enable full-text search across all docs, with tag-based filtering for topics, types, and use cases.
- **Action Plan:**
  - Add metadata/tags to each doc (e.g., "ETL", "math", "file", "Python integration").
  - Configure the search and tagging system in the docs generator.
  - Test search and tag filtering for discoverability.

### 3. Mobile-Friendly and Responsive Design
- **Goal:** Ensure the documentation site is fully usable and visually appealing on mobile devices and tablets.
- **Action Plan:**
  - Use a modern, responsive theme (e.g., mkdocs-material, Docusaurus, Sphinx with a responsive theme).
  - Test the docs site on various devices and screen sizes.
  - Adjust navigation, code blocks, and tables for mobile usability.

> **Note:** Review/setup the static site generator (e.g., MkDocs Material) as a prerequisite if not already done. This will enable the above features and provide a foundation for further improvements.

---

## Advanced Documentation Features & Enhancements (Backlog)

- **Interactive Playground/REPL:**
  - Embed a web-based Grapa REPL or code playground in the docs for live code experimentation.

- **Versioned Documentation:**
  - Host docs for multiple Grapa versions (stable, latest, legacy) so users on older versions can find accurate docs.

- **Edit on GitHub / Report Issue Buttons:**
  - Add buttons to every doc page for easy editing or reporting issues.

- **Guided Tutorials and Learning Paths:**
  - Curated, step-by-step tutorials for common workflows, with “Next”/“Previous” navigation.

- **Video Walkthroughs and Screencasts:**
  - Short videos demonstrating installation, first script, Python integration, and advanced features.

- **Community Showcase and Recipes:**
  - Section for user-contributed scripts, recipes, and real-world use cases.

- **FAQ and Troubleshooting Wizard:**
  - Expand FAQ and add a “wizard” or flowchart for common troubleshooting steps.

- **Accessibility and Internationalization:**
  - Ensure docs are accessible (screen reader friendly, high contrast, etc.) and consider translations for key pages.

- **Automated Doc Testing:**
  - All code examples in docs are automatically tested as part of CI.

- **Release Notes and Upgrade Guides:**
  - Dedicated section for release notes, upgrade/migration guides, and breaking changes.

- **Contributor Recognition:**
  - Highlight top contributors, recent PRs, and community involvement. (Current and primary contributor: Chris Matichuk)

- **Analytics (Privacy-Respecting):**
  - Track which docs are most/least used (with user privacy in mind) to prioritize improvements.

---

## [2024-06] FUNCTION_QUICK_REFERENCE.md Upgraded

- Added Table of Contents for fast navigation.
- Added onboarding callouts for Python users and newcomers/non-Python users, with direct links to migration and onboarding resources.
- Added a 'See Also' box at the end with links to Getting Started, Migration Guides, Examples, Troubleshooting, and System Functions (Detailed).
- Inserted 'Back to Top' links after every major section for easy navigation.
- Improved cross-linking, especially for functional programming and parallelism (with links to best practices and migration guides).
- Highlighted parallelism and production-readiness in the Functional Programming section.
- All changes align with best-in-class documentation goals and recent improvements to README, EXAMPLES.md, GETTING_STARTED.md, and migration guides.

---

## [2024-06] Chat-to-Docs Context Capture

- All major documentation and process changes are logged in docs/internal/DOCS_IMPROVEMENT_IDEAS.md for continuity after restarts or environment changes.
- Extensions installed for optimal workflow: CMake Tools, Markdown All in One, Markdownlint, Spell Right, GitLens, YAML Support. C++ extension (ms-vscode.cpptools) not found in Cursor marketplace; check for alternatives if C++ language support is needed.
- Workflow: Use minimal, focused extensions for docs and code/test/debug. Disable unnecessary extensions to reduce crashes.
- Cursor crash reduction steps: keep app updated, increase memory limits, clear cache/state, monitor system resources, toggle hardware acceleration, check for conflicting software, check logs, reinstall if needed.
- Reducing chat size/history can improve stability. All important context and next steps should be captured in docs/internal, so extensive chat history is not required.
- After restart, rebuild context from docs/internal, which is updated frequently to ensure seamless task resumption.

---

*Update this file as new ideas arise or features are implemented. Use as a roadmap for evolving the Grapa documentation site to best-in-class standards.* 