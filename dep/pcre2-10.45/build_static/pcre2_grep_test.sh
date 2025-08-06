#! /bin/sh
# This is a generated file.
srcdir=/data/GitHub/grapa/dep/pcre2-10.45
pcre2grep=/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2grep
test -z "$CMAKE_CONFIG_TYPE" || pcre2grep=/data/GitHub/grapa/dep/pcre2-10.45/build_static/$CMAKE_CONFIG_TYPE/pcre2grep
pcre2test=/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2test
test -z "$CMAKE_CONFIG_TYPE" || pcre2test=/data/GitHub/grapa/dep/pcre2-10.45/build_static/$CMAKE_CONFIG_TYPE/pcre2test
. /data/GitHub/grapa/dep/pcre2-10.45/RunGrepTest
if test "$?" != "0"; then exit 1; fi
# End
