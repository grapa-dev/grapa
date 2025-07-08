// test_unicode_binary_mode.cpp
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

#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>

using namespace GrapaUnicode;

void print_hex(const std::string& data) {
    std::cout << "Hex: ";
    for (unsigned char byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void test_unicode_binary_mode() {
    std::cout << "=== Unicode Grep with Binary Mode Test ===" << std::endl;
    
    // Test 1: Pure ASCII text - both modes should give same results
    std::cout << "\n1. Pure ASCII text:" << std::endl;
    std::string ascii_text = "Hello World";
    std::cout << "Input: '" << ascii_text << "'" << std::endl;
    
    auto unicode_matches = grep_unicode(ascii_text, "Hello", "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    auto binary_matches = grep_unicode(ascii_text, "Hello", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary mode matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test 2: Text with binary data
    std::cout << "\n2. Text with binary data:" << std::endl;
    std::string mixed_data = "Hello\x00World\xFF\xFE\xFD";
    std::cout << "Input: ";
    print_hex(mixed_data);
    
    unicode_matches = grep_unicode(mixed_data, "Hello", "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    binary_matches = grep_unicode(mixed_data, "Hello", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary mode matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test 3: Unicode text
    std::cout << "\n3. Unicode text:" << std::endl;
    std::string unicode_text = "café résumé naïve";
    std::cout << "Input: '" << unicode_text << "'" << std::endl;
    
    unicode_matches = grep_unicode(unicode_text, "café", "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    binary_matches = grep_unicode(unicode_text, "café", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary mode matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test 4: Binary pattern matching
    std::cout << "\n4. Binary pattern matching:" << std::endl;
    std::string binary_pattern = "\\x00";
    std::cout << "Pattern: " << binary_pattern << std::endl;
    
    unicode_matches = grep_unicode(mixed_data, binary_pattern, "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    binary_matches = grep_unicode(mixed_data, binary_pattern, "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary mode matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test 5: Case insensitive matching
    std::cout << "\n5. Case insensitive matching:" << std::endl;
    std::string case_text = "Hello WORLD";
    std::cout << "Input: '" << case_text << "'" << std::endl;
    
    unicode_matches = grep_unicode(case_text, "hello", "i", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    binary_matches = grep_unicode(case_text, "hello", "i", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode (case insensitive) matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary mode (case insensitive) matches: " << binary_matches.size() << std::endl;
    std::cout << "Result: " << (unicode_matches.size() == binary_matches.size() ? "SAME" : "DIFFERENT") << std::endl;
    
    // Test 6: Extract matches
    std::cout << "\n6. Extract matches:" << std::endl;
    auto extracted_unicode = grep_extract_matches_unicode(mixed_data, "Hello", "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    auto extracted_binary = grep_extract_matches_unicode(mixed_data, "Hello", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode extracted: " << extracted_unicode.size() << std::endl;
    std::cout << "Binary mode extracted: " << extracted_binary.size() << std::endl;
    
    // Test 7: Performance comparison
    std::cout << "\n7. Performance comparison (large ASCII data):" << std::endl;
    std::string large_ascii;
    large_ascii.reserve(10000);
    for (int i = 0; i < 1000; ++i) {
        large_ascii += "This is a test line with some text to search for patterns. ";
    }
    large_ascii += "Target pattern here.";
    
    std::cout << "Large ASCII data size: " << large_ascii.size() << " bytes" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    unicode_matches = grep_unicode(large_ascii, "Target pattern", "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    auto unicode_time = std::chrono::high_resolution_clock::now() - start;
    
    start = std::chrono::high_resolution_clock::now();
    binary_matches = grep_unicode(large_ascii, "Target pattern", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    auto binary_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "Unicode mode time: " << std::chrono::duration_cast<std::chrono::microseconds>(unicode_time).count() << " μs" << std::endl;
    std::cout << "Binary mode time: " << std::chrono::duration_cast<std::chrono::microseconds>(binary_time).count() << " μs" << std::endl;
    std::cout << "Speedup: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(unicode_time).count() / 
                                std::chrono::duration_cast<std::chrono::microseconds>(binary_time).count() << "x" << std::endl;
    
    // Test 8: File header detection
    std::cout << "\n8. File header detection:" << std::endl;
    std::string png_header = "\x89PNG\r\n\x1A\n";
    std::cout << "PNG header: ";
    print_hex(png_header);
    
    unicode_matches = grep_unicode(png_header, "PNG", "", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    binary_matches = grep_unicode(png_header, "PNG", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    
    std::cout << "Unicode mode 'PNG' matches: " << unicode_matches.size() << std::endl;
    std::cout << "Binary mode 'PNG' matches: " << binary_matches.size() << std::endl;
    
    // Test 9: Different normalization forms in binary mode (should be ignored)
    std::cout << "\n9. Normalization forms in binary mode (should be ignored):" << std::endl;
    std::string test_text = "café";
    std::cout << "Input: '" << test_text << "'" << std::endl;
    
    auto nfc_matches = grep_unicode(test_text, "café", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    auto nfd_matches = grep_unicode(test_text, "café", "", "", ProcessingMode::BINARY_MODE, NormalizationForm::NFD);
    
    std::cout << "NFC binary mode matches: " << nfc_matches.size() << std::endl;
    std::cout << "NFD binary mode matches: " << nfd_matches.size() << std::endl;
    std::cout << "Result: " << (nfc_matches.size() == nfd_matches.size() ? "SAME (normalization ignored)" : "DIFFERENT") << std::endl;
}

int main() {
    test_unicode_binary_mode();
    return 0;
} 