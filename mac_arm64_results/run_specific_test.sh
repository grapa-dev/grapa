#!/bin/bash

# Script to run a specific Unicode grep test

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <test_file> [options]"
    echo "Examples:"
    echo "  $0 test_unicode_grep.cpp"
    echo "  $0 unicode_grep_comprehensive_tests.grc"
    exit 1
fi

test_file="$1"
options="${2:-}"

echo "=== Running Specific Unicode Grep Test ==="
echo "Test file: $test_file"
echo

if [[ "$test_file" == *.cpp ]]; then
    echo "Building and running C++ test..."
    clang++ -std=c++17 -O3 -Isource -Isource/grep -Isource/utf8proc \
        -DUTF8PROC_STATIC \
        "$test_file" \
        source/grep/grapa_grep_unicode.cpp \
        source/utf8proc/utf8proc.c \
        -framework CoreFoundation -framework AppKit -framework IOKit \
        -o test_program
    
    echo "Running test..."
    ./test_program $options
    rm -f test_program
elif [[ "$test_file" == *.grc ]]; then
    if [[ "$GRAPA_AVAILABLE" == "true" ]]; then
        echo "Running Grapa test..."
        $GRAPA_CMD -cfile "$test_file" $options
    else
        echo "ERROR: grapa not found in PATH or current directory"
        exit 1
    fi
else
    echo "ERROR: Unsupported file type. Use .cpp or .grc files."
    exit 1
fi
