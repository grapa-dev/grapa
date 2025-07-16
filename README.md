
# Grapa

<!-- Project Logo -->
<!-- ![Grapa Logo](docs/assets/logo.png) -->

<!-- Badges -->
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/grapa-dev/grapa/actions)
[![PyPI](https://img.shields.io/pypi/v/grapapy)](https://pypi.org/project/grapapy/)
[![Docs](https://img.shields.io/badge/docs-online-blue)](https://github.com/grapa-dev/grapa/tree/main/docs)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

---

## What is Grapa?
Grapa is a modern, grammar-based programming language and data processing platform designed for unlimited precision, parallelism, and seamless Python integration. It is ideal for data engineers, educators, researchers, and anyone who needs high-performance ETL, language experimentation, or advanced scripting.

---

## Key Features
- **Unlimited Precision:** Arbitrary-precision math for $INT, $FLOAT, $TIME
- **True Parallelism:** Production-ready parallel ETL/data processing (map/reduce/filter, $thread, $net)
- **Unified File/Database API:** Seamless navigation and processing of files and databases
- **Python Integration:** Use GrapaPy for idiomatic, high-performance scripting and data workflows
- **Executable BNF & Mutable Grammar:** Define and experiment with grammars at runtime
- **Education & Prototyping:** Rapidly prototype languages, teach compiler concepts, and experiment with meta-programming
- **Cross-Platform:** Windows, Mac, Linux, AWS, and more

---

## Table of Contents
- [Quickstart](#quickstart)
- [For Python Users](#for-python-users)
- [For Newcomers/Non-Python-Users](#for-newcomersnon-python-users)
- [Why Grapa?](#why-grapa)
- [Explore More](#explore-more)
- [Community & Support](#community--support)
- [License](#license)

[Back to Top](#grapa)

---

## Quickstart

### Install
```bash
pip install grapapy
```

### Hello World
```python
import grapapy
xy = grapapy.grapa()
print(xy.eval('"Hello, Grapa!".echo();'))
```

### Run a Script
```bash
grapa -cfile "my_script.grc"
```

[Back to Top](#grapa)

---

> **For Python Users:**
> - [GrapaPy Quickstart](docs/GRAPAPY_INTEGRATION.md#quickstart-for-python-users)
> - [Migration Tips for Python Users](docs/PYTHON_USE_CASES.md#migration-tips-for-python-users)
> - [Python Use Cases](docs/PYTHON_USE_CASES.md)

> **For Newcomers/Non-Python Users:**
> - [Getting Started](docs/GETTING_STARTED.md)
> - [Examples](docs/EXAMPLES.md)
> - [JS-to-Grapa Migration Guide](docs/JS_TO_GRAPA_MIGRATION.md)
> - [Language Reference](docs/FUNCTION_QUICK_REFERENCE.md)

[Back to Top](#grapa)

---

## Why Grapa?
- **Data Engineering:** Run high-throughput ETL pipelines with true parallelism and unlimited precision.
- **Education:** Teach and experiment with grammars, compilers, and language design.
- **Python Integration:** Offload heavy data processing to Grapa from Python, bypassing the GIL.
- **Research:** Prototype new languages, algorithms, and data workflows quickly.
- **Cross-Platform:** Use the same code and tools on Windows, Mac, Linux, and cloud.

[Back to Top](#grapa)

---

## Explore More
- [Getting Started](docs/GETTING_STARTED.md)
- [Examples](docs/EXAMPLES.md)
- [Migration Guides](docs/PYTHON_TO_GRAPA_MIGRATION.md)
- [Use Cases](docs/USE_CASES.md)
- [API Reference](docs/FUNCTION_QUICK_REFERENCE.md)
- [Community & Support](#community--support)

[Back to Top](#grapa)

---

## Community & Support
- [GitHub Issues](https://github.com/grapa-dev/grapa/issues)
- [Discussions](https://github.com/grapa-dev/grapa/discussions)
- [Contributing Guide](CONTRIBUTING.md)
- [Contact](mailto:matichuk@hotmail.com)

[Back to Top](#grapa)

---

## License
See [LICENSE](LICENSE) file for details.

[Back to Top](#grapa)
