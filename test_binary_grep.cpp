// test_binary_grep.cpp
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

#include "source/grep/grapa_grep_binary.hpp"
#include <iostream>
#include <iomanip>
#include <string>

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

void test_binary_grep() {
    std::cout << "=== Binary Grep Test ===" << std::endl;
    
    // Test 1: Simple binary data with text pattern
    std::cout << "\n1. Simple binary data with text pattern:" << std::endl;
    std::string binary_data = "Hello\x00World\xFF\xFE\xFD";
    std::cout << "Input data: ";
    print_hex(binary_data);
    print_escaped(binary_data);
    
    auto matches = grep_binary(binary_data, "Hello", "", "");
    std::cout << "Matches for 'Hello': " << matches.size() << std::endl;
    for (const auto& match : matches) {
        std::string match_data = binary_data.substr(match.offset, match.length);
        std::cout << "  Offset: " << match.offset << ", Length: " << match.length 
                  << ", Data: ";
        print_escaped(match_data);
    }
    
    // Test 2: Hex pattern matching
    std::cout << "\n2. Hex pattern matching:" << std::endl;
    std::string hex_pattern = "48 65 6C 6C 6F"; // "Hello"
    std::cout << "Hex pattern: " << hex_pattern << std::endl;
    
    auto hex_matches = grep_binary_hex(binary_data, hex_pattern, "", "");
    std::cout << "Hex pattern matches: " << hex_matches.size() << std::endl;
    for (const auto& match : hex_matches) {
        std::string match_data = binary_data.substr(match.offset, match.length);
        std::cout << "  Offset: " << match.offset << ", Length: " << match.length 
                  << ", Data: ";
        print_escaped(match_data);
    }
    
    // Test 3: Binary pattern with null bytes
    std::cout << "\n3. Binary pattern with null bytes:" << std::endl;
    std::string null_pattern = "\\x00";
    std::cout << "Pattern: " << null_pattern << std::endl;
    
    auto null_matches = grep_binary(binary_data, null_pattern, "", "");
    std::cout << "Null byte matches: " << null_matches.size() << std::endl;
    for (const auto& match : null_matches) {
        std::string match_data = binary_data.substr(match.offset, match.length);
        std::cout << "  Offset: " << match.offset << ", Length: " << match.length 
                  << ", Data: ";
        print_escaped(match_data);
    }
    
    // Test 4: Hex pattern with null bytes
    std::cout << "\n4. Hex pattern with null bytes:" << std::endl;
    std::string hex_null_pattern = "00";
    std::cout << "Hex null pattern: " << hex_null_pattern << std::endl;
    
    auto hex_null_matches = grep_binary_hex(binary_data, hex_null_pattern, "", "");
    std::cout << "Hex null matches: " << hex_null_matches.size() << std::endl;
    for (const auto& match : hex_null_matches) {
        std::string match_data = binary_data.substr(match.offset, match.length);
        std::cout << "  Offset: " << match.offset << ", Length: " << match.length 
                  << ", Data: ";
        print_escaped(match_data);
    }
    
    // Test 5: Complex binary data
    std::cout << "\n5. Complex binary data:" << std::endl;
    std::string complex_data = "PNG\x89\x50\x4E\x47\x0D\x0A\x1A\x0A";
    std::cout << "PNG header data: ";
    print_hex(complex_data);
    print_escaped(complex_data);
    
    auto png_matches = grep_binary(complex_data, "PNG", "", "");
    std::cout << "PNG text matches: " << png_matches.size() << std::endl;
    
    auto png_hex_matches = grep_binary_hex(complex_data, "50 4E 47", "", "");
    std::cout << "PNG hex matches: " << png_hex_matches.size() << std::endl;
    
    // Test 6: Extract matches
    std::cout << "\n6. Extract matches:" << std::endl;
    auto extracted = grep_extract_matches_binary(binary_data, "Hello", "", "");
    std::cout << "Extracted matches: " << extracted.size() << std::endl;
    for (const auto& match : extracted) {
        std::cout << "  ";
        print_escaped(match);
    }
    
    auto extracted_hex = grep_extract_matches_binary_hex(binary_data, "48 65 6C 6C 6F", "", "");
    std::cout << "Extracted hex matches: " << extracted_hex.size() << std::endl;
    for (const auto& match : extracted_hex) {
        std::cout << "  ";
        print_escaped(match);
    }
    
    // Test 7: Binary data with line breaks
    std::cout << "\n7. Binary data with line breaks:" << std::endl;
    std::string line_data = "Line1\x0A\x0DLine2\x0A\x0DLine3";
    std::cout << "Line data: ";
    print_hex(line_data);
    print_escaped(line_data);
    
    auto line_matches = grep_binary(line_data, "Line", "", "");
    std::cout << "Line matches: " << line_matches.size() << std::endl;
    for (const auto& match : line_matches) {
        std::string match_data = line_data.substr(match.offset, match.length);
        std::cout << "  Offset: " << match.offset << ", Length: " << match.length 
                  << ", Data: ";
        print_escaped(match_data);
    }
    
    // Test 8: Utility functions
    std::cout << "\n8. Utility functions:" << std::endl;
    std::string test_hex = "48 65 6C 6C 6F";
    std::string converted = hex_to_binary(test_hex);
    std::cout << "Hex '" << test_hex << "' -> Binary: ";
    print_escaped(converted);
    
    std::string back_to_hex = binary_to_hex(converted);
    std::cout << "Binary -> Hex: '" << back_to_hex << "'" << std::endl;
    
    std::cout << "Is printable ASCII: " << (is_printable_ascii(converted) ? "Yes" : "No") << std::endl;
    std::cout << "Is printable ASCII (with nulls): " << (is_printable_ascii(binary_data) ? "Yes" : "No") << std::endl;
}

int main() {
    test_binary_grep();
    return 0;
} 