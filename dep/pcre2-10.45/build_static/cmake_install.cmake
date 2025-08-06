# Install script for directory: /data/GitHub/grapa/dep/pcre2-10.45

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-8.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-posix.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-16.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-32.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2grep" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2grep")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2grep"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2grep")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2grep" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2grep")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2grep")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2test")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2test"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2test")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pcre2test")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-posix.pc"
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-8.pc"
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-16.pc"
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/libpcre2-32.pc"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2-config")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/pcre2.h"
    "/data/GitHub/grapa/dep/pcre2-10.45/src/pcre2posix.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/pcre2" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/cmake/pcre2-config.cmake"
    "/data/GitHub/grapa/dep/pcre2-10.45/build_static/cmake/pcre2-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2-config.1"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2grep.1"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2test.1"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man3" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_callout_enumerate.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_code_copy.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_code_copy_with_tables.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_code_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_compile.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_compile_context_copy.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_compile_context_create.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_compile_context_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_config.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_convert_context_copy.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_convert_context_create.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_convert_context_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_converted_pattern_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_dfa_match.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_general_context_copy.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_general_context_create.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_general_context_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_error_message.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_mark.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_match_data_heapframes_size.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_match_data_size.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_ovector_count.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_ovector_pointer.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_get_startchar.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_jit_compile.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_jit_free_unused_memory.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_jit_match.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_jit_stack_assign.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_jit_stack_create.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_jit_stack_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_maketables.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_maketables_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match_context_copy.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match_context_create.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match_context_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match_data_create.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match_data_create_from_pattern.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_match_data_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_pattern_convert.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_pattern_info.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_serialize_decode.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_serialize_encode.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_serialize_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_serialize_get_number_of_codes.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_bsr.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_callout.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_character_tables.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_compile_extra_options.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_compile_recursion_guard.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_depth_limit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_glob_escape.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_glob_separator.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_heap_limit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_match_limit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_max_pattern_compiled_length.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_max_pattern_length.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_max_varlookbehind.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_newline.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_offset_limit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_optimize.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_parens_nest_limit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_recursion_limit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_recursion_memory_management.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_substitute_callout.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_set_substitute_case_callout.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substitute.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_copy_byname.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_copy_bynumber.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_get_byname.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_get_bynumber.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_length_byname.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_length_bynumber.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_list_free.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_list_get.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_nametable_scan.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2_substring_number_from_name.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2api.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2build.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2callout.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2compat.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2convert.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2demo.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2jit.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2limits.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2matching.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2partial.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2pattern.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2perform.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2posix.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2sample.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2serialize.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2syntax.3"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2unicode.3"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/pcre2" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/AUTHORS.md"
    "/data/GitHub/grapa/dep/pcre2-10.45/COPYING"
    "/data/GitHub/grapa/dep/pcre2-10.45/ChangeLog"
    "/data/GitHub/grapa/dep/pcre2-10.45/LICENCE.md"
    "/data/GitHub/grapa/dep/pcre2-10.45/NEWS"
    "/data/GitHub/grapa/dep/pcre2-10.45/README"
    "/data/GitHub/grapa/dep/pcre2-10.45/SECURITY.md"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2-config.txt"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2.txt"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2grep.txt"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/pcre2test.txt"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/pcre2/html" TYPE FILE FILES
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/NON-AUTOTOOLS-BUILD.txt"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/README.txt"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/index.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2-config.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_callout_enumerate.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_code_copy.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_code_copy_with_tables.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_code_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_compile.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_compile_context_copy.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_compile_context_create.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_compile_context_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_config.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_convert_context_copy.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_convert_context_create.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_convert_context_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_converted_pattern_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_dfa_match.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_general_context_copy.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_general_context_create.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_general_context_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_error_message.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_mark.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_match_data_heapframes_size.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_match_data_size.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_ovector_count.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_ovector_pointer.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_get_startchar.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_jit_compile.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_jit_free_unused_memory.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_jit_match.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_jit_stack_assign.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_jit_stack_create.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_jit_stack_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_maketables.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_maketables_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match_context_copy.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match_context_create.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match_context_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match_data_create.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match_data_create_from_pattern.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_match_data_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_pattern_convert.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_pattern_info.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_serialize_decode.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_serialize_encode.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_serialize_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_serialize_get_number_of_codes.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_bsr.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_callout.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_character_tables.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_compile_extra_options.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_compile_recursion_guard.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_depth_limit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_glob_escape.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_glob_separator.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_heap_limit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_match_limit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_max_pattern_compiled_length.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_max_pattern_length.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_max_varlookbehind.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_newline.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_offset_limit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_optimize.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_parens_nest_limit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_recursion_limit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_recursion_memory_management.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_substitute_callout.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_set_substitute_case_callout.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substitute.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_copy_byname.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_copy_bynumber.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_get_byname.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_get_bynumber.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_length_byname.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_length_bynumber.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_list_free.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_list_get.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_nametable_scan.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2_substring_number_from_name.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2api.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2build.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2callout.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2compat.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2convert.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2demo.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2grep.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2jit.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2limits.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2matching.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2partial.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2pattern.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2perform.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2posix.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2sample.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2serialize.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2syntax.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2test.html"
    "/data/GitHub/grapa/dep/pcre2-10.45/doc/html/pcre2unicode.html"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/data/GitHub/grapa/dep/pcre2-10.45/build_static/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
