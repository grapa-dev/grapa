# create
```
docker pull amazon/aws-cli:latest
docker pull public.ecr.aws/lambda/python:3.12

docker run --platform=linux/arm64 -it --name amazonlinux-apple -v $HOME:/data amazonlinux:2023 bash
docker run --platform=linux/amd64 -it --name amazonlinux-intel -v $HOME:/data amazonlinux:2023 bash

docker ps
docker commit [ID] amazonlinux-intel2
docker commit [ID] amazonlinux-apple2

docker start -ai amazonlinux-apple2
docker start -ai amazonlinux-intel2

dnf update -y
dnf install -y make
dnf install -y perl
dnf install -y mesa-libGL-devel libGLU-devel

dnf install -y python3 python3-devel gcc make
python3 -m ensurepip --upgrade
python3 -m pip install --upgrade pip setuptools
pip3 install --upgrade setuptools
pip install --user --upgrade packaging
dnf install -y python3-devel
dnf install -y tar

```

# build first time
```
Windows: docker run -it -v %cd%:/data lambci/lambda:build-python3.8 /bin/bash
Mac: docker run -it -v $HOME:/data public.ecr.aws/lambda/python:3.12 bash
Mac: docker run -it -v $HOME:/data amazonlinux bash
yum install libX*
cd /data/Downloads/openssl-1.1.1w/
make clean
./config no-shared
make
make install
cd /data/Downloads/fltk-1.3.11/
./configure --disable-shared
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
objdump -p /data/GitHub/grapa/grapa | grep NEEDED
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
find /usr/ -name libpng16.so.16
```

# copy dependancies
```
cp /usr/lib64/libXfixes.so.3.1.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXfixes.so
cp /usr/lib64/libXft.so.2.3.8 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXft.so
cp /usr/lib64/libXext.so.6.4.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXext.so
cp /usr/lib64/libXrender.so.1.3.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXrender.so
cp /usr/lib64/libXinerama.so.1.0.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXinerama.so
cp /usr/lib64/libfontconfig.so.1.12.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libfontconfig.so
cp /usr/lib64/libXcursor.so.1.0.2 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXcursor.so
cp /usr/lib64/libdl.so.2 /data/GitHub/grapa/source/X11-lib/aws-arm64/libdl.so
cp /usr/lib64/libX11.so.6.4.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libX11.so
cp /usr/lib64/libstdc++.so.6.0.33 /data/GitHub/grapa/source/X11-lib/aws-arm64/libstdc++.so
cp /usr/lib64/libm.so.6 /data/GitHub/grapa/source/X11-lib/aws-arm64/libm.so
cp /usr/lib64/libpthread.so.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libpthread.so
cp /usr/lib64/libc.so.6 /data/GitHub/grapa/source/X11-lib/aws-arm64/libc.so
cp /usr/lib64/libfreetype.so.6.20.1 /data/GitHub/grapa/source/X11-lib/aws-arm64/libfreetype.so
cp /usr/lib64/libxcb.so.1.1.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libxcb.so
cp /usr/lib64/libpng16.so.16.37.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libpng.so
cp /usr/lib64/libXau.so.6.0.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXau.so


cp /usr/lib64/libXfixes.so.3.1.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXfixes.so
cp /usr/lib64/libXft.so.2.3.8 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXft.so
cp /usr/lib64/libXext.so.6.4.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXext.so
cp /usr/lib64/libXrender.so.1.3.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXrender.so
cp /usr/lib64/libXinerama.so.1.0.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXinerama.so
cp /usr/lib64/libfontconfig.so.1.12.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libfontconfig.so
cp /usr/lib64/libXcursor.so.1.0.2 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXcursor.so
cp /usr/lib64/libdl.so.2 /data/GitHub/grapa/source/X11-lib/aws-amd64/libdl.so
cp /usr/lib64/libX11.so.6.4.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libX11.so
cp /usr/lib64/libstdc++.so.6.0.33 /data/GitHub/grapa/source/X11-lib/aws-amd64/libstdc++.so
cp /usr/lib64/libm.so.6 /data/GitHub/grapa/source/X11-lib/aws-amd64/libm.so
cp /usr/lib64/libpthread.so.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libpthread.so
cp /usr/lib64/libc.so.6 /data/GitHub/grapa/source/X11-lib/aws-amd64/libc.so
cp /usr/lib64/libfreetype.so.6.20.1 /data/GitHub/grapa/source/X11-lib/aws-amd64/libfreetype.so
cp /usr/lib64/libxcb.so.1.1.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libxcb.so
cp /usr/lib64/libpng16.so.16.37.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libpng.so
cp /usr/lib64/libXau.so.6.0.0 /data/GitHub/grapa/source/X11-lib/aws-amd64/libXau.so


```
