# CMake generated Testfile for 
# Source directory: /data/GitHub/grapa/dep/pcre2-10.45
# Build directory: /data/GitHub/grapa/dep/pcre2-10.45/build_static
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pcre2_test "sh" "/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2_test.sh")
set_tests_properties(pcre2_test PROPERTIES  _BACKTRACE_TRIPLES "/data/GitHub/grapa/dep/pcre2-10.45/CMakeLists.txt;1146;add_test;/data/GitHub/grapa/dep/pcre2-10.45/CMakeLists.txt;0;")
add_test(pcre2_grep_test "sh" "/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2_grep_test.sh")
set_tests_properties(pcre2_grep_test PROPERTIES  _BACKTRACE_TRIPLES "/data/GitHub/grapa/dep/pcre2-10.45/CMakeLists.txt;1167;add_test;/data/GitHub/grapa/dep/pcre2-10.45/CMakeLists.txt;0;")
add_test(pcre2posix_test "pcre2posix_test")
set_tests_properties(pcre2posix_test PROPERTIES  _BACKTRACE_TRIPLES "/data/GitHub/grapa/dep/pcre2-10.45/CMakeLists.txt;1236;add_test;/data/GitHub/grapa/dep/pcre2-10.45/CMakeLists.txt;0;")
