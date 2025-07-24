# Grapa Language Programmer Friendliness & Adoption Plan

> **Note:** Per project policy, do not add links or references from user-facing docs (`docs-src`) to this plan or any other maintainer/internal documentation. All roadmap/process content must remain in `maintainers/`.

## Introduction & Goals

This document outlines a comprehensive plan to make the Grapa language more programmer friendly and to increase adoption. The goal is to audit the current language, summarize migration findings, identify pain points, and propose concrete improvements for syntax, usability, and onboarding.

## Audit of Current Language Features

### Comment Handling
- Only block comments (`/* ... */`) are supported. Line comments (`//`, `#`) are not.
- Comments are allowed between/after/before commands and as standalone lines, but not inside class definitions, parameter lists, or array/object literals.
- See [COMMENT_HANDLING_GUIDE.md](COMMENT_HANDLING_GUIDE.md) for full details and BNF references.

### Loop Syntax
- No `for`/`foreach`/`range` loops. Use `while`, `.range()`, `.map()`, `.filter()`, `.reduce()`.
- Example: `i = 0; while (i < arr.len()) { ...; i += 1; }` or `(n).range(0,1).map(op(i) { ... })`.
- Migration docs highlight this as a key difference and pain point for new users.

### Error Handling
- No `try/catch`. Use `.iferr()` for fallback or check for `$ERR` type.
- Example: `result = some_operation().iferr(0);`

### Module/Import System
- No formal module/import system. Use `$file()` for file operations and `$sys().eval()` for dynamic code execution.

### Syntax Ergonomics
- No string interpolation (use concatenation: `"Hello " + name`).
- No attribute-style access for dict/list keys (use `[]` or dot notation for $LIST/$OBJ).
- All statements and blocks must end with a semicolon (`;`).
- No implicit truthy/falsy; use explicit boolean checks.
- No regular expressions as first-class objects (use `.grep()` for pattern matching).

### Onboarding & Documentation
- Migration docs for Python, JS, Go, etc. provide idiom mapping and highlight differences.
- Canonical syntax reference: `docs/docs/syntax/basic_syntax.md`.
- Onboarding and About pages in user docs.

## Migration Doc Findings (Summary)

### Python
- No `for`/`foreach` loops; use `while` or `.range()`+functional methods.
- No `try/catch`; use `.iferr()`.
- Only block comments, always on their own line.
- `.get()` only for `$file`/`$TABLE`.
- No string interpolation; use concatenation.
- Many Python features are available via different mechanisms or are not needed due to Grapa's design.

### JavaScript
- No `for`/`foreach` loops; use `while` or `.range()`+functional methods.
- No `try/catch`; use `.iferr()`.
- Only block comments, always on their own line.
- `.get()` only for `$file`/`$TABLE`.
- No template literals; use concatenation.
- No destructuring, spread, or optional chaining; use explicit property access and assignment.

### Go
- No `for`/`range` loops; use `while` or `.range()`+functional methods.
- Only block comments, always on their own line.
- `.get()` only for `$file`/`$TABLE`.
- No static typing; dynamic typing with runtime checks.
- No package/import system; use file system and dynamic eval.

## Known Pain Points & User Feedback
- Lack of line comments (`//`, `#`).
- No `for`/`foreach`/`range` loop syntax; functional style is unfamiliar to many.
- No string interpolation or template literals.
- No module/import system for code reuse.
- Error handling is functional, not exception-based.
- Some migration idioms require more verbose code.
- Onboarding could be improved with more real-world examples and recipes.

## Proposed Improvements
- **Comment Handling:** Consider adding support for line comments (`//`, `#`) if feasible in the parser.
- **Loop Syntax:** Explore adding a `for` or `foreach` loop construct as syntactic sugar for `.range()`/`.map()` patterns.
- **String Interpolation:** Add support for template literals or a simple interpolation syntax.
- **Module/Import System:** Design a basic import/include system for code reuse.
- **Onboarding:** Expand onboarding docs, add more real-world examples, and improve migration guides.
- **Error Handling:** Consider adding a `try/catch`-like construct or improving `.iferr()` ergonomics.
- **IDE/Tooling:** Improve error messages, add syntax highlighting, and provide better feedback for common mistakes.

## Adoption Strategy
- Prioritize improvements that lower the learning curve for new users (especially those coming from Python, JS, Go).
- Maintain migration guides and keep them up to date with language changes.
- Solicit user feedback and pain points via issues/PRs.
- Document all new features and idioms in the canonical syntax guide and onboarding docs.
- Provide clear upgrade/migration paths for breaking changes.

## Canonical Syntax Reference & Ongoing Maintenance

- The [Basic Syntax Guide](../../docs-src/docs/syntax/basic_syntax.md) is the canonical, empirically tested reference for all Grapa syntax and access patterns. The access patterns table and unsupported syntax summary in that file are authoritative for all type access, loop, comment, and syntax rules.
- As the Grapa language improves with the recommendations in this plan, this document, the Basic Syntax Guide, and all code samples throughout `docs-src` must be updated to reflect new syntax and features.
- **All Grapa code samples in documentation must be empirically tested as .grc scripts.**
- After any language change, perform a full scan of all samples in `docs-src` for possible revisions and re-test all .grc scripts to ensure compliance and correctness.
- Periodically review and update all documentation and samples to maintain strict alignment with the latest language rules and best practices.

## Documentation & Sample Maintenance Policy

- **Policy:** Any change to the Grapa language (syntax, semantics, features, or deprecations) requires:
  - Immediate update of the [Basic Syntax Guide](../../docs-src/docs/syntax/basic_syntax.md) and any other canonical syntax docs.
  - Full scan of all `.md` files in `docs-src` for Grapa code samples that may need revision.
  - Empirical re-testing of all `.grc` code samples to ensure compliance and correctness.
  - Update of migration docs and this plan if new features address previous pain points.
  - A checklist or sign-off step in the PR/release process for documentation and sample updates.

- **Automation Recommendation:**
  - Consider creating a script or CI job to extract all Grapa code blocks from `docs-src` `.md` files, run them as `.grc` scripts, and report failures or outdated patterns.

- **Maintainer Checklist:**
  - After any language change, you must:
    - Update `basic_syntax.md` and migration docs.
    - Scan and update all code samples in documentation.
    - Re-test all `.grc` scripts.
    - Update this plan if new features address previous pain points.
    - Cross-link all relevant docs.

- **Periodic Review:**
  - Schedule a periodic (e.g., quarterly) review of all documentation and samples, even if no major language changes have occurred, to catch any drift or missed updates.

- **Cross-linking:**
  - Ensure `basic_syntax.md`, this plan, migration docs, and the comment handling guide all reference each other for rationale, canonical rules, and roadmap.

## References
- [COMMENT_HANDLING_GUIDE.md](COMMENT_HANDLING_GUIDE.md)
- Migration docs: [python_to_grapa_migration.md](../../docs-src/docs/migrations/python_to_grapa_migration.md), [js_to_grapa_migration.md](../../docs-src/docs/migrations/js_to_grapa_migration.md), [go_to_grapa_migration.md](../../docs-src/docs/migrations/go_to_grapa_migration.md)
- [BACKLOG.md](BACKLOG.md) (see items on comment handling, loop syntax, string interpolation, onboarding)
- [Basic Syntax Guide](../../docs-src/docs/syntax/basic_syntax.md) 