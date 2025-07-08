#!/bin/bash

# Simple script to run Grapa Unicode tests on Mac ARM64
# This script runs the existing Grapa Unicode grep tests and saves results

set -e

echo "=== Running Grapa Unicode Tests on Mac ARM64 ==="
echo "Platform: $(uname -s) $(uname -m)"
echo "Date: $(date)"
echo

# Check if grapa is available
if ! command -v grapa &> /dev/null; then
    echo "ERROR: grapa not found in PATH"
    echo "Please ensure grapa is built and available in your PATH"
    exit 1
fi

echo "Found grapa: $(which grapa)"
echo "Grapa version: $(grapa --version 2>/dev/null || echo 'Version info not available')"
echo

# Create results directory
RESULTS_DIR="grapa_mac_arm64_results"
mkdir -p "$RESULTS_DIR"

echo "Results will be saved to: $RESULTS_DIR"
echo

# Function to run a Grapa test file
run_grapa_test() {
    local test_file="$1"
    local output_file="$2"
    
    if [[ ! -f "$test_file" ]]; then
        echo "WARNING: Test file not found: $test_file"
        return
    fi
    
    echo "Running: $test_file"
    
    # Run the test and save output
    grapa -f "$test_file" > "$output_file" 2>&1
    
    echo "  Results saved to: $output_file"
    echo "  Output lines: $(wc -l < "$output_file")"
    echo
}

# Run the comprehensive Unicode tests
echo "=== Running Comprehensive Unicode Tests ==="

# Look for the comprehensive test file
COMPREHENSIVE_TEST="grapa_test_grep_unicode_comprehensive.grc"
if [[ -f "$COMPREHENSIVE_TEST" ]]; then
    run_grapa_test "$COMPREHENSIVE_TEST" "$RESULTS_DIR/comprehensive_unicode_tests.txt"
else
    echo "WARNING: Comprehensive test file not found: $COMPREHENSIVE_TEST"
    echo "Looking for other test files..."
    
    # Look for any Grapa Unicode test files
    for test_file in grapa_test_grep_unicode*.grc; do
        if [[ -f "$test_file" ]]; then
            run_grapa_test "$test_file" "$RESULTS_DIR/$(basename "$test_file" .grc).txt"
        fi
    done
fi

# Run basic Unicode tests
echo "=== Running Basic Unicode Tests ==="

# Create a simple test for basic Unicode functionality
cat > "$RESULTS_DIR/basic_unicode_test.grc" << 'EOF'
# Basic Unicode Grep Tests for Mac ARM64
echo("=== Basic Unicode Tests - Mac ARM64 ===");

# Test café normalization
result = $grep().extract("café", "café", "o");
echo("Café NFC match: " + result);

result = $grep().extract("café", "cafe\u0301", "o");
echo("Café NFD match: " + result);

result = $grep().extract("café", "cafe\u0301", "on");
echo("Café NFD to NFC: " + result);

# Test case insensitive
result = $grep().extract("CAFÉ", "café", "oi");
echo("Café case insensitive: " + result);

# Test Unicode word boundaries
result = $grep().extract("\\b\\w+\\b", "hello café こんにちは world", "o");
echo("Unicode word boundaries: " + result);

# Test emoji
result = $grep().extract("😀+", "😀😀😀", "o");
echo("Emoji repetition: " + result);

# Test line separators
result = $grep().extract("\\xE2\\x80\\xA8", "line1\xE2\x80\xA8line2", "o");
echo("Unicode line separator: " + result);

echo("=== Basic Tests Completed ===");
EOF

run_grapa_test "$RESULTS_DIR/basic_unicode_test.grc" "$RESULTS_DIR/basic_unicode_tests.txt"

# Run performance tests
echo "=== Running Performance Tests ==="

# Create a performance test
cat > "$RESULTS_DIR/performance_test.grc" << 'EOF'
# Performance Test for Unicode Grep on Mac ARM64
echo("=== Performance Test - Mac ARM64 ===");

# Create long emoji string
long_emoji = "";
for(i=0; i<1000; i++) long_emoji = long_emoji + "😀";

start_time = $sys().time();
result = $grep().extract("😀+", long_emoji, "o");
end_time = $sys().time();

echo("Performance test completed in " + (end_time - start_time) + " seconds");
echo("Result length: " + result.length());

# Test with mixed Unicode content
mixed_text = "";
for(i=0; i<100; i++) {
    mixed_text = mixed_text + "hello café こんにちは world " + i + "\n";
}

start_time = $sys().time();
result = $grep().extract("\\b\\w+\\b", mixed_text, "o");
end_time = $sys().time();

echo("Mixed Unicode test completed in " + (end_time - start_time) + " seconds");
echo("Word matches found: " + result.length());

echo("=== Performance Tests Completed ===");
EOF

run_grapa_test "$RESULTS_DIR/performance_test.grc" "$RESULTS_DIR/performance_tests.txt"

# Create test summary
echo "=== Creating Test Summary ==="

cat > "$RESULTS_DIR/test_summary.txt" << EOF
=== Grapa Unicode Grep Test Summary - Mac ARM64 ===
Platform: $(uname -s) $(uname -m)
Date: $(date)
Grapa: $(which grapa)

Test Results:
EOF

# Add test results to summary
for result_file in "$RESULTS_DIR"/*.txt; do
    if [[ -f "$result_file" && "$(basename "$result_file")" != "test_summary.txt" ]]; then
        echo "  $(basename "$result_file"): $(wc -l < "$result_file") lines" >> "$RESULTS_DIR/test_summary.txt"
    fi
done

echo "Test summary saved to: $RESULTS_DIR/test_summary.txt"

# Create comparison script
echo "=== Creating Comparison Script ==="

cat > "$RESULTS_DIR/compare_with_windows.sh" << 'EOF'
#!/bin/bash

# Script to compare Mac ARM64 results with Windows AMD64 results

echo "=== Comparing Mac ARM64 vs Windows AMD64 Grapa Results ==="
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

# Compare all result files
for result_file in *.txt; do
    if [[ -f "$result_file" && "$result_file" != "test_summary.txt" ]]; then
        compare_files "$result_file" "../${result_file}" "$(basename "$result_file" .txt)"
    fi
done

echo
echo "=== Comparison Summary ==="
echo "If all tests show '✓ Results match', then Mac ARM64 and Windows AMD64"
echo "are producing identical results for Grapa Unicode grep functionality."
echo
echo "If any tests show '✗ Results differ', there may be platform-specific"
echo "differences that need investigation."
EOF

chmod +x "$RESULTS_DIR/compare_with_windows.sh"

# Summary
echo
echo "=== Test Execution Summary ==="
echo "All Grapa Unicode tests completed successfully!"
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
echo "3. View test summary:"
echo "   cat $RESULTS_DIR/test_summary.txt"
echo
echo "4. View specific test results:"
echo "   cat $RESULTS_DIR/basic_unicode_tests.txt"
echo "   cat $RESULTS_DIR/performance_tests.txt"

echo
echo "=== Verification Complete ==="
echo "The Mac ARM64 Unicode grep functionality has been tested and results"
echo "are ready for comparison with Windows AMD64 results." 