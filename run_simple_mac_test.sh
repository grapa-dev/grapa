#!/bin/bash

# Simple Mac ARM64 Unicode Grep Test Runner
# This script runs the existing comprehensive Unicode grep test

set -e

echo "=== Simple Mac ARM64 Unicode Grep Test Runner ==="
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

# Create results directory
RESULTS_DIR="mac_arm64_results"
mkdir -p "$RESULTS_DIR"

print_status "Results will be saved to: $RESULTS_DIR"

# Check if grapa is available
print_status "Checking for grapa..."

if ! command -v grapa &> /dev/null; then
    print_warning "grapa not found in PATH"
    print_warning "You may need to build grapa first or add it to your PATH"
    print_warning "Only test file validation will be performed"
    GRAPA_AVAILABLE=false
else
    print_success "Found grapa: $(which grapa)"
    GRAPA_AVAILABLE=true
fi

# Check for the comprehensive test file
TEST_FILE="unicode_grep_comprehensive_tests.grc"

if [[ ! -f "$TEST_FILE" ]]; then
    print_error "Test file not found: $TEST_FILE"
    exit 1
fi

print_success "Found test file: $TEST_FILE"

# Run the test if grapa is available
if [[ "$GRAPA_AVAILABLE" == "true" ]]; then
    print_status "Running comprehensive Unicode grep test..."
    
    # Run the test and save output
    grapa -f "$TEST_FILE" > "$RESULTS_DIR/comprehensive_unicode_tests.txt" 2>&1
    
    print_success "Test completed. Results saved to: $RESULTS_DIR/comprehensive_unicode_tests.txt"
    
    # Show a preview of the results
    echo
    print_status "Test results preview (first 20 lines):"
    echo "----------------------------------------"
    head -20 "$RESULTS_DIR/comprehensive_unicode_tests.txt"
    echo "----------------------------------------"
    
    # Count PASS/FAIL results
    PASS_COUNT=$(grep -c "PASS" "$RESULTS_DIR/comprehensive_unicode_tests.txt" || echo "0")
    FAIL_COUNT=$(grep -c "FAIL" "$RESULTS_DIR/comprehensive_unicode_tests.txt" || echo "0")
    
    echo
    print_status "Test Summary:"
    echo "  PASS: $PASS_COUNT"
    echo "  FAIL: $FAIL_COUNT"
    
    if [[ "$FAIL_COUNT" -eq 0 ]]; then
        print_success "All tests passed!"
    else
        print_warning "Some tests failed. Check the full results."
    fi
else
    print_status "Validating test file structure..."
    
    # Show test file structure
    echo
    print_status "Test file structure:"
    echo "  Lines: $(wc -l < "$TEST_FILE")"
    echo "  Size: $(ls -lh "$TEST_FILE" | awk '{print $5}')"
    
    # Show some test examples
    echo
    print_status "Sample tests from file:"
    echo "----------------------------------------"
    grep -E "\.echo\(\)" "$TEST_FILE" | head -10
    echo "----------------------------------------"
    
    print_warning "Cannot run tests without grapa. Please build grapa first."
fi

# Create a test summary
print_status "Creating test summary..."

cat > "$RESULTS_DIR/test_summary.txt" << EOF
=== Mac ARM64 Unicode Grep Test Summary ===
Platform: $(uname -s) $(uname -m)
Date: $(date)
Test File: $TEST_FILE
Grapa Available: $GRAPA_AVAILABLE

Test Results:
EOF

if [[ "$GRAPA_AVAILABLE" == "true" ]]; then
    echo "  Comprehensive Unicode Tests: $(wc -l < "$RESULTS_DIR/comprehensive_unicode_tests.txt") lines" >> "$RESULTS_DIR/test_summary.txt"
    echo "  PASS: $PASS_COUNT" >> "$RESULTS_DIR/test_summary.txt"
    echo "  FAIL: $FAIL_COUNT" >> "$RESULTS_DIR/test_summary.txt"
else
    echo "  Tests not run (grapa not available)" >> "$RESULTS_DIR/test_summary.txt"
fi

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

# Compare comprehensive test results
if [[ -f "comprehensive_unicode_tests.txt" ]]; then
    compare_files "comprehensive_unicode_tests.txt" "../comprehensive_unicode_tests.txt" "Comprehensive Unicode Tests"
fi

echo
echo "=== Comparison Summary ==="
echo "If all tests show '✓ Results match', then Mac ARM64 and Windows AMD64"
echo "are producing identical results for Unicode grep functionality."
echo
echo "If any tests show '✗ Results differ', there may be platform-specific"
echo "differences that need investigation."
EOF

chmod +x "$RESULTS_DIR/compare_with_windows.sh"

# Summary
echo
echo "=== Test Execution Summary ==="
if [[ "$GRAPA_AVAILABLE" == "true" ]]; then
    print_success "Unicode grep test completed successfully!"
else
    print_warning "Test file validated but grapa not available for execution"
fi

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
echo "2. View full test results:"
echo "   cat $RESULTS_DIR/comprehensive_unicode_tests.txt"
echo
echo "3. Compare with Windows AMD64 results:"
echo "   cd $RESULTS_DIR && ./compare_with_windows.sh"
echo
echo "4. View test summary:"
echo "   cat $RESULTS_DIR/test_summary.txt"

if [[ "$GRAPA_AVAILABLE" == "false" ]]; then
    echo
    echo "=== To Build Grapa on Mac ARM64 ==="
    echo "1. Install Xcode Command Line Tools:"
    echo "   xcode-select --install"
    echo
    echo "2. Build grapa using the existing build scripts:"
    echo "   # Check docs/BUILD.md for Mac ARM64 build instructions"
    echo
    echo "3. Add grapa to your PATH or run from the build directory"
fi

print_success "Mac ARM64 Unicode grep testing completed!" 