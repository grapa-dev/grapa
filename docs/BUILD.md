# Build

## Windows
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa

msbuild build/win/grapa.sln /p:Configuration=Release

## Ubuntu64
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.a -lcrypto -ldl -lm -static-libgcc -lX11 -O3 -pthread -o grapa
```

## Mac-Apple
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa
```

## Mac-Intel
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa
```

## AWS
```
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a -std=gnu++11 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa
```

# Dependencies
Grapa is dependant on the static libraries for OpenSSL and FLTK. Use the following to build the libraries from source. Additionally, some environments may require the crypto library installed for grapa to build, and if so, build OpenSSL from source and install.

[Dependencies](DEPENDENCIES.md)
