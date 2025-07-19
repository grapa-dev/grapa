---
tags:
  - maintainer
  - lowlevel
---
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

### Windows AMD64
```
msbuild prj/win-amd64/grapa.sln /p:Configuration=Release
del grapa.exe
copy prj\win-amd64\x64\Release\grapa.exe grapa.exe
rmdir /S /q prj\win-amd64\x64
rmdir /S /q prj\win-amd64\grapa

msbuild prj/winlib-amd64/grapalib.sln /p:Configuration=Release
del grapa.lib
copy prj\winlib-amd64\x64\Release\grapa.lib grapa.lib
del source\grapa-lib\win-amd64\grapa.lib
copy prj\winlib-amd64\x64\Release\grapa.lib source\grapa-lib\win-amd64\grapa.lib
rmdir /S /q prj\winlib-amd64\x64
rmdir /S /q prj\winlib-amd64\grapalib

7z a bin/grapa-win-amd64.zip grapa.exe grapa.lib

rmdir /s /q dist
python setup.py sdist
rmdir /s /q grapapy.egg-info
grapa.exe -q -ccmd "f=$file().ls('dist')[0].$KEY;$sys().shell('pip install dist/'+f);"
```

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

### Mac ARM64
```
rm grapa
clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3 
clang++ -Isource source/main.cpp source/grapa/*.cpp utf8proc.o source/openssl-lib/mac-arm64/*.a source/fl-lib/mac-arm64/*.a source/blst-lib/mac-arm64/*.a source/pcre2-lib/mac-arm64/libpcre2-8.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -o grapa
rm *.o
codesign -s dev-grapa-cert ./grapa

clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3 
clang++ -Isource -c source/grapa/*.cpp -std=c++17 -m64 -O3 -pthread
ar -crs libgrapa.a *.o
rm *.o
codesign -s dev-grapa-cert ./libgrapa.a
cp libgrapa.a source/grapa-lib/mac-arm64/libgrapa.a
rm libgrapa.a

clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3 
clang++ -shared -Isource source/grapa/*.cpp utf8proc.o source/openssl-lib/mac-arm64/*.a source/fl-lib/mac-arm64/*.a source/blst-lib/mac-arm64/*.a source/pcre2-lib/mac-arm64/libpcre2-8.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -fPIC -o libgrapa.so
rm *.o
codesign -s dev-grapa-cert ./libgrapa.so
cp libgrapa.so source/grapa-other/mac-arm64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-mac-arm64.tar.gz grapa source/grapa-lib/mac-arm64/* source/grapa-other/mac-arm64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"
```

### Mac AMD64
```
rm grapa
clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3
clang++ -Isource source/main.cpp source/grapa/*.cpp utf8proc.o source/openssl-lib/mac-amd64/*.a source/fl-lib/mac-amd64/*.a source/blst-lib/mac-amd64/*.a source/pcre2-lib/mac-amd64/libpcre2-8.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -o grapa
rm *.o
codesign -s dev-grapa-cert ./grapa

clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3 
clang++ -Isource -c source/grapa/*.cpp -std=c++17 -m64 -O3 -pthread
ar -crs libgrapa.a *.o
rm *.o
codesign -s dev-grapa-cert ./libgrapa.a
cp libgrapa.a source/grapa-lib/mac-amd64/libgrapa.a
rm libgrapa.a

clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -m64 -O3 
clang++ -shared -Isource source/grapa/*.cpp utf8proc.o source/openssl-lib/mac-amd64/*.a source/fl-lib/mac-amd64/*.a source/blst-lib/mac-amd64/*.a source/pcre2-lib/mac-amd64/libpcre2-8.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -fPIC -o libgrapa.so
codesign -s dev-grapa-cert ./libgrapa.so
rm *.o
cp libgrapa.so source/grapa-other/mac-amd64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-mac-amd64.tar.gz grapa source/grapa-lib/mac-amd64/* source/grapa-other/mac-amd64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"
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
python3
import grapapy
xy = grapapy.grapa()
xy.eval("x = 3.45; y = 4.32; x**y;")
```

GitHub for Linux
```
https://github.com/shiftkey/desktop
```

Other
```
sudo apt install gcc
sudo apt install g++
sudo apt install gdebi-core 
sudo apt install cmake
```

### Linux ARM64
```
rm grapa
g++ -Isource -DUTF8PROC_STATIC source/main.cpp source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-arm64/*.a source/fl-lib/linux-arm64/*.a source/blst-lib/linux-arm64/*.a source/pcre2-lib/linux-arm64/libpcre2-8.a -Lsource/openssl-lib/linux-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c -std=c++17 -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/linux-arm64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-arm64/*.a source/fl-lib/linux-arm64/*.a source/blst-lib/linux-arm64/*.a source/pcre2-lib/linux-arm64/libpcre2-8.a -Lsource/openssl-lib/linux-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/linux-arm64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-linux-arm64.tar.gz grapa source/grapa-lib/linux-arm64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
pip3 install dist/*
```

### Linux AMD64
```
rm grapa
g++ -Isource -DUTF8PROC_STATIC source/main.cpp source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-amd64/*.a source/fl-lib/linux-amd64/*.a source/blst-lib/linux-amd64/*.a source/pcre2-lib/linux-amd64/libpcre2-8.a -Lsource/openssl-lib/linux-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c -std=c++17 -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/linux-amd64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/linux-amd64/*.a source/fl-lib/linux-amd64/*.a source/blst-lib/linux-amd64/*.a source/pcre2-lib/linux-amd64/libpcre2-8.a -Lsource/openssl-lib/linux-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/linux-amd64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-linux-amd64.tar.gz grapa source/grapa-lib/linux-amd64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
pip3 install dist/*
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

---

## AWS

### Setup
Docker setup
```
docker pull amazon/aws-cli:latest

docker run --platform=linux/arm64 -it -v $HOME:/data amazonlinux:2023 bash
docker run --platform=linux/amd64 -it -v $HOME:/data amazonlinux:2023 bash

docker ps
docker commit [ID] amazonlinux-intel2
docker commit [ID] amazonlinux-apple2

docker start -ai amazonlinux-apple2
docker start -ai amazonlinux-intel2
```

Required
```
dnf update -y
dnf install -y libX*
dnf install -y python3-devel
python3 -m ensurepip --upgrade
python3 -m pip install --upgrade pip setuptools
pip3 install --user --upgrade packaging
dnf install -y perl
dnf install -y tar
```

Other
```
dnf install -y make
dnf install -y gcc
dnf install -y python3
dnf install -y mesa-libGL-devel
dnf install -y libGLU-devel
```

[OS library dependancies](SETUPAWSDOCKER.md)

[Build 3rd party library dependancies](DEPENDENCIES.md)


### AWS ARM64
```
rm grapa
g++ -Isource -DUTF8PROC_STATIC source/main.cpp source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/aws-arm64/*.a source/fl-lib/aws-arm64/*.a source/blst-lib/aws-arm64/*.a source/pcre2-lib/aws-arm64/libpcre2-8.a -Lsource/openssl-lib/aws-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c -std=c++17 -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/aws-arm64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/aws-arm64/*.a source/fl-lib/aws-arm64/*.a source/blst-lib/aws-arm64/*.a source/pcre2-lib/aws-arm64/libpcre2-8.a -Lsource/openssl-lib/aws-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/aws-arm64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-aws-arm64.tar.gz grapa source/grapa-lib/aws-arm64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"
```

### AWS AMD64
```
rm grapa
g++ -Isource -DUTF8PROC_STATIC source/main.cpp source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/aws-amd64/*.a source/fl-lib/aws-amd64/*.a source/blst-lib/aws-amd64/*.a source/pcre2-lib/aws-amd64/libpcre2-8.a -Lsource/openssl-lib/aws-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c -std=c++17 -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/aws-amd64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource -DUTF8PROC_STATIC source/grapa/*.cpp source/utf8proc/utf8proc.c source/openssl-lib/aws-amd64/*.a source/fl-lib/aws-amd64/*.a source/blst-lib/aws-amd64/*.a source/pcre2-lib/aws-amd64/libpcre2-8.a -Lsource/openssl-lib/aws-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/aws-amd64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-aws-amd64.tar.gz grapa source/grapa-lib/aws-amd64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
pip3 install dist/*
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


