#include <iostream>
#include <string>
#include <vector>
#include "source/grep/grapa_grep_unicode.hpp"

using namespace GrapaUnicode;

void test_unicode_properties() {
    std::cout << "=== Unicode Property Escapes Test ===\n\n";
    
    // Test data with mixed scripts
    std::string input = "Hello 世界 Привет مرحبا 123 ٤٥٦ १२३ １２３ .!?";
    
    std::cout << "Input: " << input << "\n\n";
    
    // Test Unicode property escapes
    std::vector<std::pair<std::string, std::string>> tests = {
        {"\\p{L}+", "Unicode letters (any script)"},
        {"\\p{N}+", "Unicode numbers (any script)"},
        {"\\p{P}+", "Unicode punctuation"},
        {"\\p{Z}+", "Unicode whitespace"},
        {"\\p{Latin}+", "Latin script letters"},
        {"\\p{Han}+", "Han (Chinese) characters"},
        {"\\p{Cyrillic}+", "Cyrillic script letters"},
        {"\\p{Arabic}+", "Arabic script letters"},
        {"\\p{Lu}+", "Uppercase letters"},
        {"\\p{Ll}+", "Lowercase letters"}
    };
    
    for (const auto& test : tests) {
        std::cout << "Pattern: " << test.first << " (" << test.second << ")\n";
        
        try {
            UnicodeRegex regex(test.first, false, NormalizationForm::NFC);
            auto matches = regex.find_all(UnicodeString(input));
            
            std::cout << "Matches: ";
            for (const auto& match : matches) {
                std::string match_text = input.substr(match.first, match.second);
                std::cout << "'" << match_text << "' ";
            }
            std::cout << "\n\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n\n";
        }
    }
}

void test_std_regex_fallback() {
    std::cout << "=== Standard Regex Fallback Test ===\n\n";
    
    std::string input = "Hello world 123 .!?";
    
    std::cout << "Input: " << input << "\n\n";
    
    // Test standard regex patterns (should work without PCRE)
    std::vector<std::string> patterns = {
        "\\w+",
        "\\d+", 
        "[.!?]+",
        "\\s+"
    };
    
    for (const auto& pattern : patterns) {
        std::cout << "Pattern: " << pattern << "\n";
        
        try {
            UnicodeRegex regex(pattern, false, NormalizationForm::NFC);
            auto matches = regex.find_all(UnicodeString(input));
            
            std::cout << "Matches: ";
            for (const auto& match : matches) {
                std::string match_text = input.substr(match.first, match.second);
                std::cout << "'" << match_text << "' ";
            }
            std::cout << "\n\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n\n";
        }
    }
}

int main() {
    std::cout << "Unicode Property Escapes Test with PCRE\n";
    std::cout << "=======================================\n\n";
    
#ifdef USE_PCRE
    std::cout << "PCRE support: ENABLED\n\n";
    test_unicode_properties();
#else
    std::cout << "PCRE support: DISABLED (using std::regex fallback)\n\n";
    test_std_regex_fallback();
#endif
    
    return 0;
} 