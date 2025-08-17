---
tags:
  - maintainer
  - implementation
  - python-integration
  - build-system
  - setup.py
  - platform-specific
---

# Python Build System Implementation

This document provides comprehensive implementation details for Grapa's Python build system, including setup.py, platform-specific configurations, build.py integration, and the DistUtils vs Setuptools migration.

## Overview

**Purpose:** Cross-platform Python package building with platform-specific configurations
**Files:** `setup.py` (420 lines), `build.py` integration
**Status:** Complete implementation with DistUtils migration pending for macOS

## Architecture

### **Build System Components**
```
build.py (Main Build Orchestrator)
├── Platform Detection
├── Build Configuration
├── Native Binary Build
└── Python Package Build
    └── setup.py (Python Package Configuration)
        ├── Platform-Specific Settings
        ├── CMake Integration
        ├── Library Dependencies
        └── Extension Module Definition
```

### **Platform Support Matrix**
| Platform | Architecture | Build System | Library Type | Dependencies |
|----------|--------------|--------------|--------------|--------------|
| Windows | AMD64 | Setuptools + CMake | Static | Windows SDK, UCRT |
| macOS | AMD64/ARM64 | **DistUtils** (legacy) | Static | CoreFoundation, AppKit |
| Linux | AMD64/ARM64 | Setuptools + CMake | Shared | X11, OpenSSL, PCRE2 |
| AWS Linux | AMD64/ARM64 | Setuptools + CMake | Shared | X11, OpenSSL, PCRE2 |

## Implementation Details

### **1. setup.py Platform Detection and Configuration**

#### **Platform Detection**
```python
import platform
import sys

is_arm = platform.machine().lower() in ["aarch64", "arm64"]
is_aws = False
is_apple = False
from_os = ''

# Detect AWS Linux
if sys.platform.startswith('linux'):
    process = subprocess.Popen(['cat', '/etc/os-release'], 
                              stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr.decode() == '':
        stdouts = stdout.decode()
        if stdouts.find("Amazon Linux") >= 0:
            is_aws = True
```

#### **Platform-Specific Configuration**
```python
# Windows Configuration
if sys.platform.startswith('win32'):
    so_ext = '.lib'
    lib_filename = 'grapa' + so_ext
    lib_pathfile = 'grapa-lib/win-amd64/' + lib_filename
    extra_compile_args = ['/DUTF8PROC_STATIC', '/DPCRE2_STATIC']
    extra_link_args = ['/MANIFEST:NO']
    
    # Windows SDK Integration
    import glob
    windows_sdk_paths = glob.glob("C:/Program Files (x86)/Windows Kits/10/Include/*")
    if windows_sdk_paths:
        latest_sdk = max(windows_sdk_paths, key=lambda x: x.split('\\')[-1])
        ucrt_path = os.path.join(latest_sdk, "ucrt")
        shared_path = os.path.join(latest_sdk, "shared")
        um_path = os.path.join(latest_sdk, "um")
        
        include_dirs = []
        if os.path.exists(ucrt_path):
            include_dirs.append(ucrt_path)
        if os.path.exists(shared_path):
            include_dirs.append(shared_path)
        if os.path.exists(um_path):
            include_dirs.append(um_path)

# Linux Configuration
elif sys.platform.startswith('linux'):
    from_os = 'linux-amd64'
    if is_aws:
        from_os = 'aws-amd64'
        if is_arm:
            from_os = 'aws-arm64'
    else:
        if is_arm:
            from_os = 'linux-arm64'
    
    # X11 Dependencies for Linux
    extra_link_args = [
        '-std=c++17', '-lX11', '-lXfixes', '-lXft', '-lXext', 
        '-lXrender', '-lXinerama', '-lXcursor', '-lxcb', '-lXau',
        '-lpng', '-lfontconfig', '-lfreetype', '-O3', '-pthread', 
        '-ldl', '-lm'
    ]
    so_ext = '.so'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/' + from_os + '/' + lib_filename
    runtime_library_dirs = ['$ORIGIN/grapapy-' + grapapy_version]

# macOS Configuration (DistUtils - Legacy)
elif sys.platform.startswith('darwin'):
    from_os = 'mac-amd64'
    if is_arm:
        is_apple = True
        from_os = 'mac-arm64'
    
    # macOS Framework Dependencies
    extra_link_args = [
        '-Wl,-rpath,@loader_path', '-std=c++17', '-stdlib=libc++',
        '-O3', '-pthread', '-fPIC', '-framework', 'CoreFoundation',
        '-framework', 'AppKit', '-framework', 'IOKit', '-O3', '-pthread'
    ]
    extra_compile_args = [
        '-std=c++17', '-O3', '-pthread', '-fPIC'
    ]
    so_ext = '.so'
    lib_filename = 'libgrapa' + so_ext
    lib_pathfile = 'grapa-lib/' + from_os + '/' + lib_filename
```

### **2. CMake Integration (Setuptools)**

#### **CMakeExtension Class**
```python
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)
```

#### **CMakeBuild Class**
```python
class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep
        
        debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"
        
        # CMake Configuration
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",
        ]
        
        # Platform-specific CMake arguments
        if sys.platform.startswith('win32'):
            cmake_args += ['-DUTF8PROC_STATIC=ON', '-DPCRE2_STATIC=ON']
        
        if sys.platform.startswith("darwin"):
            # Cross-compile support for macOS
            archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
            if archs:
                cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]
        
        if sys.platform.startswith('linux') or sys.platform.startswith('aws'):
            build_args += ['-Wl,-rpath,${ORIGIN}']
            destPath = os.path.join(extdir, 'grapapy')
            self.mkpath(destPath)
        
        # Build with CMake
        build_temp = os.path.join(self.build_temp, ext.name)
        if not os.path.exists(build_temp):
            os.makedirs(build_temp)
        
        subprocess.check_call(["cmake", ext.sourcedir] + cmake_args, cwd=build_temp)
        subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=build_temp)
```

### **3. Library Management System**

#### **Library Directory Selection**
```python
def pick_library_dirs():
    my_system = platform.system()
    if my_system == 'Linux':
        if is_aws:
            if is_arm:
                return ["source", "source/grapa-lib/aws-arm64", "source/X11-lib/aws-arm64"]
            else:
                return ["source", "source/grapa-lib/aws-amd64", "source/X11-lib/aws-amd64"]
        else:
            if is_arm:
                return ["source", "source/grapa-lib/linux-arm64", "source/X11-lib/linux-arm64"]
            else:
                return ["source", "source/grapa-lib/linux-amd64", "source/X11-lib/linux-amd64"]
    if my_system == 'Darwin':
        if is_arm:
            return ["source", "source/grapa-lib/mac-arm64"]
        else:
            return ["source", "source/grapa-lib/mac-amd64"]
    if my_system == 'Windows':
        return ["source", "source/grapa-lib/win-amd64"]
    raise ValueError("Unknown platform: " + my_system)
```

#### **Library Selection**
```python
def pick_libraries():
    my_system = platform.system()
    if my_system == 'Linux':
        return ['grapa']  # Shared library
    if my_system == 'Darwin':
        return ['source/grapa-lib/libgrapa.a']  # Static library
    if my_system == 'Windows':
        return ["grapa", "Gdi32", "Advapi32", "User32", "Ole32", "Shell32", "Comdlg32"]
    raise ValueError("Unknown platform: " + my_system)
```

### **4. Shared Library Copying System**

#### **CopySharedLibrary Command**
```python
class CopySharedLibrary(Command):
    user_options = []
    
    def initialize_options(self):
        self.build_lib = None
        self.inplace = 0
        self.build_dir = "source"
        self.filename = lib_filename
        self.lib_source_path = os.path.join(self.build_dir, lib_pathfile)
        self.package_name = 'source'
    
    def finalize_options(self):
        self.set_undefined_options('build', ('build_lib', 'build_lib'))
        self.set_undefined_options('build_ext', ('inplace', 'inplace'))
    
    def run(self) -> None:
        self.inplace = self.get_finalized_command('build_ext').inplace
        if self.inplace:
            lib_target_path = self.package_name
        else:
            lib_target_path = os.path.join(self.build_lib, "grapapy-"+grapapy_version)
            self.mkpath(lib_target_path)
        
        # Copy main library
        self.copy_file(self.lib_source_path, os.path.join(lib_target_path, self.filename))
        
        # Copy platform-specific libraries
        if sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
            # Copy Grapa libraries
            for file_name in os.listdir(os.path.join(self.build_dir, 'grapa-lib/'+from_os)):
                self.copy_file(
                    os.path.join(os.path.join(self.build_dir, 'grapa-lib/'+from_os), file_name),
                    os.path.join(lib_target_path, file_name)
                )
            
            # Copy OpenSSL libraries
            for file_name in os.listdir(os.path.join(self.build_dir, 'openssl-lib/'+from_os)):
                self.copy_file(
                    os.path.join(os.path.join(self.build_dir, 'openssl-lib/'+from_os), file_name),
                    os.path.join(lib_target_path, file_name)
                )
            
            # Copy BLST libraries
            for file_name in os.listdir(os.path.join(self.build_dir, 'blst-lib/'+from_os)):
                self.copy_file(
                    os.path.join(os.path.join(self.build_dir, 'blst-lib/'+from_os), file_name),
                    os.path.join(lib_target_path, file_name)
                )
            
            # Copy FL libraries
            for file_name in os.listdir(os.path.join(self.build_dir, 'fl-lib/'+from_os)):
                self.copy_file(
                    os.path.join(os.path.join(self.build_dir, 'fl-lib/'+from_os), file_name),
                    os.path.join(lib_target_path, file_name)
                )
            
            # Copy PCRE2 libraries
            for file_name in os.listdir(os.path.join(self.build_dir, 'pcre2-lib/'+from_os)):
                self.copy_file(
                    os.path.join(os.path.join(self.build_dir, 'pcre2-lib/'+from_os), file_name),
                    os.path.join(lib_target_path, file_name)
                )
        
        # Set runtime library path for Linux
        if sys.platform.startswith('linux'):
            os.environ["ORIGIN"] = os.path.abspath(lib_target_path)
```

### **5. Extension Module Definition**

#### **GrapaPy Extension Module**
```python
lib_grapa = Extension(
    'grapapy', 
    sources=['source/mainpy.cpp'],
    include_dirs=include_dirs,
    library_dirs=pick_library_dirs(),
    libraries=pick_libraries(),
    runtime_library_dirs=runtime_library_dirs,
    extra_link_args=extra_link_args,
    extra_compile_args=extra_compile_args,
)
```

### **6. Platform-Specific Setup Configuration**

#### **Setuptools Configuration (Linux/Windows)**
```python
if sys.platform.startswith('linux') or sys.platform.startswith('win32'):
    setup(
        name="grapapy",
        version=grapapy_version,
        author="Chris Matichuk",
        author_email="matichuk@hotmail.com",
        description="GrapaPy brings robust, production-ready parallel ETL/data processing to Python.",
        long_description="""
GrapaPy is a Python extension for the Grapa language, designed for advanced data processing, ETL, and language experimentation. GrapaPy brings robust, production-ready parallel ETL/data processing to Python. By leveraging Grapa's C++ backend, GrapaPy enables true parallelism for high-throughput data workflows—bypassing Python's GIL and making advanced data processing simple and fast.

Features:
- True parallel ETL/data processing from Python
- Hardened, production-ready parallelism
- High performance for large file processing, data transformation, analytics, and more
- Simple, functional API

For comprehensive documentation, visit: https://grapa-dev.github.io/grapa/
""",
        long_description_content_type="text/markdown",
        url="https://grapa-dev.github.io/grapa/",
        ext_modules=[lib_grapa],
        cmdclass={
            'copy_grapalib': CopySharedLibrary,
            'build_ext': CustomBuildExt,
            'build': CustomBuild,
        },
        zip_safe=False,
        python_requires=">=3.6",
        packages=find_packages(),
        options={
            'build_ext': {
                'build_temp': os.path.join(os.getcwd(), 'build_temp'),
            }
        },
    )
```

#### **DistUtils Configuration (macOS - Legacy)**
```python
else:  # macOS
    setup(
        name="grapapy",
        version=grapapy_version,
        author="Chris Matichuk",
        description="Grammar-based language and parser with unlimited precision, parallelism, and Python integration",
        long_description=(
            "Grapa is a modern grammar-based programming language and parser designed for Python users, educators, "
            "researchers, and data engineers. It features unlimited precision math, runtime-mutable grammars, true parallelism, "
            "and seamless Python integration via GrapaPy. Rapidly prototype languages, process data, and experiment with advanced grammars—all with a unified, high-performance toolset.\n\n"
            "For comprehensive documentation, visit: https://grapa-dev.github.io/grapa/\n\n"
            "See the [Grapa project on GitHub](https://github.com/grapa-dev/grapa) for full documentation, examples, and installation instructions."
        ),
        long_description_content_type="text/markdown",
        url="https://grapa-dev.github.io/grapa/",
        ext_modules=[CMakeExtension("grapapy")],
        cmdclass={"build_ext": CMakeBuild},
        zip_safe=False,
        python_requires=">=3.6",
    )
```

## Integration with build.py

### **Python Package Building in build.py**
```python
def build_python_package(self, config: BuildConfig):
    """Build Python package using setup.py"""
    print("Building Python package...")
    
    # Build package
    python_cmd = "python" if config.platform == "windows" else "python3"
    pip_cmd = "pip" if config.platform == "windows" else "pip3"
    subprocess.run([python_cmd, "setup.py", "sdist"], check=True)
    
    # Find the built package file
    dist_files = list(Path("dist").glob("*.tar.gz"))
    if not dist_files:
        raise RuntimeError("No Python package found in dist/ directory")
    
    package_file = dist_files[0].name
    print(f"Found package: {package_file}")
    
    # Install package
    package_path = os.path.join("dist", package_file)
    subprocess.run([pip_cmd, "install", package_path], check=True)
```

## Platform-Specific Dependencies

### **Windows Dependencies**
- **Windows SDK**: UCRT, shared, um libraries
- **Static Libraries**: UTF8PROC_STATIC, PCRE2_STATIC
- **System Libraries**: Gdi32, Advapi32, User32, Ole32, Shell32, Comdlg32
- **Build System**: Setuptools + CMake

### **Linux Dependencies**
- **X11 Libraries**: X11, Xfixes, Xft, Xext, Xrender, Xinerama, Xcursor
- **System Libraries**: xcb, Xau, png, fontconfig, freetype
- **Runtime Libraries**: pthread, dl, m
- **Build System**: Setuptools + CMake
- **Library Type**: Shared libraries with runtime path resolution

### **macOS Dependencies (Current - DistUtils)**
- **Frameworks**: CoreFoundation, AppKit, IOKit
- **Compiler**: clang++ with libc++
- **Build System**: **DistUtils (Legacy)**
- **Library Type**: Static libraries
- **Architecture**: AMD64 and ARM64 support

### **AWS Linux Dependencies**
- **Same as Linux**: X11 libraries and system dependencies
- **Platform Detection**: Special detection for Amazon Linux
- **Build System**: Setuptools + CMake
- **Library Type**: Shared libraries

## DistUtils vs Setuptools Migration

### **Current Status**
- **Windows/Linux/AWS**: Using Setuptools + CMake
- **macOS**: Still using DistUtils (legacy)
- **Backlog Item**: Migrate macOS to Setuptools

### **Migration Requirements for macOS**
```python
# Target configuration for macOS (after migration)
if sys.platform.startswith('darwin'):
    # Use Setuptools instead of DistUtils
    setup(
        name="grapapy",
        version=grapapy_version,
        ext_modules=[lib_grapa],  # Use Extension instead of CMakeExtension
        cmdclass={
            'copy_grapalib': CopySharedLibrary,
            'build_ext': CustomBuildExt,
            'build': CustomBuild,
        },
        # ... other configuration
    )
```

### **Migration Benefits**
- **Consistency**: All platforms use same build system
- **Modern Features**: Better dependency management
- **Maintenance**: Single build system to maintain
- **Compatibility**: Better Python version support

## Build Process Flow

### **1. Platform Detection**
```
build.py → detect_platform() → BuildConfig
├── Windows: win-amd64
├── macOS: mac-amd64/mac-arm64
├── Linux: linux-amd64/linux-arm64
└── AWS: aws-amd64/aws-arm64
```

### **2. Native Binary Build**
```
build.py → build_native() → platform-specific build
├── Windows: MSBuild
├── macOS: clang++
├── Linux: g++
└── AWS: g++
```

### **3. Python Package Build**
```
build.py → build_python_package() → setup.py
├── Platform Detection
├── Library Selection
├── Extension Definition
├── CMake Build (Setuptools)
├── DistUtils Build (macOS)
└── Package Installation
```

### **4. Library Copying**
```
CopySharedLibrary → copy_libraries()
├── Main Grapa Library
├── OpenSSL Libraries
├── BLST Libraries
├── FL Libraries
└── PCRE2 Libraries
```

## Error Handling

### **Permission Issues**
```python
try:
    super().build_extension(ext)
except PermissionError as e:
    if "cache" in str(e).lower():
        print("\n" + "="*60)
        print("PERMISSION ERROR: Pip cannot write to its cache directory.")
        print("SOLUTION: Use one of these commands:")
        print("  pip install --no-cache-dir dist/grapapy-0.0.25.tar.gz")
        print("  pip install --no-cache-dir -e .")
        print("="*60)
    raise
```

### **Platform Detection Errors**
```python
def pick_library_dirs():
    my_system = platform.system()
    # ... platform-specific logic
    raise ValueError("Unknown platform: " + my_system)
```

## Performance Characteristics

### **Build Performance**
- **Parallel Builds**: CMake parallel compilation
- **Caching**: Build artifact caching
- **Incremental Builds**: Only rebuild changed components
- **Cross-Platform**: Platform-specific optimizations

### **Runtime Performance**
- **Static Linking**: Windows and macOS use static libraries
- **Shared Libraries**: Linux uses shared libraries with runtime path resolution
- **Optimization**: -O3 optimization flags
- **Architecture**: ARM64 and AMD64 support

## Security Considerations

### **Library Security**
- **Static Libraries**: Reduce dependency vulnerabilities
- **Version Pinning**: Specific library versions
- **Path Validation**: Secure library path handling
- **Permission Checks**: Proper file permissions

### **Build Security**
- **Input Validation**: Platform detection validation
- **Path Sanitization**: Secure path handling
- **Error Isolation**: Build failures don't affect system
- **Dependency Verification**: Library integrity checks

## Future Enhancements

### **Planned Improvements**
- **macOS Migration**: Complete DistUtils to Setuptools migration
- **Cross-Compilation**: Support for cross-platform builds
- **Container Support**: Docker-based builds
- **CI/CD Integration**: Automated build pipelines

### **Build System Enhancements**
- **Modern CMake**: Updated CMake configurations
- **Package Managers**: Integration with vcpkg, conan
- **Dependency Management**: Automated dependency resolution
- **Build Caching**: Advanced build artifact caching

## Related Documentation

- **[MAINPY_IMPLEMENTATION.md](MAINPY_IMPLEMENTATION.md)** - Main Python integration implementation
- **[GRAPAPY_IMPLEMENTATION.md](GRAPAPY_IMPLEMENTATION.md)** - GrapaPy Python extension details
- **[TYPE_MAPPING_IMPLEMENTATION.md](TYPE_MAPPING_IMPLEMENTATION.md)** - Type conversion system
- **[FOREIGN_FUNCTION_INTERFACE.md](FOREIGN_FUNCTION_INTERFACE.md)** - Python script execution
- **[docs-src/docs/python_use_cases.md](../../../docs-src/docs/python_use_cases.md)** - Python integration examples
- **[BUILD.md](../../../BUILD.md)** - General build instructions
