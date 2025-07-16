# Internal Documentation Index and Roadmap

This file summarizes all internal (non-user-facing) docs in `docs/internal/`, with brief descriptions and recommendations for the order in which to pursue enhancements or investigations.

---

## Internal Docs Index

- **xml_bnf_analysis.md**
  - *Summary and analysis of Grapa's XML BNF support, current limitations, and recommendations for full XML compliance.*
- **grapa_language_enhancement_roadmap.md**
  - *Tracks general language feature gaps, most useful additions, Python integration opportunities, and unique Grapa strengths. Includes a living TODO section.*
- **bnf_performance_analysis.md**
  - *Analysis of BNF patterns that may impact parse/compile speed, with suggestions for optimization.*
- **bnf_maintainability.md**
  - *Assessment of BNF clarity, modularity, and organization, with recommendations for maintainability and extensibility.*
- **bnf_language_evolution.md**
  - *Proposals for BNF-level changes to enable new language features, better error messages, or more robust grammars.*
- **eval_error_handling_todo.md** (planned)
  - *Plan for improving C++ error handling in eval, preventing crashes, and making $ERR diagnostics more useful.*
- **web_playground_wasm_planning.md** (planned)
  - *Feasibility and planning for a Grapa web playground using WebAssembly. Notes the challenge of binary size and multi-platform dependencies, as seen in the large grapapy-0.0.25.tar.gz Python extension, which must bundle all C++ and third-party libraries for all platforms. This will be a key consideration for a browser-based Grapa.*

---

## Recommended Order of Pursuit

1. **BNF Performance Analysis**
   - Improving parse/compile speed will benefit all users and make future BNF changes safer.
2. **BNF Maintainability**
   - Refactoring for clarity/modularity will make it easier to implement new features and fix bugs.
3. **BNF Language Evolution**
   - Once performance and maintainability are solid, pursue new features or grammar enhancements.
4. **XML BNF Enhancements**
   - Tackle after general BNF improvements, as XML is a complex and widely used format.
5. **Language Enhancement Roadmap**
   - Use as a living reference and update as features are added or priorities shift.
6. **Eval Error Handling**
   - Address as soon as possible if crashes are frequent, or after BNF/grammar improvements for more robust error reporting.
7. **Web Playground/WASM Feasibility**
   - Consider after core language and documentation improvements, as this will require addressing binary size and dependency bundling challenges.

---

*This index is for internal planning only and is not linked from user-facing documentation.* 