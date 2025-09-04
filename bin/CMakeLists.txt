cmake_minimum_required(VERSION 3.16)
project(grapa_example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags to match build.py exactly
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Isource -DUTF8PROC_STATIC -std=c++17 -O3 -pthread -DFLTK_USE_X11 -static-libgcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Isource -DUTF8PROC_STATIC -O3 -pthread -DFLTK_USE_X11")
endif()

# Detect platform and set appropriate library name - using same logic as build.py
if(WIN32)
    set(GRAPA_LIB_NAME "grapa.dll")
    set(GRAPA_LIB_DIR "win-amd64")
elseif(APPLE)
    set(GRAPA_LIB_NAME "libgrapa.dylib")
    set(GRAPA_LIB_DIR "mac-arm64")
else()
    # Linux/AWS detection - same logic as build.py
    set(GRAPA_LIB_NAME "libgrapa.so")
    
    # Debug processor detection
    message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    
    # Check if this is AWS Linux
    if(EXISTS "/etc/system-release")
        file(READ "/etc/system-release" system_release)
        if(system_release MATCHES "Amazon Linux")
            # AWS Linux - detect architecture
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
                set(GRAPA_LIB_DIR "aws-arm64")
            else()
                set(GRAPA_LIB_DIR "aws-amd64")
            endif()
        else()
            # Regular Linux - detect architecture
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
                set(GRAPA_LIB_DIR "linux-arm64")
            else()
                set(GRAPA_LIB_DIR "linux-amd64")
            endif()
        endif()
    else()
        # Regular Linux - detect architecture
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
            set(GRAPA_LIB_DIR "linux-arm64")
        else()
            set(GRAPA_LIB_DIR "linux-amd64")
        endif()
    endif()
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(include)

# Create the executable
add_executable(grapa_example main.cpp)

# Link against the Grapa shared library
target_link_libraries(grapa_example ${CMAKE_CURRENT_SOURCE_DIR}/${GRAPA_LIB_DIR}/${GRAPA_LIB_NAME})

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    set_target_properties(grapa_example PROPERTIES
        OUTPUT_NAME "grapa_example"
        SUFFIX ".exe"
    )
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(grapa_example PROPERTIES
        OUTPUT_NAME "grapa_example"
    )
    # Set RPATH for macOS to find the library in the same directory as executable
    set_target_properties(grapa_example PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path"
    )
else()
    # Linux-specific settings
    set_target_properties(grapa_example PROPERTIES
        OUTPUT_NAME "grapa_example"
    )
    # Set RPATH for Linux to find the library in the same directory as executable
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
endif()

# Print configuration info
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Grapa library: ${GRAPA_LIB_DIR}/${GRAPA_LIB_NAME}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Copy the shared library to the build directory so the executable can find it
add_custom_command(TARGET grapa_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/${GRAPA_LIB_DIR}/${GRAPA_LIB_NAME}
        ${CMAKE_CURRENT_BINARY_DIR}/${GRAPA_LIB_NAME}
    COMMENT "Copying ${GRAPA_LIB_NAME} to build directory"
)

# Add custom target for easy building
add_custom_target(build_example
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target grapa_example
    COMMENT "Building grapa_example for ${CMAKE_SYSTEM_NAME}"
)
