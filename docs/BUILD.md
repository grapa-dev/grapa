# Build

## Windows
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa

## Ubuntu64
g++ -IGrapaLib source/main.cpp source/grapa/*.cpp source/openssl-lib/ubuntu64/*.a source/fl-lib/ubuntu64/*.a -lcrypto -ldl -lm -static-libgcc -lX11 -O3 -pthread -o grapa

## Mac-Apple
g++ -IGrapaLib source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-apple/*.a source/fl-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

## Mac-Intel
g++ -IGrapaLib source/main.cpp source/grapa/*.cpp source/openssl-lib/mac-intel/*.a source/fl-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

## AWS
g++ -IGrapaLib source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a -std=gnu++11 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa

# Dependencies

[Dependencies](DEPENDENCIES.md)
