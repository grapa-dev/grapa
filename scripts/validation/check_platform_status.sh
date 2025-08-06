#!/bin/bash
# Check Platform Build Status
# This script shows the status of all platform builds and what's available

echo "🔍 Checking Platform Build Status..."
echo "=================================="

# Function to check if files exist for a platform
check_platform() {
    local platform=$1
    local arch=$2
    local target="$platform-$arch"
    
    echo ""
    echo "📋 Platform: $target"
    
    # Check executable
    if [[ "$platform" == "win" ]]; then
        exe_name="grapa.exe"
    else
        exe_name="grapa"
    fi
    
    if [[ -f "$exe_name" ]]; then
        echo "  ✅ Executable: $exe_name ($(stat -f%z "$exe_name" 2>/dev/null || stat -c%s "$exe_name" 2>/dev/null || echo "unknown") bytes)"
    else
        echo "  ❌ Executable: $exe_name (missing)"
    fi
    
               # Check static library
           if [[ "$platform" == "win" ]]; then
               static_lib="source/grapa-lib/$target/grapa.lib"
           else
               static_lib="source/grapa-lib/$target/libgrapa.a"
           fi
           if [[ -f "$static_lib" ]]; then
               echo "  ✅ Static library: $static_lib ($(stat -f%z "$static_lib" 2>/dev/null || stat -c%s "$static_lib" 2>/dev/null || echo "unknown") bytes)"
           else
               echo "  ❌ Static library: $static_lib (missing)"
           fi
           
           # Check shared library
           if [[ "$platform" == "win" ]]; then
               shared_lib="source/grapa-other/$target/grapa.dll"
           else
               shared_lib="source/grapa-other/$target/libgrapa.so"
           fi
           
           if [[ -f "$shared_lib" ]]; then
               echo "  ✅ Shared library: $shared_lib ($(stat -f%z "$shared_lib" 2>/dev/null || stat -c%s "$shared_lib" 2>/dev/null || echo "unknown") bytes)"
           else
               echo "  ❌ Shared library: $shared_lib (missing)"
           fi
           
           # Check compressed package
           if [[ "$platform" == "win" ]]; then
               compressed_file="bin/grapa-$target.zip"
           else
               compressed_file="bin/grapa-$target.tar.gz"
           fi
           if [[ -f "$compressed_file" ]]; then
               echo "  ✅ Compressed package: $compressed_file ($(stat -f%z "$compressed_file" 2>/dev/null || stat -c%s "$compressed_file" 2>/dev/null || echo "unknown") bytes)"
           else
               echo "  ❌ Compressed package: $compressed_file (missing)"
           fi
}

# Check each platform
check_platform "linux" "arm64"
check_platform "linux" "amd64"
check_platform "mac" "arm64"
check_platform "mac" "amd64"
check_platform "win" "amd64"

echo ""
echo "📊 Summary:"
echo "==========="

# Count available builds
total_platforms=5
available_builds=0

for platform in "linux-arm64" "linux-amd64" "mac-arm64" "mac-amd64" "win-amd64"; do
    if [[ -f "bin/grapa-$platform.tar.gz" ]]; then
        ((available_builds++))
    fi
done

echo "✅ Available builds: $available_builds/$total_platforms platforms"
echo ""
echo "🚀 To build missing platforms:"
echo "   Linux ARM64:   ./scripts/build/build_grapa_linux_arm64.sh"
echo "   Linux AMD64:   ./scripts/build/build_grapa_linux_amd64.sh"
echo "   macOS ARM64:   python3 build.py --bin-only"
echo "   macOS AMD64:   ./scripts/build/build_grapa_macos_amd64.sh (requires ARM64 Mac)"
echo "   Windows AMD64: ./scripts/build/build_grapa_windows_amd64.sh (requires Windows)"
echo ""
echo "🌐 Build all available: ./scripts/build/build_all_platforms.sh" 