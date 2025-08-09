# Distutils Removal and Shared Library Naming Plan

**Date**: August 2025  
**Status**: Planning Phase  
**Priority**: High (distutils deprecation)  
**Scope**: Mac and Linux build systems

## 🎯 **Executive Summary**

This plan addresses two related issues:
1. **Distutils Deprecation**: Remove dependency on `distutils` (deprecated since Python 3.12, removed in Python 3.13)
2. **Shared Library Naming**: Implement consistent naming conventions for static vs shared libraries across all platforms

## 📊 **Current State Analysis**

### **Distutils Usage**

#### **Active Dependencies in `setup.py`**
```python
# Current distutils imports
from distutils.command.build import build  # Line 6 - DEPRECATED
```

#### **Platform-Specific Issues**
- **Mac**: Uses `distutils.command.build` for custom build process
- **Linux**: Uses `setuptools` primarily (already modern)
- **Windows**: Uses `setuptools` primarily (already modern)

### **Shared Library Naming Issues**

#### **Current Naming Convention**
```
Platform    | Static Library | Shared Library | Location Conflict
------------|----------------|----------------|------------------
Mac         | libgrapa.a     | libgrapa.so    | ✅ Separate folders
Linux       | libgrapa.a     | libgrapa.so    | ❌ Same folder
AWS         | libgrapa.a     | libgrapa.so    | ❌ Same folder
Windows     | grapa.lib      | grapa.dll      | ❌ Same folder
```

#### **Mac-Specific Problem**
- **Shared library location**: `source/grapa-other/{platform}/libgrapa.so`
- **Static library location**: `source/grapa-lib/{platform}/libgrapa.a`
- **Issue**: This separation was implemented to avoid conflicts, but creates inconsistency

## 🎯 **Proposed Solution**

### **1. Distutils Removal Strategy**

#### **Phase 1: Replace distutils.command.build**
```python
# Replace this:
from distutils.command.build import build

# With this:
from setuptools.command.build import build
# OR
from setuptools import Command
```

#### **Phase 2: Update Custom Build Classes**
```python
# Current (deprecated):
class CustomBuild(build):
    sub_commands = [
        ('build_clib', build.has_c_libraries),
        ('build_ext', build.has_ext_modules),
        ('build_py', build.has_pure_modules),
        ('build_scripts', build.has_scripts),
    ]

# New (setuptools-based):
class CustomBuild(build):
    sub_commands = [
        ('build_clib', build.has_c_libraries),
        ('build_ext', build.has_ext_modules),
        ('build_py', build.has_pure_modules),
        ('build_scripts', build.has_scripts),
    ]
```

#### **Phase 3: Update Command Classes**
```python
# Current:
class CopySharedLibrary(Command):
    def initialize_options(self):
        self.build_lib = None
        self.inplace = 0
        # ... uses distutils patterns

# New:
class CopySharedLibrary(Command):
    def initialize_options(self):
        self.build_lib = None
        self.inplace = 0
        # ... uses setuptools patterns
```

### **2. Shared Library Naming Convention**

#### **Proposed Naming Scheme**
```
Platform    | Static Library    | Shared Library    | Location
------------|-------------------|-------------------|------------------
Mac         | libgrapa_static.a | libgrapa_shared.so| source/grapa-lib/{platform}/
Linux       | libgrapa_static.a | libgrapa_shared.so| source/grapa-lib/{platform}/
AWS         | libgrapa_static.a | libgrapa_shared.so| source/grapa-lib/{platform}/
Windows     | grapa_static.lib  | grapa_shared.dll  | source/grapa-lib/{platform}/
```

#### **Benefits of Consistent Naming**
1. **Unified locations**: All libraries in `source/grapa-lib/{platform}/`
2. **Clear distinction**: `_static` vs `_shared` suffix
3. **Simplified build process**: No need for separate `grapa-other` directory
4. **Consistent patterns**: Same naming convention across all platforms

## 🔄 **Implementation Plan**

### **Phase 1: Distutils Removal (Week 1-2)**

#### **Task 1.1: Update Imports**
- [ ] Replace `distutils.command.build` with `setuptools.command.build`
- [ ] Update any other distutils imports
- [ ] Test build process on all platforms

#### **Task 1.2: Update Command Classes**
- [ ] Refactor `CustomBuild` class to use setuptools
- [ ] Refactor `CopySharedLibrary` class to use setuptools
- [ ] Update `CustomBuildExt` class if needed
- [ ] Test command execution

#### **Task 1.3: Validation**
- [ ] Test build process on Mac (ARM64, AMD64)
- [ ] Test build process on Linux (ARM64, AMD64)
- [ ] Test build process on Windows (AMD64)
- [ ] Verify Python package creation

### **Phase 2: Shared Library Naming (Week 3-4)**

#### **Task 2.1: Update Build Scripts**
- [ ] Modify `build.py` to use new naming convention
- [ ] Update Mac build process (`_run_mac_build_command`)
- [ ] Update Linux build process (`_run_linux_build_command`)
- [ ] Update AWS build process (`_run_aws_build_command`)

#### **Task 2.2: Update Setup Scripts**
- [ ] Modify `setup.py` to reference new library names
- [ ] Update `pick_libraries()` function
- [ ] Update `pick_library_dirs()` function
- [ ] Update `CopySharedLibrary` command

#### **Task 2.3: Update CMakeLists.txt**
- [ ] Update library references in CMakeLists.txt
- [ ] Ensure all platforms use correct library names
- [ ] Test CMake builds

#### **Task 2.4: Cleanup**
- [ ] Remove `source/grapa-other` directory references
- [ ] Update documentation to reflect new naming
- [ ] Update any hardcoded library paths

### **Phase 3: Testing and Validation (Week 5)**

#### **Task 3.1: Comprehensive Testing**
- [ ] Test full build process on all platforms
- [ ] Verify Python package installation
- [ ] Test runtime functionality
- [ ] Verify library loading

#### **Task 3.2: Documentation Updates**
- [ ] Update build documentation
- [ ] Update developer guides
- [ ] Update deployment documentation

## 🎯 **Technical Details**

### **Mac Build Process Updates**

#### **Current Mac Build (Problematic)**
```python
# Current: Separate locations
shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
shutil.copy("libgrapa.so", f"source/grapa-other/{config.target}/libgrapa.so")
```

#### **Proposed Mac Build (Improved)**
```python
# New: Unified location with distinct names
shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa_static.a")
shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa_shared.so")
```

### **Linux Build Process Updates**

#### **Current Linux Build**
```python
# Current: Same names, same location
shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa.a")
shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa.so")
```

#### **Proposed Linux Build**
```python
# New: Distinct names, same location
shutil.copy("libgrapa.a", f"source/grapa-lib/{config.target}/libgrapa_static.a")
shutil.copy("libgrapa.so", f"source/grapa-lib/{config.target}/libgrapa_shared.so")
```

### **Setup.py Updates**

#### **Library Selection Logic**
```python
def pick_libraries():
    my_system = platform.system()
    if my_system == 'Linux':
        return ['grapa_static']  # Updated name
    if my_system == 'Darwin':
        return ['grapa_static']  # Updated name
    if my_system == 'Windows':
        return ["grapa_static", "Gdi32", "Advapi32", "User32", "Ole32", "Shell32", "Comdlg32"]
    raise ValueError("Unknown platform: " + my_system)
```

## 🚨 **Risk Assessment**

### **High Risk Items**
1. **Mac Build Process**: Complex dependency on static libraries
2. **Python Package Compatibility**: Changes may break existing installations
3. **CMake Integration**: Library name changes may affect CMake builds

### **Mitigation Strategies**
1. **Incremental Migration**: Test each platform individually
2. **Backward Compatibility**: Maintain old paths during transition
3. **Comprehensive Testing**: Test all build scenarios before deployment

### **Rollback Plan**
1. **Git Branch**: Maintain separate branch for this work
2. **Documentation**: Keep detailed notes of all changes
3. **Quick Revert**: Ability to revert to current working state

## 📋 **Success Criteria**

### **Functional Requirements**
- [ ] All builds work without distutils dependency
- [ ] Shared and static libraries use consistent naming
- [ ] All libraries located in unified `source/grapa-lib/{platform}/` directory
- [ ] Python package installation works on all platforms
- [ ] Runtime functionality preserved

### **Performance Requirements**
- [ ] Build times maintained or improved
- [ ] Package sizes maintained or reduced
- [ ] Runtime performance unaffected

### **Compatibility Requirements**
- [ ] Works with Python 3.8-3.13
- [ ] Works with all supported platforms (Mac, Linux, Windows, AWS)
- [ ] Maintains backward compatibility where possible

## 🔗 **Related Documentation**

- [BUILD_SYSTEM.md](../BUILD_AND_DEPLOYMENT/BUILD_SYSTEM.md) - Current build system reference
- [DOCUMENTATION_DEPLOYMENT.md](../BUILD_AND_DEPLOYMENT/DOCUMENTATION_DEPLOYMENT.md) - Deployment processes
- [setup.py](../../setup.py) - Current setup configuration
- [build.py](../../build.py) - Current build system

## 📅 **Timeline**

- **Week 1-2**: Distutils removal and testing
- **Week 3-4**: Shared library naming implementation
- **Week 5**: Comprehensive testing and documentation
- **Week 6**: Deployment and monitoring

## 🎯 **Next Steps**

1. **Approve this plan** - Review and approve implementation approach
2. **Create development branch** - Start work in isolated branch
3. **Begin Phase 1** - Start distutils removal
4. **Regular updates** - Provide progress updates during implementation 