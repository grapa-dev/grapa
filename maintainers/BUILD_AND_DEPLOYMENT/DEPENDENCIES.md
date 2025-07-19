---
tags:
  - maintainer
  - lowlevel
  - build
  - dependencies
---

# Source
## OpenSSL
- https://github.com/openssl/openssl.git
- https://www.openssl.org/source/openssl-1.1.1w.tar.gz

## FLTK
- https://github.com/fltk/fltk.git
- https://github.com/fltk/fltk/releases/download/release-1.3.11/fltk-1.3.11-source.tar.gz

## BLST
- https://github.com/supranational/blst
- https://github.com/supranational/blst/archive/refs/heads/master.zip

## utf8proc
- https://github.com/JuliaStrings/utf8proc
- https://github.com/JuliaStrings/utf8proc/archive/v2.8.0.tar.gz

## pcre2
- https://github.com/PCRE2Project/pcre2/releases/tag/pcre2-10.45

# win-amd64
See https://stackoverflow.com/questions/45494630/how-to-build-openssl-on-windows-with-visual-studio-2017

Install https://strawberryperl.com/

Install https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/

Run "x64 Native Tools Command Prompt for VS 2022" in administrator mode.

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
perl Configure VC-WIN64A
nmake
nmake install
```
Copy include/openssl to Grapa/source

Copy *.lib to Grapa/source/openssl-lib/win

## FLTK
Navigate to FLTK source folder and complete the following.
```
cmake -S . -B build_vs2022 -G "Visual Studio 17 2022" -A x64 -D BUILD_SHARED_LIBS=OFF -D OPTION_USE_SYSTEM_LIBJPEG=OFF -D OPTION_USE_SYSTEM_LIBPNG=OFF -D OPTION_USE_SYSTEM_ZLIB=OFF -D FLTK_BUILD_FLUID=OFF -D FLTK_BUILD_TEST=OFF -D FLTK_BUILD_EXAMPLES=OFF -D FLTK_MSVC_RUNTIME_DLL=OFF

cmake --build build_vs2022 --config Release
```

Copy FL/* to Grapa/source/FL
Copy build_vs2022/FL/* to Grapa/source/FL

Copy build_vs2022/lib/Release/*.lib to Grapa/source/fl-lib/win

## BLST

```
cd prj/blst
build

copy bindings\*.h* and bindings\*.swg to source\blst
copy *.lib and *.pdb to source\blst-lib\win-amd64
```

## utf8proc

```

```

## pcre2
```
rmdir /s /q build_static
mkdir build_static
cd build_static
cmake -G "Unix Makefiles" -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DPCRE2_STATIC_RUNTIME=ON -DPCRE2_SUPPORT_UNICODE=ON -DCMAKE_BUILD_TYPE=Release ..
make
```

For the lib files:
copy *.lib to grapa\source\pcre2-lib\win-amd64

For the include files (this is done onece for all platforms):
copy ..\src\*.h to grapa\source\pcre2

# win-arm64

## OpenSSL

```
perl Configure VC-WIN64-ARM
nmake
nmake install
```

## FLTK
```
cmake -S . -B build_vs2022_arm64 -G "Visual Studio 17 2022" -A ARM64 -D BUILD_SHARED_LIBS=OFF -D OPTION_USE_SYSTEM_LIBJPEG=OFF -D OPTION_USE_SYSTEM_LIBPNG=OFF -D OPTION_USE_SYSTEM_ZLIB=OFF -D FLTK_BUILD_FLUID=OFF -D FLTK_BUILD_TEST=OFF -D FLTK_BUILD_EXAMPLES=OFF -D FLTK_MSVC_RUNTIME_DLL=OFF

cmake --build build_vs2022_arm64 --config Release
```

## BLST

```
cd prj/blst
build
```

## pcre2
```powershell
Remove-Item -Path "build_static" -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path "build_static"
Set-Location build_static
cmake -G "NMake Makefiles" -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DPCRE2_STATIC_RUNTIME=ON -DCMAKE_BUILD_TYPE=Release ..
nmake

Copy-Item -Path "..\src\*" -Destination "..\..\source\pcre2" -Recurse -Force
Copy-Item -Path "*.lib" -Destination "..\..\source\pcre2-lib\win-arm64" -Force
```

# linux-amd64

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
./config no-shared
make
sudo make install
```
Copy include/openssl to Grapa/source

Copy *.a to Grapa/source/openssl-lib/linux-amd64

## FLTK
Navigate to FLTK source folder and complete the following.
```
sudo apt-get install g++
sudo apt-get install gdb
sudo apt-get install git
sudo apt-get install autoconf
sudo apt-get install libx11-dev
sudo apt-get install libglu1-mesa-dev
sudo apt-get install libasound2-dev
sudo apt-get install libxft-dev
sudo apt-get install -y libfreetype-dev
NOCONFIGURE=1 ./autogen.sh
./configure --with-optim="-fPIC"
# mkdir build
# cd build
# export CFLAGS="-fPIC"
# export CXXFLAGS="-fPIC"
# cmake -DOPTION_BUILD_SHARED_LIBS=ON ..
make
sudo make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/linux-amd64
Copy lib/*.so to Grapa/source/fl-lib/linux-amd64

cp lib/* ~/Documents/GitHub/grapa/source/fl-lib/linux-amd64/
cp lib/* ~/Documents/GitHub/grapa/lib/linux-amd64/

## pcre2
```
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
make

cp -r ../src/* ~/Documents/GitHub/grapa/source/pcre2/
cp *.a ~/Documents/GitHub/grapa/source/pcre2-lib/linux-amd64/
```


# linux-arm64

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
./config no-shared
make
sudo make install
```
Copy include/openssl to Grapa/source

Copy *.a to Grapa/source/openssl-lib/linux-arm64

## FLTK
Navigate to FLTK source folder and complete the following.
```
sudo apt-get install g++
sudo apt-get install gdb
sudo apt-get install git
sudo apt-get install autoconf
sudo apt-get install libx11-dev
sudo apt-get install libglu1-mesa-dev
sudo apt-get install libasound2-dev
sudo apt-get install libxft-dev
sudo apt-get install -y libfreetype-dev
NOCONFIGURE=1 ./autogen.sh
./configure --with-optim="-fPIC"
make
sudo make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/linux-arm64
Copy lib/*.so to Grapa/source/fl-lib/linux-arm64

## pcre2
```
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
make

cp -r ../src/* ~/Documents/GitHub/grapa/source/pcre2/
cp *.a ~/Documents/GitHub/grapa/source/pcre2-lib/linux-arm64/
```


# mac-amd64

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```

# aws-amd64

## pcre2
```
rm -rf build_static
mkdir build_static
cd build_static
cmake -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" ..
make
cp *.a /data/GitHub/grapa/source/pcre2-lib/aws-amd64/
```
