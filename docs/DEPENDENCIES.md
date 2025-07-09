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
cmake -G "NMake Makefiles" -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON -DBUILD_SHARED_LIBS=OFF -DPCRE2_STATIC_RUNTIME=ON -DCMAKE_BUILD_TYPE=Release ..
nmake

copy ..\src\*.h to source\pcre2
copy *.lib to source\pcre2-lib\win-amd64
```

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

# Ubuntu

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
./config no-shared
make
sudo make install
```
Copy include/openssl to Grapa/source

Copy *.a to Grapa/source/openssl-lib/ubuntu64

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

Copy lib/*.a to Grapa/source/fl-lib/ubuntu64
Copy lib/*.so to Grapa/source/fl-lib/ubuntu64

cp lib/* ~/Documents/GitHub/grapa/source/fl-lib/ubuntu64/
cp lib/* ~/Documents/GitHub/grapa/lib/ubuntu64/


# Mac

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
./config no-shared no-tests enable-ec_nistp_64_gcc_128
make
sudo make install
```
Copy include/openssl to Grapa/source

Copy *.a to Grapa/source/openssl-lib/mac-apple

## FLTK
Navigate to FLTK source folder and complete the following.
```
sudo apt-get install -y libfreetype-dev
./configure --with-optim="-fPIC"
make
sudo make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/mac-apple

or 

Copy lib/*.a to Grapa/source/fl-lib/mac-intel

## BLST

```
build.sh

```

# AWS

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
make clean
./config no-shared
make
make install
```
Copy include/openssl to Grapa/source

Copy *.a to Grapa/source/openssl-lib/aws

## FLTK
Navigate to FLTK source folder and complete the following.
```
yum install libX*
./configure --with-optim="-fPIC"
make
make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/aws
