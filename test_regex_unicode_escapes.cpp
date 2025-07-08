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

void test_unicode_escapes() {
    std::cout << "=== Unicode Escape Processing Test ===\n\n";
    
    // Test Unicode escapes in string literals
    std::string unicode_escape = "\u2028";
    std::string hex_escape = "\xE2\x80\xA8";
    
    std::cout << "Unicode escape \\u2028:" << std::endl;
    print_hex(unicode_escape, "  String literal");
    std::cout << "  Length: " << unicode_escape.length() << " bytes\n";
    
    std::cout << "Hex escape \\xE2\\x80\\xA8:" << std::endl;
    print_hex(hex_escape, "  String literal");
    std::cout << "  Length: " << hex_escape.length() << " bytes\n\n";
    
    // Test if they're equal
    std::cout << "Are they equal? " << (unicode_escape == hex_escape ? "YES" : "NO") << "\n\n";
}

void test_std_regex_unicode() {
    std::cout << "=== std::regex Unicode Test ===\n\n";
    
    std::string input = "a\u2028b\u2029c";
    print_hex(input, "Input string");
    
    // Test std::regex directly
    std::vector<std::string> patterns = {
        "[\u2028\u2029]",      // Unicode escapes in pattern
        "[\\u2028\\u2029]",    // Escaped Unicode escapes
        "[\\xE2\\x80\\xA8\\xE2\\x80\\xA9]", // Hex escapes
        "[\xE2\x80\xA8\xE2\x80\xA9]"        // Direct hex
    };
    
    for (const auto& pattern : patterns) {
        std::cout << "Pattern: " << pattern << std::endl;
        print_hex(pattern, "Pattern bytes");
        
        try {
            std::regex rx(pattern, std::regex::ECMAScript);
            std::smatch match;
            
            if (std::regex_search(input, match, rx)) {
                std::cout << "  std::regex MATCH: '" << match.str() << "'";
                print_hex(match.str(), "    ");
            } else {
                std::cout << "  std::regex NO MATCH" << std::endl;
            }
        } catch (const std::regex_error& e) {
            std::cout << "  std::regex ERROR: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }
}

void test_unicode_regex_implementation() {
    std::cout << "=== UnicodeRegex Implementation Test ===\n\n";
    
    std::string input = "a\u2028b\u2029c";
    print_hex(input, "Input string");
    
    std::vector<std::string> patterns = {
        "[\u2028\u2029]",
        "[\\u2028\\u2029]",
        "[\\xE2\\x80\\xA8\\xE2\\x80\\xA9]",
        "[\xE2\x80\xA8\xE2\x80\xA9]"
    };
    
    for (const auto& pattern : patterns) {
        std::cout << "Pattern: " << pattern << std::endl;
        print_hex(pattern, "Pattern bytes");
        
        try {
            UnicodeRegex regex(pattern, false, NormalizationForm::NFC);
            auto matches = regex.find_all(UnicodeString(input));
            
            std::cout << "  UnicodeRegex matches: " << matches.size() << std::endl;
            for (const auto& match : matches) {
                std::string match_text = input.substr(match.first, match.second);
                std::cout << "    Match at " << match.first << ":" << match.second 
                          << " = '" << match_text << "'";
                print_hex(match_text, "      ");
            }
        } catch (const std::exception& e) {
            std::cout << "  UnicodeRegex ERROR: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }
}

void test_compiler_unicode_processing() {
    std::cout << "=== Compiler Unicode Processing Test ===\n\n";
    
    // Test various Unicode escape forms
    std::vector<std::pair<std::string, std::string>> tests = {
        {"\\u2028", "Unicode escape"},
        {"\\U00002028", "Long Unicode escape"},
        {"\\xE2\\x80\\xA8", "Hex escape"},
        {"\u2028", "Direct Unicode in literal"}
    };
    
    for (const auto& test : tests) {
        std::cout << test.second << ":" << std::endl;
        print_hex(test.first, "  Pattern");
        std::cout << "  Length: " << test.first.length() << " bytes\n\n";
    }
}

void test_platform_info() {
    std::cout << "=== Platform Information ===\n\n";
    
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
    
    std::cout << "sizeof(wchar_t): " << sizeof(wchar_t) << "\n";
    std::cout << "sizeof(char): " << sizeof(char) << "\n";
    std::cout << "sizeof(char16_t): " << sizeof(char16_t) << "\n";
    std::cout << "sizeof(char32_t): " << sizeof(char32_t) << "\n\n";
}

int main() {
    test_unicode_escapes();
    test_std_regex_unicode();
    test_unicode_regex_implementation();
    test_compiler_unicode_processing();
    test_platform_info();
    
    return 0;
} 