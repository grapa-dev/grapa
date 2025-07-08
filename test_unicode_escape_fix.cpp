#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <string>

using namespace GrapaUnicode;

void test_unicode_escape_fix() {
    std::cout << "Unicode Escape Fix Test\n";
    std::cout << "=======================\n\n";
    
    try {
        // Test 1: Basic Unicode escape parsing
        std::cout << "Test 1: Basic Unicode escape parsing\n";
        std::string pattern = "[\u2028\u2029]";
        std::string parsed = parse_unicode_escapes(pattern);
        
        std::cout << "Original pattern: " << pattern << "\n";
        std::cout << "Parsed pattern hex: ";
        for (unsigned char c : parsed) {
            printf("%02X ", c);
        }
        std::cout << "\n\n";
        
        // Test 2: Unicode escape in regex pattern
        std::cout << "Test 2: Unicode escape in regex pattern\n";
        std::string input = "a\u2028b\u2029c";
        std::string pattern2 = "[\u2028\u2029]";
        
        std::cout << "Input hex: ";
        for (unsigned char c : input) {
            printf("%02X ", c);
        }
        std::cout << "\n";
        
        std::cout << "Pattern hex: ";
        for (unsigned char c : pattern2) {
            printf("%02X ", c);
        }
        std::cout << "\n";
        
        UnicodeRegex regex(pattern2, false, NormalizationForm::NFC);
        bool found = regex.search(UnicodeString(input));
        
        std::cout << "Match found: " << (found ? "YES" : "NO") << "\n\n";
        
        // Test 3: Direct grep call
        std::cout << "Test 3: Direct grep call\n";
        auto results = grep_unicode(input, pattern2, "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
        
        std::cout << "Results found: " << results.size() << "\n";
        for (const auto& result : results) {
            std::cout << "  Offset: " << result.offset << ", Length: " << result.length << "\n";
            std::string match = input.substr(result.offset, result.length);
            std::cout << "  Match hex: ";
            for (unsigned char c : match) {
                printf("%02X ", c);
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        
        // Test 4: Compare with hex pattern
        std::cout << "Test 4: Compare with hex pattern\n";
        std::string hex_pattern = "[\xE2\x80\xA8\xE2\x80\xA9]";
        auto hex_results = grep_unicode(input, hex_pattern, "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
        
        std::cout << "Hex pattern results: " << hex_results.size() << "\n";
        std::cout << "Results match: " << (results.size() == hex_results.size() ? "YES" : "NO") << "\n\n";
        
        std::cout << "Unicode escape fix test completed!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in Unicode escape fix test: " << e.what() << std::endl;
        std::cout << "Test failed due to exception\n";
    } catch (...) {
        std::cerr << "Unknown error in Unicode escape fix test" << std::endl;
        std::cout << "Test failed due to unknown error\n";
    }
}

int main() {
    test_unicode_escape_fix();
    return 0;
} 