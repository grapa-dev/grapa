cmake_minimum_required(VERSION 3.16)
project(grapa_example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(WIN32)
    # Windows detection
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
        set(GRAPA_PLATFORM_DIR "win-amd64")
    else()
        set(GRAPA_PLATFORM_DIR "win-amd64")  # Default to AMD64 for now
    endif()
    set(GRAPA_STATIC_LIB "grapa_static.lib")
elseif(APPLE)
    # macOS detection
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
        set(GRAPA_PLATFORM_DIR "mac-arm64")
    else()
        set(GRAPA_PLATFORM_DIR "mac-amd64")
    endif()
    set(GRAPA_STATIC_LIB "libgrapa_static.a")
else()
    # Linux/AWS detection
    if(EXISTS "/etc/system-release")
        file(READ "/etc/system-release" system_release)
        if(system_release MATCHES "Amazon Linux")
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
                set(GRAPA_PLATFORM_DIR "aws-arm64")
            else()
                set(GRAPA_PLATFORM_DIR "aws-amd64")
            endif()
        else()
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
                set(GRAPA_PLATFORM_DIR "linux-arm64")
            else()
                set(GRAPA_PLATFORM_DIR "linux-amd64")
            endif()
        endif()
    else()
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
            set(GRAPA_PLATFORM_DIR "linux-arm64")
        else()
            set(GRAPA_PLATFORM_DIR "linux-amd64")
        endif()
    endif()
    set(GRAPA_STATIC_LIB "libgrapa_static.a")
endif()

# Set compiler flags to match build.py exactly
if(WIN32)
    # Windows flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Isource -DUTF8PROC_STATIC -std=c++17 -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Isource -DUTF8PROC_STATIC -O3")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Isource -DUTF8PROC_STATIC -std=c++17 -O3 -pthread -DFLTK_USE_X11 -static-libgcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Isource -DUTF8PROC_STATIC -O3 -pthread -DFLTK_USE_X11")
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Print configuration info
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Grapa library: platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Create executable
add_executable(grapa_example main.cpp)

# Set include directories
target_include_directories(grapa_example PRIVATE include)

# Link against Grapa static library and all required 3rd party libraries
if(WIN32)
	target_link_libraries(grapa_example 
		${CMAKE_CURRENT_SOURCE_DIR}/platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/blst.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk_forms.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk_gl.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk_images.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk_jpeg.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk_png.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/fltk_z.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libcrypto.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libssl.lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/pcre2-8-static.lib
	)
else()
	target_link_libraries(grapa_example 
		${CMAKE_CURRENT_SOURCE_DIR}/platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk_gl.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk_png.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk_z.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk_images.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk_forms.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libfltk_jpeg.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libssl.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libcrypto.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libblst.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libpcre2-8.a
	)
endif()

# Platform-specific settings
if(APPLE)
    # macOS settings
    set_target_properties(grapa_example PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path"
    )
    
    # Link against macOS system frameworks that FLTK requires
    target_link_libraries(grapa_example 
        "-framework CoreGraphics"
        "-framework CoreFoundation"
        "-framework AppKit"
        "-framework IOKit"
        "-framework ScreenCaptureKit"
        "-framework CoreText"
        "-framework QuartzCore"
        "-framework Carbon"
        "-framework ApplicationServices"
        "-framework Security"
        "-framework SystemConfiguration"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux settings
    set_target_properties(grapa_example PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
        SKIP_BUILD_RPATH FALSE
    )
    # Also set the RPATH during build
    set_target_properties(grapa_example PROPERTIES
        BUILD_RPATH "$ORIGIN"
    )
    # Force the RPATH to be set
    set_target_properties(grapa_example PROPERTIES
        LINK_FLAGS "-Wl,-rpath,\$ORIGIN"
    )
    
    # Link against X11 system libraries that FLTK requires on Linux
    target_link_libraries(grapa_example 
        "-lX11"
        "-lXfixes"
        "-lXft"
        "-lXext"
        "-lXrender"
        "-lXinerama"
        "-lfontconfig"
        "-lXcursor"
        "-ldl"
        "-lm"
    )
elseif(WIN32)
    # Windows settings
    set_target_properties(grapa_example PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
    )
    
    # Windows doesn't need additional system libraries - everything is statically linked
    # The FLTK, OpenSSL, BLST, and PCRE2 static libraries contain all necessary symbols
endif()

# Copy 3rd party static libraries to build directory
add_custom_command(TARGET grapa_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/lib
    COMMENT "Copying 3rd party libraries to build directory"
)

