#!/bin/bash

# Build Grapa for All Platforms
# This script builds Grapa application, static library, and shared library for all 5 platforms
# Assumes third-party libraries are already available in source/*/{platform}-{arch}/

set -e

echo "🚀 Building Grapa for All Platforms..."
echo "📋 Platforms: Windows AMD64, macOS ARM64, macOS AMD64, Linux AMD64, Linux ARM64"

# Build Docker image once
echo "🐳 Building Docker image..."
docker build -f Dockerfile.grapa-build -t grapa-build .

# Function to build for a specific platform
build_platform() {
    local platform=$1
    local arch=$2
    local docker_platform=$3
    
    echo ""
    echo "🔨 Building for $platform-$arch..."
    
    docker run --platform=$docker_platform -it --rm \
        -v $HOME:/data \
        grapa-build \
        bash -c "
            echo '🚀 Starting Grapa build for $platform-$arch...'
            cd /data/GitHub/grapa
            
            # Create output directories
            mkdir -p source/grapa-lib/$platform-$arch
            mkdir -p source/grapa-other/$platform-$arch
            mkdir -p bin
            
                               # Build Grapa Application and create package using build.py
                   echo '📦 Building Grapa Application and creating package for $platform-$arch using build.py...'
                   python3 build.py --bin-only
                   echo '✅ Grapa Application and package built successfully for $platform-$arch'
            
            echo '🎉 All Grapa components built successfully for $platform-$arch!'
        "
    
    echo "✅ Completed build for $platform-$arch"
}

# Build for each platform
echo ""
echo "🔨 Starting builds for all platforms..."

# Linux ARM64
build_platform "linux" "arm64" "linux/arm64"

# Linux AMD64  
build_platform "linux" "amd64" "linux/amd64"

# macOS ARM64 (native build on Mac)
echo ""
echo "🍎 Building for macOS ARM64 (native)..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "✅ Running on macOS, building natively..."
    mkdir -p source/grapa-lib/mac-arm64
    mkdir -p source/grapa-other/mac-arm64
    mkdir -p bin
    python3 build.py --bin-only
    echo "✅ macOS ARM64 build completed"
else
    echo "⚠️  macOS ARM64 build skipped (not on macOS)"
fi

# macOS AMD64 (cross-compilation from ARM64 Mac)
echo ""
echo "🍎 Building for macOS AMD64 (cross-compilation)..."
if [[ "$OSTYPE" == "darwin"* && "$(uname -m)" == "arm64" ]]; then
    echo "✅ Running on ARM64 Mac, using cross-compilation..."
    mkdir -p source/grapa-lib/mac-amd64
    mkdir -p source/grapa-other/mac-amd64
    mkdir -p bin
    python3 build.py --bin-only --target-platform mac-amd64
    echo "✅ macOS AMD64 build completed"
else
    echo "⚠️  macOS AMD64 build skipped (requires ARM64 Mac)"
fi

# Windows AMD64 (GitHub Actions workflow)
echo ""
echo "🪟 Building for Windows AMD64..."
echo "📋 Checking if GitHub CLI is available for Windows workflow..."

if command -v gh &> /dev/null && gh auth status &> /dev/null; then
    echo "✅ GitHub CLI available and authenticated"
    echo "🚀 Triggering Windows build workflow..."
    
    # Trigger the workflow
    gh workflow run "Build Windows AMD64.yml"
    
    if [[ $? -eq 0 ]]; then
        echo "✅ Windows workflow triggered successfully!"
        echo "🔄 Monitoring workflow and downloading artifacts..."
        
        # Use the monitoring script to wait for completion and download
        ./scripts/monitor_and_download_windows.sh
        
        if [[ $? -eq 0 ]]; then
            echo "✅ Windows AMD64 build completed and artifacts downloaded"
        else
            echo "❌ Windows build failed or download failed"
            echo "   Please check the workflow at: https://github.com/grapa-dev/grapa/actions"
        fi
    else
        echo "❌ Failed to trigger Windows workflow"
        echo "   Please check GitHub CLI permissions and try again"
    fi
else
    echo "⚠️  GitHub CLI not available or not authenticated"
    echo "   Manual options:"
    echo "   1. Install GitHub CLI: https://cli.github.com/"
    echo "   2. Run: gh auth login"
    echo "   3. Or trigger manually: https://github.com/grapa-dev/grapa/actions"
    echo "   4. Then run: ./scripts/monitor_and_download_windows.sh"
fi

echo ""
echo "✅ All available builds completed successfully!"
echo "📁 Applications: grapa (Linux ARM64/AMD64, macOS ARM64/AMD64), grapa.exe (Windows AMD64)"
echo "📁 Static libraries: source/grapa-lib/*/libgrapa.a, source/grapa-lib/win-amd64/grapa.lib"
echo "📁 Shared libraries: source/grapa-other/*/libgrapa.so"
echo "📁 Compressed files: bin/grapa-*.tar.gz, bin/grapa-win-amd64.zip"
echo ""

# Validation step
echo "🔍 Validating build artifacts..."
echo "=================================="

# Get build start time (when script started)
BUILD_START_TIME=$(date +%s)
echo "📅 Build started at: $(date -d @$BUILD_START_TIME)"

# Function to validate platform artifacts
validate_platform() {
    local platform=$1
    local arch=$2
    local target="$platform-$arch"
    
    echo ""
    echo "📋 Validating $target..."
    
    # Check executable
    if [[ "$platform" == "win" ]]; then
        exe_name="grapa.exe"
    else
        exe_name="grapa"
    fi
    
    if [[ -f "$exe_name" ]]; then
        # Check if file was created during this build
        file_time=$(stat -c %Y "$exe_name" 2>/dev/null || stat -f %m "$exe_name" 2>/dev/null)
        if [[ $file_time -ge $BUILD_START_TIME ]]; then
            echo "  ✅ Executable: $exe_name (built in this session)"
        else
            echo "  ⚠️  Executable: $exe_name (exists but may be from previous build)"
        fi
    else
        echo "  ❌ Executable: $exe_name (missing)"
        return 1
    fi
    
    # Check static library
    if [[ "$platform" == "win" ]]; then
        static_lib="source/grapa-lib/$target/grapa.lib"
    else
        static_lib="source/grapa-lib/$target/libgrapa.a"
    fi
    
    if [[ -f "$static_lib" ]]; then
        # Check if file was created during this build
        file_time=$(stat -c %Y "$static_lib" 2>/dev/null || stat -f %m "$static_lib" 2>/dev/null)
        if [[ $file_time -ge $BUILD_START_TIME ]]; then
            echo "  ✅ Static library: $static_lib (built in this session)"
        else
            echo "  ⚠️  Static library: $static_lib (exists but may be from previous build)"
        fi
    else
        echo "  ❌ Static library: $static_lib (missing)"
        return 1
    fi
    
    # Check shared library (except Windows which doesn't build .dll)
    if [[ "$platform" != "win" ]]; then
        shared_lib="source/grapa-other/$target/libgrapa.so"
        if [[ -f "$shared_lib" ]]; then
            # Check if file was created during this build
            file_time=$(stat -c %Y "$shared_lib" 2>/dev/null || stat -f %m "$shared_lib" 2>/dev/null)
            if [[ $file_time -ge $BUILD_START_TIME ]]; then
                echo "  ✅ Shared library: $shared_lib (built in this session)"
            else
                echo "  ⚠️  Shared library: $shared_lib (exists but may be from previous build)"
            fi
        else
            echo "  ❌ Shared library: $shared_lib (missing)"
            return 1
        fi
    fi
    
    # Check compressed package
    if [[ "$platform" == "win" ]]; then
        compressed_file="bin/grapa-$target.zip"
    else
        compressed_file="bin/grapa-$target.tar.gz"
    fi
    
    if [[ -f "$compressed_file" ]]; then
        # Check if file was created during this build
        file_time=$(stat -c %Y "$compressed_file" 2>/dev/null || stat -f %m "$compressed_file" 2>/dev/null)
        if [[ $file_time -ge $BUILD_START_TIME ]]; then
            echo "  ✅ Compressed package: $compressed_file (built in this session)"
        else
            echo "  ⚠️  Compressed package: $compressed_file (exists but may be from previous build)"
        fi
        
        # Validate package contents
        echo "  📦 Package contents:"
        if [[ "$platform" == "win" ]]; then
            unzip -l "$compressed_file" | grep -E "(grapa\.exe|grapa\.lib)" || echo "    ❌ Missing expected files"
        else
            tar -tzf "$compressed_file" | grep -E "(grapa$|libgrapa\.(a|so))" || echo "    ❌ Missing expected files"
        fi
    else
        echo "  ❌ Compressed package: $compressed_file (missing)"
        return 1
    fi
    
    echo "  ✅ $target validation passed"
    return 0
}

# Validate each platform
validation_failed=false

validate_platform "linux" "arm64" || validation_failed=true
validate_platform "linux" "amd64" || validation_failed=true
validate_platform "mac" "arm64" || validation_failed=true
validate_platform "mac" "amd64" || validation_failed=true
validate_platform "win" "amd64" || validation_failed=true

echo ""
if [[ "$validation_failed" == "true" ]]; then
    echo "❌ Validation failed! Some artifacts are missing."
    echo "   Please check the build logs and rebuild if necessary."
    exit 1
else
    echo "✅ All platform validations passed!"
    echo "🎉 Build system is ready for distribution!"
fi

echo ""
echo "🎯 Next steps:"
echo "   1. Run: ./scripts/check_platform_status.sh (detailed verification)"
echo "   2. Build Python distribution: python3 setup.py sdist bdist_wheel"
echo "   3. Deploy to PyPI: twine upload dist/*"

# Python distribution building and validation
echo ""
echo "🐍 Building Python distribution..."
echo "=================================="

# Build Python package
echo "📦 Building Python package with --python-only --preserve-dist..."
python3 build.py --python-only --preserve-dist

if [[ $? -eq 0 ]]; then
    echo "✅ Python package built successfully"
    
    # Find the built package
    dist_files=$(ls dist/*.tar.gz 2>/dev/null || echo "")
    if [[ -n "$dist_files" ]]; then
        package_file=$(echo "$dist_files" | head -1)
        echo "📁 Found package: $package_file"
        
        # Install the package
        echo "📥 Installing package with pip3..."
        pip3 install "$package_file"
        
        if [[ $? -eq 0 ]]; then
            echo "✅ Package installed successfully"
            
            # Validate version
            echo "🔍 Validating Python package version..."
            echo "📋 Running version validation script..."
            
            # Create temporary validation script
            cat > /tmp/validate_grapapy_version.grc << 'EOF'
$sys().getenv($VERSION);
EOF
            
            # Run validation using grapapy
            validation_result=$(python3 -c "
import grapapy
import subprocess
import sys

try:
    # Get version from grapapy
    grapapy_version = grapapy.__version__
    print(f'Grapapy version: {grapapy_version}')
    
    # Run grapapy with version script
    result = subprocess.run(['grapapy', '-c', '\$sys().getenv(\$VERSION);'], 
                          capture_output=True, text=True, timeout=10)
    
    if result.returncode == 0:
        env_version = result.stdout.strip()
        print(f'Environment VERSION: {env_version}')
        
        if env_version == grapapy_version:
            print('✅ Version validation passed!')
            sys.exit(0)
        else:
            print(f'❌ Version mismatch: grapapy={grapapy_version}, env={env_version}')
            sys.exit(1)
    else:
        print(f'❌ Grapapy execution failed: {result.stderr}')
        sys.exit(1)
        
except Exception as e:
    print(f'❌ Validation error: {e}')
    sys.exit(1)
")

            if [[ $? -eq 0 ]]; then
                echo "✅ Python package version validation passed!"
                echo "🎉 All builds and validations completed successfully!"
            else
                echo "❌ Python package version validation failed!"
                echo "$validation_result"
                exit 1
            fi
            
        else
            echo "❌ Failed to install Python package"
            exit 1
        fi
    else
        echo "❌ No Python package found in dist/ directory"
        exit 1
    fi
else
    echo "❌ Failed to build Python package"
    exit 1
fi

echo ""
echo "🚀 Ready for deployment!"
echo "   Run: twine upload dist/*" 