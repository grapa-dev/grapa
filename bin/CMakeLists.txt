cmake_minimum_required(VERSION 3.16)
project(grapa_example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Force Release mode for Windows to match the static library
if(WIN32)
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CONFIGURATION_TYPES Release)

    # Ensure Ninja generator works properly on Windows
    if(CMAKE_GENERATOR STREQUAL "Ninja")
        # Set Ninja-specific variables
        set(CMAKE_CXX_COMPILER_LAUNCHER "")
        set(CMAKE_C_COMPILER_LAUNCHER "")

        # Ensure proper Windows SDK detection for Ninja
        set(CMAKE_SYSTEM_VERSION 10.0)
        set(CMAKE_SYSTEM_NAME Windows)
    endif()
    
    # Enable OpenMP for Windows using clang (better OpenMP support than MSVC)
    find_package(OpenMP REQUIRED)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
        message(STATUS "OpenMP C++ flags: ${OpenMP_CXX_FLAGS}")
        message(STATUS "OpenMP C++ libraries: ${OpenMP_CXX_LIB_NAMES}")
    else()
        message(WARNING "OpenMP not found - LLAMA.cpp may run slower without parallel processing")
    endif()
endif()

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
    # Windows flags - match Python extension configuration exactly
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUTF8PROC_STATIC -DPCRE2_STATIC -std:c++17 /wd4996")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUTF8PROC_STATIC -DPCRE2_STATIC -O3 /wd4996")
    
    # Add manifest flag to match Python extension
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/llama)
    
    # Add Windows SDK include paths for standard C headers like float.h
    # This is especially important for Ninja builds which may not auto-detect SDK paths
    set(WINDOWS_SDK_FOUND FALSE)
    
    # Try the newer Windows SDK location first (Windows 10/11)
    if(EXISTS "C:/Program Files/Windows Kits/10/Include")
        file(GLOB WINDOWS_SDK_VERSIONS "C:/Program Files/Windows Kits/10/Include/*")
        if(WINDOWS_SDK_VERSIONS)
            list(SORT WINDOWS_SDK_VERSIONS)
            list(REVERSE WINDOWS_SDK_VERSIONS)
            list(GET WINDOWS_SDK_VERSIONS 0 LATEST_SDK_PATH)
            get_filename_component(LATEST_SDK_VERSION ${LATEST_SDK_PATH} NAME)
            include_directories("C:/Program Files/Windows Kits/10/Include/${LATEST_SDK_VERSION}/ucrt")
            include_directories("C:/Program Files/Windows Kits/10/Include/${LATEST_SDK_VERSION}/shared")
            include_directories("C:/Program Files/Windows Kits/10/Include/${LATEST_SDK_VERSION}/um")
            set(WINDOWS_SDK_FOUND TRUE)
            message(STATUS "Added Windows SDK ${LATEST_SDK_VERSION} include paths (Program Files)")
        endif()
    endif()
    
    # Fallback to x86 location
    if(NOT WINDOWS_SDK_FOUND AND EXISTS "C:/Program Files (x86)/Windows Kits/10/Include")
        file(GLOB WINDOWS_SDK_VERSIONS "C:/Program Files (x86)/Windows Kits/10/Include/*")
        if(WINDOWS_SDK_VERSIONS)
            list(SORT WINDOWS_SDK_VERSIONS)
            list(REVERSE WINDOWS_SDK_VERSIONS)
            list(GET WINDOWS_SDK_VERSIONS 0 LATEST_SDK_PATH)
            get_filename_component(LATEST_SDK_VERSION ${LATEST_SDK_PATH} NAME)
            include_directories("C:/Program Files (x86)/Windows Kits/10/Include/${LATEST_SDK_VERSION}/ucrt")
            include_directories("C:/Program Files (x86)/Windows Kits/10/Include/${LATEST_SDK_VERSION}/shared")
            include_directories("C:/Program Files (x86)/Windows Kits/10/Include/${LATEST_SDK_VERSION}/um")
            set(WINDOWS_SDK_FOUND TRUE)
            message(STATUS "Added Windows SDK ${LATEST_SDK_VERSION} include paths (Program Files x86)")
        endif()
    endif()
    
    # Additional fallback paths for older SDKs or different installations
    if(NOT WINDOWS_SDK_FOUND)
        # Try Visual Studio installation paths
        if(EXISTS "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC")
            file(GLOB VS_VERSIONS "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/*")
            if(VS_VERSIONS)
                list(SORT VS_VERSIONS)
                list(REVERSE VS_VERSIONS)
                list(GET VS_VERSIONS 0 VS_VERSION_PATH)
                get_filename_component(VS_VERSION ${VS_VERSION_PATH} NAME)
                include_directories("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/${VS_VERSION}/include")
                message(STATUS "Added Visual Studio 2022 Community include paths: ${VS_VERSION}")
                set(WINDOWS_SDK_FOUND TRUE)
            endif()
        endif()
    endif()
    
    if(NOT WINDOWS_SDK_FOUND)
        message(WARNING "Windows SDK not found. Standard C headers like float.h may not be available.")
    endif()

    # Set Windows-specific compiler definitions to match Visual Studio project
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNDEBUG)
    add_definitions(-D_CONSOLE)
    add_definitions(-DUTF8PROC_STATIC)
    add_definitions(-DPCRE2_STATIC)
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUTF8PROC_STATIC -DPCRE2_STATIC -std=c++17 -O3 -pthread -DFLTK_USE_X11 -static-libgcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUTF8PROC_STATIC -DPCRE2_STATIC -O3 -pthread -DFLTK_USE_X11")
    
    # Add include directories for Linux
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/llama)
elseif(APPLE)
    # macOS flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUTF8PROC_STATIC -DPCRE2_STATIC -std=c++17 -O3 -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUTF8PROC_STATIC -DPCRE2_STATIC -O3 -pthread")
    
    # Add include directories for macOS
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/llama)
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Print configuration info
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Grapa library: platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Platform-specific dependency checks
if(WIN32)
    # Check for Visual Studio (MSVC) compiler
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(FATAL_ERROR 
            "❌ VISUAL STUDIO COMPILER REQUIRED\n"
            "This project requires Visual Studio (MSVC) compiler for Windows builds.\n"
            "Please install Visual Studio 2022 or Build Tools for Visual Studio 2022.\n"
            "Download from: https://visualstudio.microsoft.com/downloads/\n"
            "Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
    
    # Check for Grapa static library
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}")
        message(FATAL_ERROR 
            "❌ GRAPA STATIC LIBRARY NOT FOUND\n"
            "Required file: platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}\n"
            "Please ensure you have the complete Grapa distribution with pre-built libraries.")
    endif()
    
    # Check for Windows SDK
    if(NOT EXISTS "C:/Program Files (x86)/Windows Kits/10/Include")
        message(WARNING 
            "⚠️  WINDOWS SDK NOT FOUND\n"
            "Windows SDK not detected at: C:/Program Files (x86)/Windows Kits/10/Include\n"
            "Compilation may fail. Please install Windows 10/11 SDK with Visual Studio.")
    endif()
    
    # Check for required third-party libraries
    set(REQUIRED_LIBS 
        "lib/${GRAPA_PLATFORM_DIR}/fltk.lib"
        "lib/${GRAPA_PLATFORM_DIR}/libssl.lib"
        "lib/${GRAPA_PLATFORM_DIR}/libcrypto.lib"
        "lib/${GRAPA_PLATFORM_DIR}/pcre2-8-static.lib"
    )
    
    foreach(LIB ${REQUIRED_LIBS})
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${LIB}")
            message(FATAL_ERROR 
                "❌ REQUIRED LIBRARY NOT FOUND\n"
                "Missing: ${LIB}\n"
                "Please ensure you have the complete Grapa distribution with all third-party libraries.")
        endif()
    endforeach()
    
    message(STATUS "✅ Windows dependencies validated successfully")
    
elseif(APPLE)
    # Check for Xcode Command Line Tools on macOS
    execute_process(
        COMMAND xcode-select -p
        OUTPUT_VARIABLE XCODE_PATH
        ERROR_QUIET
    )
    
    if(NOT XCODE_PATH)
        message(FATAL_ERROR 
            "❌ XCODE COMMAND LINE TOOLS REQUIRED\n"
            "Please install Xcode Command Line Tools:\n"
            "  xcode-select --install")
    endif()
    
    # Check for Grapa static library
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}")
        message(FATAL_ERROR 
            "❌ GRAPA STATIC LIBRARY NOT FOUND\n"
            "Required file: platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}\n"
            "Please ensure you have the complete Grapa distribution with pre-built libraries.")
    endif()
    
    message(STATUS "✅ macOS dependencies validated successfully")
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Check for required development packages on Linux
    find_library(SSL_LIB ssl)
    find_library(CRYPTO_LIB crypto)
    find_library(X11_LIB X11)
    
    if(NOT SSL_LIB)
        message(FATAL_ERROR 
            "❌ OPENSSL LIBRARY NOT FOUND\n"
            "Please install OpenSSL development package:\n"
            "  Ubuntu/Debian: sudo apt-get install libssl-dev\n"
            "  CentOS/RHEL:   sudo yum install openssl-devel")
    endif()
    
    if(NOT CRYPTO_LIB)
        message(FATAL_ERROR 
            "❌ OPENSSL CRYPTO LIBRARY NOT FOUND\n"
            "Please install OpenSSL development package:\n"
            "  Ubuntu/Debian: sudo apt-get install libssl-dev\n"
            "  CentOS/RHEL:   sudo yum install openssl-devel")
    endif()
    
    if(NOT X11_LIB)
        message(FATAL_ERROR 
            "❌ X11 LIBRARY NOT FOUND\n"
            "Please install X11 development package:\n"
            "  Ubuntu/Debian: sudo apt-get install libx11-dev\n"
            "  CentOS/RHEL:   sudo yum install libX11-devel")
    endif()
    
    # Check for Grapa static library
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}")
        message(FATAL_ERROR 
            "❌ GRAPA STATIC LIBRARY NOT FOUND\n"
            "Required file: platforms/${GRAPA_PLATFORM_DIR}/${GRAPA_STATIC_LIB}\n"
            "Please ensure you have the complete Grapa distribution with pre-built libraries.")
    endif()
    
    message(STATUS "✅ Linux dependencies validated successfully")
endif()

# Create executable
add_executable(grapa_example main.cpp)

# Set include directories
target_include_directories(grapa_example PRIVATE include)

# Add library directories first
target_link_directories(grapa_example PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/platforms/${GRAPA_PLATFORM_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}
)

# Link against Grapa static library and all required 3rd party libraries
# Match the exact configuration used by the Python extension (setup.py)
if(WIN32)
	target_link_libraries(grapa_example 
		${GRAPA_STATIC_LIB}
		fltk
		fltk_forms
		fltk_gl
		fltk_images
		fltk_jpeg
		fltk_png
		fltk_z
		libcrypto
		libssl
		llama
		ggml
		ggml-base
		ggml-cpu
		mtmd
		blst
		pcre2-8-static
		# Windows system libraries (matching Python extension)
		gdiplus
		Gdi32
		Advapi32
		User32
		Ole32
		Shell32
		Comdlg32
		winspool
		crypt32
		comctl32
		# OpenMP library for LLAMA.cpp parallel processing
		OpenMP::OpenMP_CXX
	)
elseif(APPLE)
	# macOS libraries
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
		# LLAMA.cpp libraries
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libllama.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml-base.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml-cpu.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml-metal.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml-blas.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libmtmd.a
	)
else()
	# Linux libraries
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
		# LLAMA.cpp libraries
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libllama.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml-base.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libggml-cpu.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libmtmd.a
		${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}/libcommon.a
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
        # LLAMA.cpp frameworks for GPU acceleration
        "-framework Metal"
        "-framework MetalKit"
        "-framework Accelerate"
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
        "-lgomp"  # Add OpenMP library for LLAMA.cpp
    )
elseif(WIN32)
    # Windows settings
    set_target_properties(grapa_example PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
    )

endif()

# Copy 3rd party static libraries to build directory
add_custom_command(TARGET grapa_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/${GRAPA_PLATFORM_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/lib
    COMMENT "Copying 3rd party libraries to build directory"
)

