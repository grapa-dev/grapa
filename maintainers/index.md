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
- **DEVELOPMENT/BACKLOG.md**: Historical or long-term backlog. Now located in the DEVELOPMENT folder.
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

### 📊 [Project Management](PROJECT_MANAGEMENT/)
Current status, planning, agent onboarding, workflow guidance, and technical debt tracking.

### 🛠️ [Development](DEVELOPMENT/)
Development workflow, progress tracking, roadmaps, and contribution guidelines.

### 📝 [Research and Analysis](RESEARCH_AND_ANALYSIS/)
Work-in-progress research, ongoing investigations, internal analysis, and experimental work.

### 🎓 [Academic Outreach](ACADEMIC_OUTREACH/)
Academic collaboration, student projects, research partnerships, and university engagement.

### 🗄️ [DBX Archive](DBXARCHIVE/)
Experimental DBX content and research for future reference.

### 📋 [Standards & Guidelines](.)
- **[Documentation Standards](DOCUMENTATION_STANDARDS.md)** - Standards for maintainer documentation
- **[Contributor Onboarding](CONTRIBUTOR_ONBOARDING.md)** - Guide for new contributors

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
- [Build System](BUILD_AND_DEPLOYMENT/BUILD_SYSTEM.md) - Complete build system reference
- [Documentation Deployment](BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md) - Documentation deployment process
- [AWS/Docker Setup](BUILD_AND_DEPLOYMENT/SETUPAWSDOCKER.md) - Cloud deployment
- [Docker Manual](BUILD_AND_DEPLOYMENT/BUILD_DOCKER_MANUAL.md) - Docker build manual

### Implementation
- [System Functions](IMPLEMENTATION/SYSTEM_FUNCTIONS.md) - Low-level system APIs
- [Base Types](IMPLEMENTATION/BASETYPES.md) - Core type system
- [Execution Trees](IMPLEMENTATION/EXECUTION_TREES.md) - Execution model
- [Include System](IMPLEMENTATION/INCLUDE_SYSTEM.md) - Module system
- [CLI Implementation](IMPLEMENTATION/GRAPA_CLI_IMPLEMENTATION.md) - Command line interface implementation
- [GRZ Format](IMPLEMENTATION/GRZ_FORMAT.md) - Binary format specification
- [Unicode Grep Status](IMPLEMENTATION/UNICODE_GREP_STATUS.md) - Unicode implementation

### Testing & Quality
- [Production Readiness](TESTING_AND_QUALITY/PRODUCTION_READINESS_ASSESSMENT.md) - Production assessment
- [Ripgrep Parity](TESTING_AND_QUALITY/RIPGREP_PARITY_RESULTS.md) - Ripgrep comparison
- [Testing Framework](TESTING_AND_QUALITY/TESTING.md) - Testing infrastructure
- [Performance Benchmarks](TESTING_AND_QUALITY/PERFORMANCE_BENCHMARKS.md) - Performance testing

### Development
- [Current Status](PROJECT_MANAGEMENT/CURRENT_STATUS.md) - **AUTHORITATIVE**: Single source of truth for all development status
- [Language Analysis](DEVELOPMENT/LANGUAGE_ANALYSIS_AND_IMPROVEMENT_PLAN.md) - Language features analysis and improvement plan
- [Technical Debt](PROJECT_MANAGEMENT/TECHNICAL_DEBT.md) - Quality standards and maintenance tracking
- [CLI Analysis](DEVELOPMENT/CLI_ANALYSIS_AND_IMPROVEMENT_PLAN.md) - CLI functionality analysis and improvement plan
- [Database Analysis](DEVELOPMENT/GRAPADB_ANALYSIS.md) - GrapaDB implementation analysis
- [In-Memory Database Discovery](DEVELOPMENT/GRAPA_IN_MEMORY_DATABASE_DISCOVERY.md) - Language feature discovery

### Project Management
- [Onboarding](PROJECT_MANAGEMENT/ONBOARD.md) - Agent onboarding and developer workflow
- [Backlog](PROJECT_MANAGEMENT/BACKLOG.md) - Development backlog and priorities
- [Migration Summary](PROJECT_MANAGEMENT/MIGRATION_SUMMARY.md) - DBX migration summary
- [Reorganization Summary](PROJECT_MANAGEMENT/REORGANIZATION_SUMMARY.md) - Documentation reorganization summary

### Research and Analysis
- [Bug Investigations](RESEARCH_AND_ANALYSIS/ROW_TABLE_INDEX_BUG_DEBUG_CONTEXT.md) - Database bug investigation
- [CLI Investigation](RESEARCH_AND_ANALYSIS/CLI_FUNCTIONALITY_INVESTIGATION.md) - CLI functionality investigation
- [Optimization Backlog](RESEARCH_AND_ANALYSIS/GRAPA_OPTIMIZATION_BACKLOG.md) - Performance optimization plans
- [Technical Analysis](RESEARCH_AND_ANALYSIS/FORMULA_VERSION_COMPATIBILITY_ANALYSIS.md) - Formula system analysis
- [BNF Review Notes](RESEARCH_AND_ANALYSIS/bnf_review_notes.md) - Grammar enhancement ideas
- [XML BNF Analysis](RESEARCH_AND_ANALYSIS/xml_bnf_analysis.md) - XML grammar analysis

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
- Archive superseded or outdated docs in `ARCHIVED/` and update the index.

**Contributor Checklist:**
- [ ] Purpose and scope stated at the top of the doc
- [ ] Cross-links to related docs
- [ ] Added to the relevant index.md
- [ ] Filename and placement appropriate
- [ ] Old/superseded docs archived if needed

> For folder-specific guidelines, see the `index.md` in each subfolder. 