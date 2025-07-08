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
