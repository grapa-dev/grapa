---
tags:
  - maintainer
  - lowlevel
  - internal
---

# Maintainers & Internal Documentation

## 📂 Maintainers Directory Organization

This section documents the organization and purpose of the `maintainers/` directory:

- **README.md**: Maintainer onboarding, project overview, and navigation. *Should remain at top level.*
- **index.md**: Quick navigation and category summary for all maintainer docs. *Should remain at top level for discoverability.*
- **BACKLOG.md**: Historical or long-term backlog. If not referenced in current workflows, consider moving to `DEVELOPMENT/` or `INTERNAL_NOTES/`.
- **.DS_Store**: System file, can be deleted.

### Subfolders:
- **BUILD_AND_DEPLOYMENT/**: Build instructions, dependency management, packaging, deployment, and cloud setup.
- **DEVELOPMENT/**: Development workflow, progress tracking, roadmaps, session context, and contribution guidelines. (Key doc: `CURRENT_STATUS.md`)
- **IMPLEMENTATION/**: Core technical implementation details, system functions, type system, execution model, and technical specs.
- **TESTING_AND_QUALITY/**: Testing frameworks, production readiness, performance benchmarks, and quality assurance.
- **INTERNAL_NOTES/**: Work-in-progress docs, internal analysis, improvement ideas, operator/optimization analysis, and archived WIP.
- **ADVANCED_TOPICS/**: Specialized or advanced topics (e.g., binary grep, grammar/BNF development, core extensions, debugging).

> **Note:** Each subfolder contains its own `index.md` for navigation and document summaries specific to that area.

> For a summary of current status, priorities, and tasks, see [`DEVELOPMENT/CURRENT_STATUS.md`](DEVELOPMENT/CURRENT_STATUS.md).

---

Welcome to the maintainers section. This area contains all documentation relevant to building, maintaining, and extending Grapa. **General users do not need these files.**

## Quick Navigation

### 🏗️ [Build & Deployment](BUILD_AND_DEPLOYMENT/)
Build instructions, dependencies, packaging, and production deployment guides.

### ⚙️ [Implementation](IMPLEMENTATION/)
Core implementation details, system functions, execution model, and technical specifications.

### 🧪 [Testing & Quality](TESTING_AND_QUALITY/)
Testing frameworks, production readiness assessment, performance benchmarks, and quality assurance.

### 🛠️ [Development](DEVELOPMENT/)
Development workflow, progress tracking, roadmaps, and contribution guidelines.

### 📝 [Internal Notes](INTERNAL_NOTES/)
Work-in-progress documentation, improvement ideas, and internal analysis.

### 🔬 [Advanced Topics](ADVANCED_TOPICS/)
Specialized topics like binary grep implementation, grammar development, and core extensions.

## For AI Assistants

When referencing maintainer content, use these paths:
- **Build issues**: `maintainers/BUILD_AND_DEPLOYMENT/`
- **Implementation details**: `maintainers/IMPLEMENTATION/`
- **Testing/quality**: `maintainers/TESTING_AND_QUALITY/`
- **Development workflow**: `maintainers/DEVELOPMENT/`
- **Internal analysis**: `maintainers/INTERNAL_NOTES/`
- **Specialized features**: `maintainers/ADVANCED_TOPICS/`

## Key Documents by Category

### Build & Deployment
- [Build Instructions](BUILD_AND_DEPLOYMENT/BUILD.md) - Complete build process
- [Dependencies](BUILD_AND_DEPLOYMENT/DEPENDENCIES.md) - External library management
- [AWS/Docker Setup](BUILD_AND_DEPLOYMENT/SETUPAWSDOCKER.md) - Cloud deployment
- [Packaging](BUILD_AND_DEPLOYMENT/PACKAGING.md) - Package manager support
- [Deployment](BUILD_AND_DEPLOYMENT/DEPLOYMENT.md) - Production deployment

### Implementation
- [System Functions](IMPLEMENTATION/SYSTEM_FUNCTIONS.md) - Low-level system APIs
- [Base Types](IMPLEMENTATION/BASETYPES.md) - Core type system
- [Execution Trees](IMPLEMENTATION/EXECUTION_TREES.md) - Execution model
- [Include System](IMPLEMENTATION/INCLUDE_SYSTEM.md) - Module system
- [GRZ Format](IMPLEMENTATION/GRZ_FORMAT.md) - Binary format specification
- [Unicode Grep Status](IMPLEMENTATION/UNICODE_GREP_STATUS.md) - Unicode implementation

### Testing & Quality
- [Production Readiness](TESTING_AND_QUALITY/PRODUCTION_READINESS_ASSESSMENT.md) - Production assessment
- [Ripgrep Parity](TESTING_AND_QUALITY/RIPGREP_PARITY_RESULTS.md) - Ripgrep comparison
- [Testing Framework](TESTING_AND_QUALITY/TESTING.md) - Testing infrastructure
- [Performance Benchmarks](TESTING_AND_QUALITY/PERFORMANCE_BENCHMARKS.md) - Performance testing

### Development
- [Implementation Progress](DEVELOPMENT/IMPLEMENTATION_PROGRESS.md) - Progress tracking
- [Language Roadmap](DEVELOPMENT/grapa_language_enhancement_roadmap.md) - Feature roadmap
- [Development Status](DEVELOPMENT/CURRENT_STATUS.md) - **AUTHORITATIVE**: Single source of truth for all development status
- [Contributing Guidelines](DEVELOPMENT/CONTRIBUTING.md) - How to contribute
- [Code Review Process](DEVELOPMENT/CODE_REVIEW.md) - Review guidelines

### Internal Notes
- [Documentation Ideas](INTERNAL_NOTES/DOCS_IMPROVEMENT_IDEAS.md) - Docs improvement plans
- [XML BNF Analysis](INTERNAL_NOTES/xml_bnf_analysis.md) - XML grammar analysis
- [BNF Review Notes](INTERNAL_NOTES/bnf_review_notes.md) - Grammar enhancement ideas
- [Archived WIP](INTERNAL_NOTES/ARCHIVED_WIP/) - Historical work-in-progress files

### Advanced Topics
- [Binary Grep Implementation](IMPLEMENTATION/BINARY_GREP.md) - Binary data processing (now in Implementation)
- [Grammar Development] - (planned/future)
- [Core Extensions] - (planned/future)
- [Debugging Guide] - (planned/future)

---

**Note:** This section is organized to provide clear separation between user-facing documentation and internal/maintainer content. The search automatically filters to show only maintainer documentation. For user documentation, return to the [main documentation](../). 

## How to Add or Update Documentation

- Before creating a new doc, check if it fits as a section in an existing canonical doc.
- Every new doc must be added to the relevant `index.md` with a one-line summary.
- Use clear, descriptive filenames and place docs in the most relevant folder.
- Cross-link related docs in other folders as needed.
- If a doc is experimental or WIP, mark it as such in the index.
- Archive superseded or outdated docs in `ARCHIVED_WIP/` and update the index.

**Contributor Checklist:**
- [ ] Purpose and scope stated at the top of the doc
- [ ] Cross-links to related docs
- [ ] Added to the relevant index.md
- [ ] Filename and placement appropriate
- [ ] Old/superseded docs archived if needed

> For folder-specific guidelines, see the `index.md` in each subfolder. 