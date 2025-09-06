---
tags:
  - maintainer
  - lowlevel
  - project-management
  - status
  - planning
---

# Project Management

This section contains core project management documentation, including current status, planning, and workflow guidance.

## Overview

Project Management includes:
- **Current Status**: Single source of truth for project status and priorities
- **Planning**: Development backlog and roadmap planning
- **Workflow**: Agent onboarding and development workflow guidance
- **History**: Migration and project evolution documentation

## Quick Navigation

### **🎯 Single Source of Truth**
- **[CURRENT_STATUS.md](CURRENT_STATUS.md)** - **AUTHORITATIVE**: Single source of truth for all development status, priorities, and progress tracking
- **[ONBOARD.md](ONBOARD.md)** - Agent onboarding and developer workflow documentation
- **[TECHNICAL_DEBT.md](TECHNICAL_DEBT.md)** - Quality standards and maintenance tracking
- **[MIGRATION_SUMMARY.md](MIGRATION_SUMMARY.md)** - Summary of GrapaDBX migration and content organization

### **📋 Planning & Roadmap**
- **[BACKLOG.md](BACKLOG.md)** - Historical or long-term backlog and development priorities
- **[REORGANIZATION_SUMMARY.md](REORGANIZATION_SUMMARY.md)** - Summary of documentation reorganization and content organization
- **[CONSOLIDATION_PLAN.md](CONSOLIDATION_PLAN.md)** - Documentation consolidation planning
- **[INDEX_AUDIT_SUMMARY.md](INDEX_AUDIT_SUMMARY.md)** - Audit summary of index file updates

### **📚 Standards & Guidelines**
- **[DOCUMENTATION_STANDARDS.md](DOCUMENTATION_STANDARDS.md)** - Documentation standards and guidelines for maintainers
- **[CONTRIBUTOR_ONBOARDING.md](CONTRIBUTOR_ONBOARDING.md)** - Contributor onboarding and development guidelines

## Key Documents

### **CURRENT_STATUS.md** - The Authoritative Status Document
This is the **single source of truth** for all project status information:
- **Current Priorities**: What we're working on right now
- **Development Status**: Progress on key features and components
- **Quick Reference**: Build commands and key resources
- **Current Focus**: Primary goals and next priorities

### **ONBOARD.md** - Agent Onboarding Guide
Essential for new agents taking over the project:
- **Project Overview**: Current state and key components
- **Development Workflow**: How to build, test, and deploy
- **Cross-Platform Commands**: Windows vs Mac/Linux differences
- **Key Documentation**: What to read first
- **Common Tasks**: How to handle typical requests

### **TECHNICAL_DEBT.md** - Quality Standards and Maintenance
Tracks ongoing quality concerns and maintenance tasks:
- **Quality Standards**: Code quality, documentation quality, build quality
- **Maintenance Tasks**: Ongoing tasks that need regular attention
- **Quality Metrics**: Measurable standards for project quality
- **Ongoing Processes**: Regular reviews and quality gates

### **BACKLOG.md** - Development Backlog
Long-term planning and feature tracking:
- **Priority Order**: What to work on next
- **Feature Categories**: Language, CLI, database, documentation
- **Status Tracking**: Planned, in progress, completed
- **Dependencies**: What blocks what

### **MIGRATION_SUMMARY.md** - Project Evolution History
Historical context for the project:
- **DBX Migration**: How we separated experimental content
- **Content Organization**: How documentation was reorganized
- **Archive Structure**: What's in DBXARCHIVE and why
- **Lessons Learned**: Key insights from the reorganization

## For AI Assistants

When working on this project:

1. **Always start with CURRENT_STATUS.md** - This is the authoritative source
2. **Check ONBOARD.md** for workflow guidance and cross-platform differences
3. **Reference BACKLOG.md** for long-term planning and priorities
4. **Use MIGRATION_SUMMARY.md** for historical context when needed

## How to Add or Update Documentation

- **Status Updates**: Always update CURRENT_STATUS.md when priorities change
- **New Workflows**: Add to ONBOARD.md for new agent guidance
- **Planning Changes**: Update BACKLOG.md for new priorities
- **Historical Context**: Add to MIGRATION_SUMMARY.md for major changes

**Contributor Checklist:**
- [ ] Purpose and scope stated at the top of the doc
- [ ] Cross-links to related docs
- [ ] Added to this index.md
- [ ] Filename and placement appropriate
- [ ] Old/superseded docs archived if needed

> For global guidelines, see the main maintainers/index.md. 