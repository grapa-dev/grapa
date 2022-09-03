# Build

## Windows
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa
Packaging requires https://www.7-zip.org/ and add location [C:\Program Files\7-Zip] to PATH.

Run "x64 Native Tools Command Prompt for VS 2022"
Navigate to Grapa folder
```
msbuild build/win/grapa.sln /p:Configuration=Release
del grapa.exe
copy build\win\x64\Release\grapa.exe grapa.exe
rmdir /S /q build\win\x64

msbuild build/winlib/grapalib.sln /p:Configuration=Release
del grapa.lib
copy build\winlib\x64\Release\grapa.lib grapa.lib
copy build\winlib\x64\Release\grapa.lib source\grapa-lib\win\grapa.lib
rmdir /S /q build\winlib\x64

7z a bin/grapa-win.zip grapa.exe grapa.lib
```

## Ubuntu64
```
sudo apt install x11-apps
sudo apt install libx11-dev
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.a source/blst-lib/ubuntu64/*.a -Lsource/openssl-lib/ubuntu64 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -O3 -pthread -o grapa

g++ -c -Isource source/grapa/*.cpp source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.a source/blst-lib/ubuntu64/*.a -Lsource/openssl-lib/ubuntu64 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -O3 -pthread
ar -crs grapa.a *.o source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.a source/blst-lib/ubuntu64/*.a
rm *.o
cp grapa.a source/grapa-lib/ubuntu64/grapa.a

g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.so source/blst-lib/ubuntu64/*.a -Lsource/openssl-lib/ubuntu64 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -O3 -pthread -fPIC -o grapa.so
cp grapa.so source/grapa-lib/ubuntu64/grapa.so

tar -czvf bin/grapa-ubuntu64.tar.gz grapa grapa.a grapa.so
```
Setting up the dev tools
```
sudo apt install gcc
sudo apt install g++
sudo apt install python3-dev
sudo wget https://github.com/shiftkey/desktop/releases/download/release-2.9.3-linux3/GitHubDesktop-linux-2.9.3-linux3.deb
sudo apt-get install gdebi-core 
sudo gdebi GitHubDesktop-linux-2.9.3-linux3.deb
sudo apt install libxcursor-dev
sudo apt install libxft-dev
sudo apt install libxext-dev
sudo apt install libxinerama-dev
sudo apt install python3-pip
sudo apt install cmake
```

## Mac

### Apple
```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

codesign -s dev-grapa-cert ./grapa
tar -czvf bin/grapa-mac-apple.tar.gz grapa
```

### Intel
```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

codesign -s dev-grapa-cert ./grapa
tar -czvf bin/grapa-mac-intel.tar.gz grapa
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

## AWS
Assumes AWS Docker image for build-python3.8 is setup.

```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a -std=gnu++11 -Lsource/openssl-lib/aws -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa

tar -czvf bin/grapa-aws.tar.gz grapa lib/aws/*
```

[Setup Docker for AWS build-python3.8](SETUPAWSDOCKER.md)

# Dependencies
Grapa is dependant on the static libraries for OpenSSL and FLTK. Use the following to build the libraries from source. 

[Dependencies](DEPENDENCIES.md)

# lib/grapa

The file source/grapa/GrapaStaticLib.c is an array of of the compiled scripts in lib/grapa. The grapa application will use this array as a lookup after searching lib/grapa. If the file exists in lib/grapa (or in the location of the grapa binary) it will be used there. Otherwise, it will be loaded from the static lib array. This serves two purposes. First, the grapa application has everything embedded that it requires. Second, there are libraries in lib/grapa that are required at an early stage of grapa initialization.

If any of the files in lib/grapa are changed, there is a grapa script that will compile the files and generate the source/grapa/GrapaStaticLib.c file. To run this, the grapa application needs to be first compiled and exist at the base project folder. From the base project folder, run grapa. From grapa, issue the following:
```
include "source/buildgrapalib.grc";

or

grapa -ccmd "include 'source/buildgrapalib.grc';" -q
```

# compile
Grapa scripts can be compiled from within grapa using the following.
```
> $sys().compile(infile,outfile);
```
