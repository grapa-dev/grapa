# Build

## Windows
Requires Visual Studio 2022. Use projects in Grapa/build/win to build grapa

## Ubuntu64
g++ -IGrapaLib GrapaLib/main.cpp GrapaLib/grapa/*.cpp GrapaLib/openssl-lib/ubuntu64/*.a GrapaLib/fl-lib/ubuntu64/*.a -lcrypto -ldl -lm -static-libgcc -lX11 -O3 -pthread -o grapa

## Mac-Apple
g++ -IGrapaLib GrapaLib/main.cpp GrapaLib/grapa/*.cpp GrapaLib/openssl-lib/mac-apple/*.a GrapaLib/fl-lib/mac-apple/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

## Mac-Intel
g++ -IGrapaLib GrapaLib/main.cpp GrapaLib/grapa/*.cpp GrapaLib/openssl-lib/mac-intel/*.a GrapaLib/fl-lib/mac-intel/*.a -framework CoreFoundation -framework AppKit -framework IOKit -std=gnu++11 -m64 -O3 -pthread -o grapa

## AWS
g++ -IGrapaLib GrapaLib/main.cpp GrapaLib/grapa/*.cpp GrapaLib/openssl-lib/aws/*.a GrapaLib/fl-lib/aws/*.a -std=gnu++11 -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa

# Dependencies

[Dependencies](DEPENDENCIES.md)
