#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <regex>
#include "source/grep/grapa_grep_unicode.hpp"

using namespace GrapaUnicode;

void print_hex(const std::string& str, const std::string& label) {
    std::cout << label << " (hex): ";
    for (unsigned char c : str) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << std::endl;
}

void test_regex_pattern_processing() {
    std::cout << "=== Regex Pattern Processing Test ===\n\n";
    
    // Test input with known Unicode separators
    std::string input = "a\u2028b\u2029c";
    print_hex(input, "Input string");
    std::cout << "Input length: " << input.length() << " bytes\n\n";
    
    // Test different pattern forms
    std::vector<std::pair<std::string, std::string>> patterns = {
        {"[\u2028\u2029]", "Unicode escapes in character class"},
        {"[\\u2028\\u2029]", "Escaped Unicode escapes in character class"},
        {"[\xE2\x80\xA8\xE2\x80\xA9]", "Hex escapes in character class"},
        {"[\\xE2\\x80\\xA8\\xE2\\x80\\xA9]", "Escaped hex escapes in character class"},
        {"\u2028", "Single Unicode escape"},
        {"\\u2028", "Escaped single Unicode escape"},
        {"\xE2\x80\xA8", "Single hex escape"},
        {"\\xE2\\x80\\xA8", "Escaped single hex escape"}
    };
    
    for (const auto& pattern : patterns) {
        std::cout << "Pattern: " << pattern.first << " (" << pattern.second << ")\n";
        print_hex(pattern.first, "Pattern bytes");
        
        // Test std::regex directly
        try {
            std::regex rx(pattern.first, std::regex::ECMAScript);
            std::smatch match;
            
            if (std::regex_search(input, match, rx)) {
                std::cout << "  std::regex: MATCH '" << match.str() << "'";
                print_hex(match.str(), "    ");
            } else {
                std::cout << "  std::regex: NO MATCH\n";
            }
        } catch (const std::regex_error& e) {
            std::cout << "  std::regex: ERROR " << e.what() << "\n";
        }
        
        // Test UnicodeRegex
        try {
            UnicodeRegex unicode_rx(pattern.first, false, NormalizationForm::NFC);
            auto matches = unicode_rx.find_all(UnicodeString(input));
            
            std::cout << "  UnicodeRegex: " << matches.size() << " matches\n";
            for (const auto& match : matches) {
                std::string match_text = input.substr(match.first, match.second);
                std::cout << "    Match at " << match.first << ":" << match.second 
                          << " = '" << match_text << "'";
                print_hex(match_text, "      ");
            }
        } catch (const std::exception& e) {
            std::cout << "  UnicodeRegex: ERROR " << e.what() << "\n";
        }
        
        std::cout << std::endl;
    }
}

void test_character_class_behavior() {
    std::cout << "=== Character Class Behavior Test ===\n\n";
    
    std::string input = "a\u2028b\u2029c";
    
    // Test individual characters in character classes
    std::vector<std::string> single_patterns = {
        "[\u2028]",      // Single Unicode escape
        "[\\u2028]",     // Escaped Unicode escape
        "[\xE2\x80\xA8]", // Single hex escape
        "[\\xE2\\x80\\xA8]" // Escaped hex escape
    };
    
    for (const auto& pattern : single_patterns) {
        std::cout << "Pattern: " << pattern << std::endl;
        print_hex(pattern, "Pattern bytes");
        
        try {
            std::regex rx(pattern, std::regex::ECMAScript);
            std::smatch match;
            
            if (std::regex_search(input, match, rx)) {
                std::cout << "  MATCH: '" << match.str() << "'";
                print_hex(match.str(), "    ");
            } else {
                std::cout << "  NO MATCH\n";
            }
        } catch (const std::regex_error& e) {
            std::cout << "  ERROR: " << e.what() << "\n";
        }
        std::cout << std::endl;
    }
}

void test_platform_specific_regex() {
    std::cout << "=== Platform-Specific Regex Test ===\n\n";
    
    std::string input = "a\u2028b\u2029c";
    
    // Test with different regex flags
    std::vector<std::pair<std::regex::flag_type, std::string>> flag_tests = {
        {std::regex::ECMAScript, "ECMAScript"},
        {std::regex::basic, "Basic"},
        {std::regex::extended, "Extended"},
        {std::regex::awk, "AWK"},
        {std::regex::grep, "Grep"},
        {std::regex::egrep, "EGrep"}
    };
    
    std::string pattern = "[\u2028\u2029]";
    
    for (const auto& flag_test : flag_tests) {
        std::cout << "Regex flavor: " << flag_test.second << std::endl;
        
        try {
            std::regex rx(pattern, flag_test.first);
            std::smatch match;
            
            if (std::regex_search(input, match, rx)) {
                std::cout << "  MATCH: '" << match.str() << "'";
                print_hex(match.str(), "    ");
            } else {
                std::cout << "  NO MATCH\n";
            }
        } catch (const std::regex_error& e) {
            std::cout << "  ERROR: " << e.what() << "\n";
        }
        std::cout << std::endl;
    }
}

void test_compiler_info() {
    std::cout << "=== Compiler and Platform Info ===\n\n";
    
#ifdef _WIN32
    std::cout << "Platform: Windows\n";
    #ifdef _MSC_VER
    std::cout << "Compiler: MSVC " << _MSC_VER << "\n";
    #endif
#elif defined(__APPLE__)
    std::cout << "Platform: macOS\n";
    #ifdef __aarch64__
    std::cout << "Architecture: ARM64\n";
    #else
    std::cout << "Architecture: x86_64\n";
    #endif
    #ifdef __clang__
    std::cout << "Compiler: Clang " << __clang_major__ << "." << __clang_minor__ << "\n";
    #endif
#elif defined(__linux__)
    std::cout << "Platform: Linux\n";
    #ifdef __GNUC__
    std::cout << "Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "\n";
    #endif
#endif
    
    std::cout << "C++ Standard: " << __cplusplus << "\n";
    std::cout << "sizeof(wchar_t): " << sizeof(wchar_t) << "\n";
    std::cout << "sizeof(char): " << sizeof(char) << "\n\n";
}

int main() {
    test_regex_pattern_processing();
    test_character_class_behavior();
    test_platform_specific_regex();
    test_compiler_info();
    
    return 0;
} 