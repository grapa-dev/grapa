# Build

## Windows
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa

Run "x64 Native Tools Command Prompt for VS 2022"
Navigate to Grapa folder
```
msbuild build/win/grapa.sln /p:Configuration=Release
del grapa.exe
copy build\win\x64\Release\grapa.exe grapa.exe
rmdir /S /q build\win\x64
```

## Ubuntu64
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.a -lcrypto -ldl -lm -static-libgcc -lX11 -O3 -pthread -o grapa

tar -czvf grapa-ubuntu64.tar.gz grapa
```

## Mac

### Apple
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

tar -czvf grapa-mac-apple.tar.gz grapa
```

### Intel
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

tar -czvf grapa-mac-intel.tar.gz grapa
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
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a -std=gnu++11 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa

tar -czvf grapa-aws.tar.gz grapa lib/aws/*
```


# Dependencies
Grapa is dependant on the static libraries for OpenSSL and FLTK. Use the following to build the libraries from source. Additionally, some environments may require the crypto library installed for grapa to build, and if so, build OpenSSL from source and install.

[Dependencies](DEPENDENCIES.md)
