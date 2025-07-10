# Build

## Overview

Grapa includes PCRE2 support for enhanced Unicode regex functionality on all supported platforms:
- **Windows**: x64 (with PCRE2 JIT compilation)
- **Mac**: x64 and ARM64 (with PCRE2 JIT compilation)  
- **Linux**: x64 and ARM64 (with PCRE2 JIT compilation)

PCRE2 provides advanced features including Unicode properties, named groups, atomic groups, lookaround assertions, and grapheme clusters.

## win-amd64
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa
Packaging requires https://www.7-zip.org/ and add location [C:\Program Files\7-Zip] to PATH.

Run "x64 Native Tools Command Prompt for VS 2022"
Navigate to Grapa folder
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

### Testing After Build
After building, run the test suite to validate the build:
```bash
# Run complete test suite
.\grapa.exe -cfile "test/run_tests.grc"

# Or run individual test categories
.\grapa.exe -cfile "test/test_current_capabilities.grc"
.\grapa.exe -cfile "test/test_performance_optimizations.grc"
```

For detailed testing information, see [Testing Documentation](TESTING.md).

## Mac

### setup

```
brew install llvm
```

### mac-arm64
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

clang -Isource -DUTF8PROC_STATIC -c source/utf8proc/utf8proc.c -o source/utf8proc/utf8proc.o
clang++ -shared -Isource source/grapa/*.cpp source/utf8proc/utf8proc.o source/openssl-lib/mac-arm64/*.a source/fl-lib/mac-arm64/*.a source/blst-lib/mac-arm64/*.a source/pcre2-lib/mac-arm64/libpcre2-8.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -fPIC -o libgrapa.so
codesign -s dev-grapa-cert ./libgrapa.so
cp libgrapa.so source/grapa-other/mac-arm64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-mac-arm64.tar.gz grapa source/grapa-lib/mac-arm64/* source/grapa-other/mac-arm64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"
```

### Testing After Build
After building, run the test suite to validate the build:
```bash
# Run complete test suite
./grapa -cfile "test/run_tests.grc"

# Or run individual test categories
./grapa -cfile "test/test_current_capabilities.grc"
./grapa -cfile "test/test_performance_optimizations.grc"
```

Helpful additons
```
sudo chmod u+x /script-location/
```

### mac-amd64
```
rm grapa
clang++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-amd64/*.a source/fl-lib/mac-amd64/*.a source/blst-lib/mac-amd64/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -o grapa
codesign -s dev-grapa-cert ./grapa

clang++ -c -Isource source/grapa/*.cpp -std=c++17 -m64 -O3 -pthread
ar -crs libgrapa.a *.o
rm *.o
codesign -s dev-grapa-cert ./libgrapa.a
cp libgrapa.a source/grapa-lib/mac-amd64/libgrapa.a
rm libgrapa.a

clang++ -shared -Isource source/grapa/*.cpp source/openssl-lib/mac-amd64/*.a source/fl-lib/mac-amd64/*.a source/blst-lib/mac-amd64/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=c++17 -m64 -O3 -pthread -fPIC -o libgrapa.so
codesign -s dev-grapa-cert ./libgrapa.so
cp libgrapa.so source/grapa-other/mac-amd64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-mac-amd64.tar.gz grapa source/grapa-lib/mac-amd64/* source/grapa-other/mac-amd64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"

### Testing After Build
After building, run the test suite to validate the build:
```bash
# Run complete test suite
./grapa -cfile "test/run_tests.grc"

# Or run individual test categories
./grapa -cfile "test/test_current_capabilities.grc"
./grapa -cfile "test/test_performance_optimizations.grc"
```

Helpful additons
```
sudo chmod u+x /script-location/
```


### Create a certificate
	open -a "Keychain Access"
	(MENU) Certificate Assistant > Create a Certificate
	Enter the Name (for example dev-grapa-cert)
	Identity Type > Self Signed Root
	Certificate Type > Code Signing
	Check "Let me override defaults" & click Continue.
	Enter a unique Serial Number.
	Enter a big Validity Period (days), like 3560 & click Continue.
	Fill in your personal information & click Continue.
	Accept defaults for the rest of the dialog boxes.

### Self sign
```
codesign -s dev-grapa-cert ./grapa
```

## linux

### setup

Need to be on Ubunto 24.04 for c++17 support. If not, go through the updates to get there. Ask ChatGPT how to do this.

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

For Unbuntu 20.04, enable c++17
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

### linux-arm64
```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/linux-arm64/*.a source/fl-lib/linux-arm64/*.a source/blst-lib/linux-arm64/*.a -Lsource/openssl-lib/linux-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource source/grapa/*.cpp -std=c++17 -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/linux-arm64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/linux-arm64/*.a source/fl-lib/linux-arm64/*.a source/blst-lib/linux-arm64/*.a -Lsource/openssl-lib/linux-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/linux-arm64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-linux-arm64.tar.gz grapa source/grapa-lib/linux-arm64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
```

### Testing After Build
After building, run the test suite to validate the build:
```bash
# Run complete test suite
./grapa -cfile "test/run_tests.grc"

# Or run individual test categories
./grapa -cfile "test/test_current_capabilities.grc"
./grapa -cfile "test/test_performance_optimizations.grc"
```

## aws

### setup

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


### aws-amd64

```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws-amd64/*.a source/fl-lib/aws-amd64/*.a source/blst-lib/aws-amd64/*.a -Lsource/openssl-lib/aws-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource source/grapa/*.cpp -std=c++17 -O3 -pthread -fPIC 
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/aws-amd64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/aws-amd64/*.a source/fl-lib/aws-amd64/*.a source/blst-lib/aws-amd64/*.a -Lsource/openssl-lib/aws-amd64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/aws-amd64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-aws-amd64.tar.gz grapa source/grapa-lib/aws-amd64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"
```


### aws-arm64

```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws-arm64/*.a source/fl-lib/aws-arm64/*.a source/blst-lib/aws-arm64/*.a -Lsource/openssl-lib/aws-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource source/grapa/*.cpp -std=c++17 -O3 -pthread -fPIC 
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/aws-arm64/libgrapa.a
rm libgrapa.a

g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/aws-arm64/*.a source/fl-lib/aws-arm64/*.a source/blst-lib/aws-arm64/*.a -Lsource/openssl-lib/aws-arm64 -std=c++17 -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/aws-arm64/libgrapa.so
rm libgrapa.so

tar -czvf bin/grapa-aws-arm64.tar.gz grapa source/grapa-lib/aws-arm64/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"
```


# lib/grapa

The file source/grapa/GrapaStaticLib.c is an array of of the compiled scripts in lib/grapa. The grapa application will use this array as a lookup after searching lib/grapa. If the file exists in lib/grapa (or in the location of the grapa binary) it will be used there. Otherwise, it will be loaded from the static lib array. This serves two purposes. First, the grapa application has everything embedded that it requires. Second, there are libraries in lib/grapa that are required at an early stage of grapa initialization.

If any of the files in lib/grapa are changed, there is a grapa script that will compile the files and generate the source/grapa/GrapaStaticLib.c file. To run this, the grapa application needs to be first compiled and exist at the base project folder. From the base project folder, run grapa. From grapa, issue the following:
```
include "source/buildgrapalib.grc";

or

grapa -ccmd "include 'source/buildgrapalib.grc';" -q
```


