# extract
```
cp /data/Documents/GitHub/Grapa/bin/grapa-aws.tar.gz ./
tar -xvf grapa-aws.tar.gz
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib/aws
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

## aws-arm64
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

cp /usr/lib64/libXfixes.so.3.1.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXfixes.so.3
cp /usr/lib64/libXft.so.2.3.8 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXft.so.2
cp /usr/lib64/libXext.so.6.4.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXext.so.6
cp /usr/lib64/libXrender.so.1.3.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXrender.so.1
cp /usr/lib64/libXinerama.so.1.0.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXinerama.so.1
cp /usr/lib64/libfontconfig.so.1.12.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libfontconfig.so.1
cp /usr/lib64/libXcursor.so.1.0.2 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXcursor.so.1
cp /usr/lib64/libdl.so.2 /data/GitHub/grapa/source/X11-lib/aws-arm64/libdl.so.2
cp /usr/lib64/libX11.so.6.4.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libX11.so.6
cp /usr/lib64/libstdc++.so.6.0.33 /data/GitHub/grapa/source/X11-lib/aws-arm64/libstdc++.so.6
cp /usr/lib64/libm.so.6 /data/GitHub/grapa/source/X11-lib/aws-arm64/libm.so.6
cp /usr/lib64/libpthread.so.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libpthread.so.0
cp /usr/lib64/libc.so.6 /data/GitHub/grapa/source/X11-lib/aws-arm64/libc.so.6
cp /usr/lib64/libfreetype.so.6.20.1 /data/GitHub/grapa/source/X11-lib/aws-arm64/libfreetype.so
cp /usr/lib64/libxcb.so.1.1.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libxcb.so.1
cp /usr/lib64/libpng16.so.16.37.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libpng.so.16
cp /usr/lib64/libXau.so.6.0.0 /data/GitHub/grapa/source/X11-lib/aws-arm64/libXau.so.6

```

## aws-amd64

```
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

## linux-arm64

```
cp /usr/lib/aarch64-linux-gnu/libXfixes.so.3.1.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXfixes.so
cp /usr/lib/aarch64-linux-gnu/libXft.so.2.3.6 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXft.so
cp /usr/lib/aarch64-linux-gnu/libXext.so.6.4.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXext.so
cp /usr/lib/aarch64-linux-gnu/libXrender.so.1.3.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXrender.so
cp /usr/lib/aarch64-linux-gnu/libXinerama.so.1.0.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXinerama.so
cp /usr/lib/aarch64-linux-gnu/libfontconfig.so.1.12.1 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libfontconfig.so
cp /usr/lib/aarch64-linux-gnu/libXcursor.so.1.0.2 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXcursor.so
cp /usr/lib/aarch64-linux-gnu/libdl.so.2 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libdl.so
cp /usr/lib/aarch64-linux-gnu/libX11.so.6.4.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libX11.so
cp /usr/lib/aarch64-linux-gnu/libstdc++.so.6.0.33 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libstdc++.so
cp /usr/lib/aarch64-linux-gnu/libm.so.6 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libm.so
cp /usr/lib/aarch64-linux-gnu/libpthread.so.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libpthread.so
cp /usr/lib/aarch64-linux-gnu/libc.so.6 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libc.so
cp /usr/lib/aarch64-linux-gnu/libfreetype.so.6.20.1 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libfreetype.so
cp /usr/lib/aarch64-linux-gnu/libxcb.so.1.1.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libxcb.so
cp /usr/lib/aarch64-linux-gnu/libpng16.so.16.43.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libpng.so
cp /usr/lib/aarch64-linux-gnu/libXau.so.6.0.0 /media/psf/Home/GitHub/grapa/source/X11-lib/linux-arm64/libXau.so


```
