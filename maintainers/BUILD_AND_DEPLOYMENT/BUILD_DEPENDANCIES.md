---
tags:
  - maintainer
  - lowlevel
---

> **MAINTAINER NOTE:**
> This is an active, internal document for tracking all build and runtime dependencies for Grapa. It is not deprecated. Some dependencies listed here are not yet documented in public-facing docs. A backlog task exists to review, validate, and formalize this information for broader documentation and onboarding.

# Build

## Quick Links
- [Troubleshooting Guide](TROUBLESHOOTING.md)
- [Dependencies](DEPENDENCIES.md)
- [AWS/Docker Setup](SETUPAWSDOCKER.md)

## Windows

### Setup
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa
Packaging requires https://www.7-zip.org/ and add location [C:\Program Files\7-Zip] to PATH.

Run "x64 Native Tools Command Prompt for VS 2022"
Navigate to Grapa folder

### Test
```bash
# Run complete test suite
.\grapa.exe -cfile "test/run_tests.grc"

# Run complete Python test suite
python test/run_tests.py

# Or run individual test categories
.\grapa.exe -cfile "test/test_current_capabilities.grc"
.\grapa.exe -cfile "test/test_performance_optimizations.grc"

# Or run individual Python test categories
python test/test_grep_python_doc_examples.py
python test/test_python_examples.py
python test/test_python_callback.py
python test/grep/test_grep_python_examples.py

# Test Python integration
python -c "import grapapy; g = grapapy.grapa(); print('Python integration working')"
python -c "import grapapy; g = grapapy.grapa(); result = g.grep('hello', 'hello world'); print('Grep functionality:', result)"
```

---

## Mac

### Setup
```
brew install llvm

* The following is needed for Mac Intel

git clone https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build && cd build
cmake -DLLVM_ENABLE_PROJECTS="libcxx;libcxxabi" -DCMAKE_BUILD_TYPE=Release ../llvm
make cxx

* and then use the resulting *a files in the build
```

### Test
```bash
# Run complete test suite
./grapa -cfile "test/run_tests.grc"

# Run complete Python test suite
python3 test/run_tests.py

# Or run individual test categories
./grapa -cfile "test/test_current_capabilities.grc"
./grapa -cfile "test/test_performance_optimizations.grc"

# Or run individual Python test categories
python3 test/test_grep_python_doc_examples.py
python3 test/test_python_examples.py
python3 test/test_python_callback.py
python3 test/grep/test_grep_python_examples.py

# Test Python integration
python3 -c "import grapapy; g = grapapy.grapa(); print('Python integration working')"
python3 -c "import grapapy; g = grapapy.grapa(); result = g.grep('hello', 'hello world'); print('Grep functionality:', result)"
```

Helpful additions
```
sudo chmod u+x /script-location/
```

---

## Linux

### Setup
Need to be on Ubuntu 24.04 for c++17 support. If not, go through the updates to get there. Ask ChatGPT how to do this.

Installing requirements.
```
sudo apt update
sudo apt install -y python3-pip
sudo apt install -y x11-apps
sudo apt install -y libx11-dev
sudo apt install -y libxcursor-dev
sudo apt install -y libxft-dev
sudo apt install -y libxext-dev
sudo apt install -y libxinerama-dev
```

For Ubuntu 20.04, enable c++17
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install g++-13
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
sudo update-alternatives --config g++
```

Setting up a virtual environment.
```
sudo apt install -y python3-venv
python3 -m venv ~/.venvs/grapa-env
source ~/.venvs/grapa-env/bin/activate
pip install dist/*
```

Or....
```
pip install --break-system-packages dist/*
```

Testing install.
```