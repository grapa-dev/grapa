# AGENTS.md

## Cursor Cloud specific instructions

### Project Overview

Grapa is a C++17 grammar-based programming language / data platform. It produces two main artifacts:
- **`grapa` CLI executable** — built via `python3 build.py`
- **`grapapy` Python extension** — built via `python3 build.py --python` or installed from `dist/` tarball with `CC=gcc CXX=g++ pip3 install dist/grapapy-*.tar.gz`

There are no external services, databases, or Docker containers required.

### Building

- `python3 build.py` — builds the `grapa` CLI executable and static library for the current platform (~2 min).
- `python3 build.py --exe-only` — fast rebuild of CLI only (skips library).
- `python3 build.py --python` — builds CLI + static lib + Python extension.
- `python3 build.py --python-only` — builds only the Python extension (requires libraries from a prior full build).
- `python3 build.py --test` — builds and runs the test suite.

### Important gotchas

1. **`grapapy` import blocks on stdin**: The `grapapy` Python module reads from stdin on import. When running Python scripts that `import grapapy` non-interactively, always redirect stdin: `python3 script.py < /dev/null`. Without this, the process will hang indefinitely.

2. **Python extension build requires `CC=gcc CXX=g++`**: The default C++ compiler on the VM may be clang, which can fail to find `-lstdc++` during the CMake-based pip build. Always set `CC=gcc CXX=g++` when building the Python extension via pip.

3. **System dependencies for building**: The build requires X11 dev libraries (`libxfixes-dev`, `libxft-dev`, `libxext-dev`, `libxrender-dev`, `libxinerama-dev`, `libxcursor-dev`), `python3-dev`, and `libstdc++-14-dev`. These are installed by the VM setup.

### Running tests

- **GRC tests**: `./grapa -f test/<category>/<test_file>.grc` (see `test/README.md` for full listing)
- **Python tests**: `python3 test/python/<test_file>.py < /dev/null`
- **Comprehensive GRC test runner**: `./grapa -f test/infrastructure/run_tests.grc`

### Linting

- **VS Code extension** (optional): `cd extensions/vscode-grapa && npx eslint src --ext ts`
- The main C++ codebase does not have a linter configured.
