# Welcome to Grapa Documentation

> **Tip:** Use the search bar (top right) to quickly find functions, guides, and examples. The search automatically filters to show only user documentation.

---

## 📚 Documentation Structure Map (Interactive)

<div class="mermaid">
graph TD;
    A[Start Here]
    B1[Quickstart]
    B2[API Reference]
    B3[Examples]
    B4[Use Cases]
    B5[Migration Guides]
    B6[Tools & Utilities]
    B7[Troubleshooting]
    A --> B1
    A --> B2
    A --> B3
    A --> B4
    A --> B5
    A --> B6
    A --> B7
    %% API Reference children
    B2 --> C1[Data Types]
    B2 --> C2[Operators]
    B2 --> C3[System Functions]
    B2 --> C4[Object Types]
    B2 --> C5[Database]
    %% Language Reference branch
    B1 -.-> D1[Language Reference]
    B2 -.-> D1
    B3 -.-> D1
    D1 --> D2[Basic Syntax]
    D1 --> D3[Operators]
    D1 --> D4[Precedence]
    D1 --> D5[Grammar Design]
    D1 --> D6[Data Types]
    %% Clickable links (relative)
    click B1 "cli_quickstart/" "Go to Quickstart"
    click B2 "api_reference/" "Go to API Reference"
    click B3 "examples/" "Go to Examples"
    click B4 "use_cases/" "Go to Use Cases"
    click B5 "migrations/" "Go to Migration Guides"
    click B6 "run/" "Go to Tools & Utilities"
    click B7 "troubleshooting/" "Go to Troubleshooting"
</div>

*This map is interactive. Click any major section to navigate.*

---

## Getting Started
- [Quickstart Guide](getting_started.md)
- [API Reference](api_reference.md)
- [Migration Guides](migrations/python_to_grapa_migration.md)
- [Examples](examples.md)

## Advanced Topics
- [Grammar Design](syntax/grammar_design.md) - Create your own languages with Grapa

## Key Features
- **Dynamic Code Execution**: Compile and execute code at runtime with `op()(<string>)()` and `$sys().eval()`
- **Execution Trees**: Direct manipulation of `$OP` objects for advanced meta-programming
- **Parallel Processing**: Built-in parallel data operations with `.map()`, `.filter()`, `.reduce()`
- **Unlimited Precision**: Native support for arbitrary-precision arithmetic
- **Cross-Platform**: Runs on Windows, macOS, and Linux

## New Features
- **Searchable API Reference:** Find all built-in functions, types, and methods in one place.
- **Tag-based Filtering:** Use tags in the API Reference and throughout the docs to filter by topic, type, or use case.
- **Mobile-Friendly:** This site is fully responsive—try it on your phone or tablet!

## Need Help?
- [Troubleshooting](troubleshooting.md)

---

*Grapa: High-performance scripting for data, ETL, and automation.*

---

> **Note:** After this round of improvements, we will review all documentation to determine where additional visuals, diagrams, or flowcharts would be most helpful for users.
