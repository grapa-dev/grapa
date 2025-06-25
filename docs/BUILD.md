# Build

## Windows
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa
Packaging requires https://www.7-zip.org/ and add location [C:\Program Files\7-Zip] to PATH.

Run "x64 Native Tools Command Prompt for VS 2022"
Navigate to Grapa folder
```
msbuild prj/win/grapa.sln /p:Configuration=Release
del grapa.exe
copy prj\win\x64\Release\grapa.exe grapa.exe
rmdir /S /q prj\win\x64
rmdir /S /q prj\win\grapa

msbuild prj/winlib/grapalib.sln /p:Configuration=Release
del grapa.lib
copy prj\winlib\x64\Release\grapa.lib grapa.lib
del source\grapa-lib\win\grapa.lib
copy prj\winlib\x64\Release\grapa.lib source\grapa-lib\win\grapa.lib
rmdir /S /q prj\winlib\x64
rmdir /S /q prj\winlib\grapalib

7z a bin/grapa-win.zip grapa.exe grapa.lib

rmdir /s /q dist
python setup.py sdist
rmdir /s /q grapapy.egg-info
grapa.exe -q -ccmd "f=$file().ls('dist')[0].$KEY;$sys().shell('pip install dist/'+f);"

```

## Linux
```
sudo apt install x11-apps
sudo apt install libx11-dev
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/linux/*.a source/fl-lib/linux/*.a source/blst-lib/linux/*.a -Lsource/openssl-lib/linux -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource source/grapa/*.cpp -O3 -pthread -fPIC
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/linux/libgrapa.a
g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/linux/*.a source/fl-lib/linux/*.a source/blst-lib/linux/*.a -Lsource/openssl-lib/linux -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/linux/libgrapa.so

tar -czvf bin/grapa-linux.tar.gz grapa source/grapa-lib/linux/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"

```
Setting up the dev tools
```
sudo apt install gcc
sudo apt install g++
sudo apt install python3-dev
sudo wget https://github.com/shiftkey/desktop/releases/download/release-2.9.3-linux3/GitHubDesktop-linux-2.9.3-linux3.deb
sudo apt-get install gdebi-core 
sudo apt install libxcursor-dev
sudo apt install libxft-dev
sudo apt install libxext-dev
sudo apt install libxinerama-dev
sudo apt install python3-pip
sudo apt install cmake

https://github.com/shiftkey/desktop

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Documents/GitHub/grapa/source/grapa-lib/linux
```

## Mac

### Apple
```

rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a source/blst-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa
codesign -s dev-grapa-cert ./grapa

g++ -c -Isource source/grapa/*.cpp -std=gnu++11 -m64 -O3 -pthread
ar -crs libgrapa.a *.o source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a source/blst-lib/mac-apple/*.a
rm *.o
codesign -s dev-grapa-cert ./libgrapa.a
cp libgrapa.a source/grapa-lib/mac-apple/libgrapa.a
g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a source/blst-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -fPIC -o libgrapa.so
codesign -s dev-grapa-cert ./libgrapa.so
cp libgrapa.so source/grapa-other/mac-apple/libgrapa.so

tar -czvf bin/grapa-mac-apple.tar.gz grapa source/grapa-lib/mac-apple/* source/grapa-other/mac-apple/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"

```

Helful additons
```
sudo chmod u+x /script-location/
```


### Intel
```
rm grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a source/blst-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa
codesign -s dev-grapa-cert ./grapa

g++ -c -Isource source/grapa/*.cpp -std=gnu++11 -m64 -O3 -pthread
ar -crs libgrapa.a *.o source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a source/blst-lib/mac-intel/*.a
rm *.o
codesign -s dev-grapa-cert ./libgrapa.a
cp libgrapa.a source/grapa-lib/mac-intel/libgrapa.a
g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a source/blst-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -fPIC -o libgrapa.so
codesign -s dev-grapa-cert ./libgrapa.so
cp libgrapa.so source/grapa-other/mac-intel/libgrapa.so

tar -czvf bin/grapa-mac-intel.tar.gz grapa source/grapa-lib/mac-intel/* source/grapa-other/mac-intel/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"


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
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a source/blst-lib/aws/*.a -Lsource/openssl-lib/aws -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -o grapa

g++ -c -Isource source/grapa/*.cpp -O3 -pthread -fPIC 
ar -crs libgrapa.a *.o
rm *.o
cp libgrapa.a source/grapa-lib/aws/libgrapa.a

g++ -shared -Isource source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a source/blst-lib/aws/*.a -Lsource/openssl-lib/aws -lcrypto -lX11 -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -O3 -pthread -fPIC -o libgrapa.so
cp libgrapa.so source/grapa-lib/aws/libgrapa.so

tar -czvf bin/grapa-aws.tar.gz grapa source/grapa-lib/aws/*

rm -rf dist
python3 setup.py sdist
rm -rf grapapy.egg-info
./grapa -q -ccmd "f=\$file().ls('dist')[0].\$KEY;$sys().shell('pip3 install dist/'+f);"

```

Other commands
```
docker run -it -v %cd%:/data lambci/lambda:build-python3.8 /bin/bash

docker ps
docker commit [CONTAINER ID) [name]

docker run -it -v %cd%:/data lambci/lambda:build-python3.8-grapa /bin/bash
docker commit [CONTAINER ID) lambci/lambda:build-python3.8-grapa

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


