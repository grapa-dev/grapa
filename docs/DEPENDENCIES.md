Grapa uses static compiled libraries built from the following. 

- https://www.fltk.org/pub/fltk/1.3.8/fltk-1.3.8-source.tar.gz
- https://www.openssl.org/source/openssl-1.1.1m.tar.gz

# Windows
See https://stackoverflow.com/questions/45494630/how-to-build-openssl-on-windows-with-visual-studio-2017

Install https://strawberryperl.com/

Install https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/

Run "x64 Native Tools Command Prompt for VS 2022"

## OpenSSL
Navigate to OpenSSL source folder and complete the following. If the compile freezes on building the test app, this is ok...just abort the build at that point as the libraries should be built.
```
perl Configure VC-WIN64A no-shared
nmake
nmake install
```
Copy include/openssl to Grapa/source

Copy *.lib to Grapa/source/openssl-lib/win

## FLTK
Navigate to FLTK source folder and complete the following.
```
mkdir build
cd build
cmake ..
```
Open FLTK.sln with VS2022 and build Release x64

Copy build/FL to Grapa/source

Copy build/lib/Release/*.lib to Grapa/source/fl-lib/win

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
sudo apt-get install -y libfreetype-dev
./configure
make
sudo make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/ubuntu64

# Apple

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
sudo apt-get install -y libfreetype-dev
./configure
make
sudo make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/ubuntu64

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

Copy *.a to Grapa/source/openssl-lib/ubuntu64

## FLTK
Navigate to FLTK source folder and complete the following.
```
yum install libX*
./configure no-shared
make
make install
```
Copy FL to Grapa/source

Copy lib/*.a to Grapa/source/fl-lib/ubuntu64