# create
```
docker pull amazon/aws-cli:latest
docker pull lambci/lambda:build-python3.8
```

# build first time
```
Windows: docker run -it -v %cd%:/data lambci/lambda:build-python3.8 /bin/bash
Mac: docker run -it -v ~/:/data lambci/lambda:build-python3.8 /bin/bash
yum install libX*
cd /data/Documents/openssl-1.1.1m/
make clean
./config no-shared
make
make install
cd /data/Documents/fltk-1.3.8/
./configure no-shared
make
make install
cd /data/Documents/GitHub/Grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a -std=gnu++11 -Lsource/openssl-lib/aws -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa
```

#build
```
Windows: docker run -it -v %cd%:/data lambci/lambda:build-python3.8-grapa /bin/bash
Mac: docker run -it -v ~/:/data lambci/lambda:build-python3.8x /bin/bash
cd /data/Documents/GitHub/Grapa
g++ -Isource source/main.cpp source/grapa/*.cpp source/openssl-lib/aws/*.a source/fl-lib/aws/*.a -std=gnu++11 -Lsource/openssl-lib/aws -lcrypto -lXfixes -lXft -lXext -lXrender -lXinerama -lfontconfig -lXcursor -ldl -lm -static-libgcc -lX11 -m64 -O3 -pthread -o grapa
```

# package
```
tar -czvf bin/grapa-aws.tar.gz grapa lib/aws/*
```

# extract
```
cp /data/Documents/GitHub/Grapa/bin/grapa-aws.tar.gz ./
tar -xvf grapa-aws.tar.gz
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib/aws
```

# save new image
```
docker ps
docker commit [CONTAINER_ID] lambci/lambda:build-python3.8-grapa
```

# identifying dependancies
```
objdump -p /data/Documents/Grapa/grapa | grep NEEDED
libXfixes.so.3
libXft.so.2
libXext.so.6
libXrender.so.1
libXinerama.so.1
libfontconfig.so.1
libXcursor.so.1
libdl.so.2
libX11.so.6
libstdc++.so.6
libm.so.6
libpthread.so.0
libc.so.6
```

Additional dependancies will be discovered when building grapa.

# locate dependancies
```
find /usr/ -name libXfixes.so.3
find /usr/ -name libXft.so.2
find /usr/ -name libXext.so.6
find /usr/ -name libXrender.so.1
find /usr/ -name libXinerama.so.1
find /usr/ -name libfontconfig.so.1
find /usr/ -name libXcursor.so.1
find /usr/ -name libdl.so.2
find /usr/ -name libX11.so.6
find /usr/ -name libstdc++.so.6
find /usr/ -name libm.so.6
find /usr/ -name libpthread.so.0
find /usr/ -name libc.so.6
```

# copy dependancies
```
cp /usr/lib64/libXfixes.so.3 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libXft.so.2 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libXext.so.6 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libXrender.so.1 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libXinerama.so.1 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libfontconfig.so.1 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libXcursor.so.1 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libdl.so.2 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libX11.so.6 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libstdc++.so.6 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libm.so.6 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libpthread.so.0 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libc.so.6 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libfreetype.so.6 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libxcb.so.1 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libpng15.so.15 /data/Documents/GitHub/Grapa/lib/aws/
cp /usr/lib64/libXau.so.6 /data/Documents/GitHub/Grapa/lib/aws/
```
