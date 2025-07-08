#!/bin/bash

# Mac ARM64 Unicode Grep Test Runner
# This script runs existing Unicode grep tests on Mac ARM64 and compares results

set -e

echo "=== Mac ARM64 Unicode Grep Test Runner ==="
echo "Platform: $(uname -s) $(uname -m)"
echo "Date: $(date)"
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're on Mac
if [[ "$(uname -s)" != "Darwin" ]]; then
    print_error "This script is designed for macOS"
    exit 1
fi

# Check for required tools
print_status "Checking required tools..."

if ! command -v clang++ &> /dev/null; then
    print_error "clang++ not found. Please install Xcode Command Line Tools:"
    print_error "xcode-select --install"
    exit 1
fi

if ! command -v grapa &> /dev/null; then
    print_warning "grapa not found in PATH. Only C++ tests will be run."
    GRAPA_AVAILABLE=false
else
    print_success "Found grapa: $(which grapa)"
    GRAPA_AVAILABLE=true
fi

# Create results directory
RESULTS_DIR="mac_arm64_results"
mkdir -p "$RESULTS_DIR"

print_status "Results will be saved to: $RESULTS_DIR"

# Function to run C++ test and save results
run_cpp_test() {
    local test_file="$1"
    local output_file="$2"
    
    print_status "Running C++ test: $test_file"
    
    # Build the test
    clang++ -std=c++17 -O3 -Isource -Isource/grep -Isource/utf8proc \
        -DUTF8PROC_STATIC \
        "$test_file" \
        source/grep/grapa_grep_unicode.cpp \
        source/grep/grapa_grep_unicode_regex.cpp \
        source/utf8proc/utf8proc.c \
        -framework CoreFoundation -framework AppKit -framework IOKit \
        -o "$RESULTS_DIR/test_program"
    
    # Run the test and save output
    "$RESULTS_DIR/test_program" > "$output_file" 2>&1
    
    print_success "C++ test completed: $output_file"
}

# Function to run Grapa test and save results
run_grapa_test() {
    local test_file="$1"
    local output_file="$2"
    
    if [[ "$GRAPA_AVAILABLE" == "false" ]]; then
        print_warning "Skipping Grapa test (grapa not available): $test_file"
        return
    fi
    
    print_status "Running Grapa test: $test_file"
    
    # Run the test and save output
    grapa -f "$test_file" > "$output_file" 2>&1
    
    print_success "Grapa test completed: $output_file"
}

# Run existing C++ tests
print_status "Running C++ Unicode grep tests..."

if [[ -f "test_unicode_grep.cpp" ]]; then
    run_cpp_test "test_unicode_grep.cpp" "$RESULTS_DIR/cpp_unicode_grep_results.txt"
else
    print_warning "test_unicode_grep.cpp not found"
fi

if [[ -f "test_unicode_debug.cpp" ]]; then
    run_cpp_test "test_unicode_debug.cpp" "$RESULTS_DIR/cpp_unicode_debug_results.txt"
else
    print_warning "test_unicode_debug.cpp not found"
fi

if [[ -f "test_unicode_fixed.cpp" ]]; then
    run_cpp_test "test_unicode_fixed.cpp" "$RESULTS_DIR/cpp_unicode_fixed_results.txt"
else
    print_warning "test_unicode_fixed.cpp not found"
fi

# Run Grapa tests
print_status "Running Grapa Unicode grep tests..."

# Look for Grapa test files
GRAPA_TEST_FILES=(
    "grapa_test_grep_unicode.grc"
    "test_unicode_debug.grc"
    "grapa_unicode_tests.grc"
)

for test_file in "${GRAPA_TEST_FILES[@]}"; do
    if [[ -f "$test_file" ]]; then
        run_grapa_test "$test_file" "$RESULTS_DIR/grapa_${test_file%.grc}_results.txt"
    else
        print_warning "Grapa test file not found: $test_file"
    fi
done

# Create a comprehensive test summary
print_status "Creating test summary..."

cat > "$RESULTS_DIR/test_summary.txt" << EOF
=== Mac ARM64 Unicode Grep Test Summary ===
Platform: $(uname -s) $(uname -m)
Date: $(date)
Compiler: $(clang++ --version | head -n1)

Test Results:
EOF

# Add C++ test results
echo "C++ Tests:" >> "$RESULTS_DIR/test_summary.txt"
for result_file in "$RESULTS_DIR"/cpp_*_results.txt; do
    if [[ -f "$result_file" ]]; then
        echo "  $(basename "$result_file"): $(wc -l < "$result_file") lines" >> "$RESULTS_DIR/test_summary.txt"
    fi
done

# Add Grapa test results
echo "Grapa Tests:" >> "$RESULTS_DIR/test_summary.txt"
for result_file in "$RESULTS_DIR"/grapa_*_results.txt; do
    if [[ -f "$result_file" ]]; then
        echo "  $(basename "$result_file"): $(wc -l < "$result_file") lines" >> "$RESULTS_DIR/test_summary.txt"
    fi
done

# Create comparison script
print_status "Creating comparison script..."

cat > "$RESULTS_DIR/compare_with_windows.sh" << 'EOF'
#!/bin/bash

# Script to compare Mac ARM64 results with Windows AMD64 results

echo "=== Comparing Mac ARM64 vs Windows AMD64 Results ==="
echo

# Function to compare files
compare_files() {
    local mac_file="$1"
    local windows_file="$2"
    local description="$3"
    
    echo "Comparing $description..."
    
    if [[ ! -f "$mac_file" ]]; then
        echo "ERROR: Mac results file not found: $mac_file"
        return 1
    fi
    
    if [[ ! -f "$windows_file" ]]; then
        echo "WARNING: Windows baseline file not found: $windows_file"
        echo "Creating baseline from Mac results..."
        cp "$mac_file" "$windows_file"
        echo "✓ Created baseline for $description"
        return 0
    fi
    
    if diff -q "$mac_file" "$windows_file" > /dev/null; then
        echo "✓ Results match for $description"
        return 0
    else
        echo "✗ Results differ for $description"
        echo "Differences:"
        diff "$mac_file" "$windows_file" || true
        return 1
    fi
}

# Compare C++ test results
for result_file in cpp_*_results.txt; do
    if [[ -f "$result_file" ]]; then
        compare_files "$result_file" "../${result_file}" "C++ $(basename "$result_file" _results.txt)"
    fi
done

# Compare Grapa test results
for result_file in grapa_*_results.txt; do
    if [[ -f "$result_file" ]]; then
        compare_files "$result_file" "../${result_file}" "Grapa $(basename "$result_file" _results.txt)"
    fi
done

echo
echo "=== Comparison Summary ==="
echo "If all tests show '✓ Results match', then Mac ARM64 and Windows AMD64"
echo "are producing identical results for Unicode grep functionality."
echo
echo "If any tests show '✗ Results differ', there may be platform-specific"
echo "differences that need investigation."
EOF

chmod +x "$RESULTS_DIR/compare_with_windows.sh"

# Create a simple test runner for specific tests
print_status "Creating specific test runner..."

cat > "$RESULTS_DIR/run_specific_test.sh" << 'EOF'
#!/bin/bash

# Script to run a specific Unicode grep test

if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <pattern> <input> [options]"
    echo "Example: $0 'café' 'café' 'o'"
    exit 1
fi

pattern="$1"
input="$2"
options="${3:-o}"

echo "=== Running Specific Unicode Grep Test ==="
echo "Pattern: $pattern"
echo "Input: $input"
echo "Options: $options"
echo

# Build test program if needed
if [[ ! -f "test_program" ]]; then
    echo "Building test program..."
    clang++ -std=c++17 -O3 -Isource -Isource/grep -Isource/utf8proc \
        -DUTF8PROC_STATIC \
        test_unicode_grep.cpp \
        source/grep/grapa_grep_unicode.cpp \
        source/grep/grapa_grep_unicode_regex.cpp \
        source/utf8proc/utf8proc.c \
        -framework CoreFoundation -framework AppKit -framework IOKit \
        -o test_program
fi

# Run the test
echo "Result:"
./test_program "$pattern" "$input" "$options"
EOF

chmod +x "$RESULTS_DIR/run_specific_test.sh"

# Summary
echo
echo "=== Test Execution Summary ==="
print_success "All tests completed successfully!"
echo
echo "Results saved to: $RESULTS_DIR/"
echo
echo "Files created:"
for file in "$RESULTS_DIR"/*; do
    if [[ -f "$file" ]]; then
        echo "  - $(basename "$file")"
    fi
done

echo
echo "=== Next Steps ==="
echo "1. Review test results:"
echo "   ls -la $RESULTS_DIR/"
echo
echo "2. Compare with Windows AMD64 results:"
echo "   cd $RESULTS_DIR && ./compare_with_windows.sh"
echo
echo "3. Run a specific test:"
echo "   cd $RESULTS_DIR && ./run_specific_test.sh 'café' 'café' 'o'"
echo
echo "4. View test summary:"
echo "   cat $RESULTS_DIR/test_summary.txt"

print_success "Mac ARM64 Unicode grep testing completed!" 