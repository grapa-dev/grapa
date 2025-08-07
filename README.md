
# Grapa

<!-- Project Logo -->
<!-- ![Grapa Logo](docs/assets/logo.png) -->

<!-- Badges -->
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/grapa-dev/grapa/actions)
[![PyPI](https://img.shields.io/pypi/v/grapapy)](https://pypi.org/project/grapapy/)
[![Docs](https://img.shields.io/badge/docs-online-blue)](https://grapa-dev.github.io/grapa/)
[![License](https://img.shields.io/badge/license-Apache%202.0-blue)](LICENSE)

---

## What is Grapa? 🧩

Grapa is a modern, grammar-based programming language and data processing platform designed for unlimited precision, parallelism, and seamless Python integration. It is ideal for data engineers, educators, researchers, and anyone who needs high-performance ETL, language experimentation, or advanced scripting.

**[📖 Full Documentation](https://grapa-dev.github.io/grapa/)**

---

## Key Features 🚀

- **Unlimited Precision:** Arbitrary-precision math for $INT, $FLOAT, $TIME
- **True Parallelism:** Production-ready parallel ETL/data processing (map/reduce/filter, $thread, $net)
- **Unified File/Database API:** Seamless navigation and processing of files and databases
- **Python Integration:** Use GrapaPy for idiomatic, high-performance scripting and data workflows
- **Executable BNF & Mutable Grammar:** Define and experiment with grammars at runtime
- **Education & Prototyping:** Rapidly prototype languages, teach compiler concepts, and experiment with meta-programming
- **Cross-Platform:** Windows, Mac, Linux, AWS, and more

---

## Quick Start ⚡

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
grapa -f "my_script.grc"
```

**[🚀 Get Started Guide](https://grapa-dev.github.io/grapa/GETTING_STARTED/)**

---

## Documentation 📚

- **[Getting Started](https://grapa-dev.github.io/grapa/GETTING_STARTED/)** - First steps with Grapa
- **[CLI Quickstart](https://grapa-dev.github.io/grapa/CLI_QUICKSTART/)** - Command-line interface guide
- **[Python Integration](https://grapa-dev.github.io/grapa/PYTHON_INTEGRATION/)** - Using Grapa with Python
- **[API Reference](https://grapa-dev.github.io/grapa/API_REFERENCE/)** - Complete function reference
- **[Migration Guides](https://grapa-dev.github.io/grapa/migrations/)** - From Python, JavaScript, Rust, and more
- **[Examples](https://grapa-dev.github.io/grapa/EXAMPLES/)** - Code examples and use cases
- **[Troubleshooting](https://grapa-dev.github.io/grapa/TROUBLESHOOTING/)** - Common issues and solutions

---

## Why Grapa? 💡

- **Data Engineering:** Run high-throughput ETL pipelines with true parallelism and unlimited precision
- **Education:** Teach and experiment with grammars, compilers, and language design
- **Python Integration:** Offload heavy data processing to Grapa from Python, bypassing the GIL
- **Research:** Prototype new languages, algorithms, and data workflows quickly
- **Cross-Platform:** Use the same code and tools on Windows, Mac, Linux, and cloud

---

## Community & Support 🤝

- **[GitHub Issues](https://github.com/grapa-dev/grapa/issues)** - Report bugs and request features
- **[Discussions](https://github.com/grapa-dev/grapa/discussions)** - Ask questions and share ideas

## For Maintainers & Contributors 🛠️

- **[Maintainer Documentation](maintainers/)** - Build instructions, development workflow, and internal docs
- **[Current Status](maintainers/PROJECT_MANAGEMENT/CURRENT_STATUS.md)** - Project priorities and development status
- **[Implementation Details](maintainers/IMPLEMENTATION/)** - Core technical implementation documentation
- **[Testing & Quality](maintainers/TESTING_AND_QUALITY/)** - Testing frameworks and quality assurance
- **[Build System](maintainers/BUILD_AND_DEPLOYMENT/)** - Build instructions and deployment guides

---

## License 📄

Apache 2.0 License - see [LICENSE](LICENSE) file for details.

---

<!-- Search Optimization: Keywords: Grapa, GrapaPy, Python, migration, quickstart, ETL, parallelism, language, grammar, unlimited precision, data processing, onboarding, examples, API reference, contributing, community, support, documentation, static site, sidebar, navigation, troubleshooting, education, research, scripting, cross-platform, install, hello world, use cases, integration, reference, type docs, object docs, build, license -->
# Test workflow trigger
# Test commit to verify pages workflow doesn't trigger
