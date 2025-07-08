// test_grep_comparison.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "source/grep/grapa_grep.hpp"
#include "source/grep/grapa_grep_unicode.hpp"
#include "source/grep/grapa_grep_binary.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>

using namespace BinaryUtils;

void print_hex(const std::string& data) {
    std::cout << "Hex: ";
    for (unsigned char byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void print_escaped(const std::string& data) {
    std::cout << "Escaped: " << escape_binary(data) << std::endl;
}

void compare_grep_methods() {
    std::cout << "=== Unicode vs Binary Grep Comparison ===" << std::endl;
    
    // Test case 1: Pure ASCII text
    std::cout << "\n1. Pure ASCII text:" << std::endl;
    std::string ascii_text = "Hello World";
    std::cout << "Input: '" << ascii_text << "'" << std::endl;
    
    auto unicode_matches = grep_unicode(ascii_text, "Hello", "", "");
    auto binary_matches = grep_binary(ascii_text, "Hello", "", "");
    
    std::cout << "Unicode grep matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary grep matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test case 2: Text with binary data
    std::cout << "\n2. Text with binary data:" << std::endl;
    std::string mixed_data = "Hello\x00World\xFF\xFE\xFD";
    std::cout << "Input: ";
    print_escaped(mixed_data);
    print_hex(mixed_data);
    
    unicode_matches = grep_unicode(mixed_data, "Hello", "", "");
    binary_matches = grep_binary(mixed_data, "Hello", "", "");
    
    std::cout << "Unicode grep matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary grep matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test case 3: Unicode text
    std::cout << "\n3. Unicode text:" << std::endl;
    std::string unicode_text = "café résumé naïve";
    std::cout << "Input: '" << unicode_text << "'" << std::endl;
    
    unicode_matches = grep_unicode(unicode_text, "café", "", "");
    binary_matches = grep_binary(unicode_text, "café", "", "");
    
    std::cout << "Unicode grep matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary grep matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test case 4: Binary pattern matching
    std::cout << "\n4. Binary pattern matching:" << std::endl;
    std::string binary_pattern = "\\x00";
    std::cout << "Pattern: " << binary_pattern << std::endl;
    
    unicode_matches = grep_unicode(mixed_data, binary_pattern, "", "");
    binary_matches = grep_binary(mixed_data, binary_pattern, "", "");
    
    std::cout << "Unicode grep matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary grep matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test case 5: Hex pattern matching
    std::cout << "\n5. Hex pattern matching:" << std::endl;
    std::string hex_pattern = "48 65 6C 6C 6F"; // "Hello"
    std::cout << "Hex pattern: " << hex_pattern << std::endl;
    
    auto hex_matches = grep_binary_hex(mixed_data, hex_pattern, "", "");
    std::cout << "Binary hex grep matches: " << hex_matches.size() << std::endl;
    
    // Test case 6: Case insensitive matching
    std::cout << "\n6. Case insensitive matching:" << std::endl;
    std::string case_text = "Hello WORLD";
    std::cout << "Input: '" << case_text << "'" << std::endl;
    
    unicode_matches = grep_unicode(case_text, "hello", "i", "");
    binary_matches = grep_binary(case_text, "hello", "i", "");
    
    std::cout << "Unicode grep (case insensitive) matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary grep (case insensitive) matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test case 7: File header detection
    std::cout << "\n7. File header detection:" << std::endl;
    std::string png_header = "\x89PNG\r\n\x1A\n";
    std::cout << "PNG header: ";
    print_hex(png_header);
    print_escaped(png_header);
    
    unicode_matches = grep_unicode(png_header, "PNG", "", "");
    binary_matches = grep_binary(png_header, "PNG", "", "");
    
    std::cout << "Unicode grep 'PNG' matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary grep 'PNG' matches: " << binary_matches.size() << std::endl;
    
    auto png_hex_matches = grep_binary_hex(png_header, "89 50 4E 47", "", "");
    std::cout << "Binary hex grep PNG header matches: " << png_hex_matches.size() << std::endl;
    
    // Test case 8: Performance comparison
    std::cout << "\n8. Performance comparison (large ASCII data):" << std::endl;
    std::string large_ascii;
    large_ascii.reserve(10000);
    for (int i = 0; i < 1000; ++i) {
        large_ascii += "This is a test line with some text to search for patterns. ";
    }
    large_ascii += "Target pattern here.";
    
    std::cout << "Large ASCII data size: " << large_ascii.size() << " bytes" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    unicode_matches = grep_unicode(large_ascii, "Target pattern", "", "");
    auto unicode_time = std::chrono::high_resolution_clock::now() - start;
    
    start = std::chrono::high_resolution_clock::now();
    binary_matches = grep_binary(large_ascii, "Target pattern", "", "");
    auto binary_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "Unicode grep time: " << std::chrono::duration_cast<std::chrono::microseconds>(unicode_time).count() << " μs" << std::endl;
    std::cout << "Binary grep time: " << std::chrono::duration_cast<std::chrono::microseconds>(binary_time).count() << " μs" << std::endl;
    std::cout << "Speedup: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(unicode_time).count() / 
                                std::chrono::duration_cast<std::chrono::microseconds>(binary_time).count() << "x" << std::endl;
}

int main() {
    compare_grep_methods();
    return 0;
} 