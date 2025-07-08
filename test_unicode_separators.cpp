#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
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

void test_unicode_separators() {
    std::cout << "=== Unicode Separators Test ===\n\n";
    
    // Test input with line and paragraph separators
    std::string input = "a\u2028b\u2029c";
    
    std::cout << "Input string: " << input << std::endl;
    print_hex(input, "Input");
    std::cout << "Input length: " << input.length() << " bytes\n\n";
    
    // Test individual characters
    std::string lsep = "\u2028";  // Line separator
    std::string psep = "\u2029";  // Paragraph separator
    
    print_hex(lsep, "Line separator (U+2028)");
    print_hex(psep, "Paragraph separator (U+2029)");
    
    std::cout << "\nLine separator length: " << lsep.length() << " bytes\n";
    std::cout << "Paragraph separator length: " << psep.length() << " bytes\n\n";
    
    // Test regex patterns
    std::vector<std::string> patterns = {
        "[\u2028\u2029]",
        "[\u2028]",
        "[\u2029]",
        "\\u2028",
        "\\u2029"
    };
    
    for (const auto& pattern : patterns) {
        std::cout << "Testing pattern: " << pattern << std::endl;
        
        try {
            UnicodeRegex regex(pattern, false, NormalizationForm::NFC);
            auto matches = regex.find_all(UnicodeString(input));
            
            std::cout << "Matches found: " << matches.size() << std::endl;
            for (const auto& match : matches) {
                std::string match_text = input.substr(match.first, match.second);
                std::cout << "  Match at " << match.first << ":" << match.second 
                          << " = '" << match_text << "'";
                print_hex(match_text, "    ");
            }
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
    
    // Test with grep function
    std::cout << "Testing with grep function:\n";
    try {
        auto matches = grep_extract_matches_unicode(input, "[\u2028\u2029]", "ou", "");
        std::cout << "Grep matches: " << matches.size() << std::endl;
        for (const auto& match : matches) {
            std::cout << "  '" << match << "'";
            print_hex(match, "    ");
        }
    } catch (const std::exception& e) {
        std::cout << "Grep error: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== Platform Info ===\n";
#ifdef _WIN32
    std::cout << "Platform: Windows\n";
#elif defined(__APPLE__)
    std::cout << "Platform: macOS\n";
    #ifdef __aarch64__
    std::cout << "Architecture: ARM64\n";
    #else
    std::cout << "Architecture: x86_64\n";
    #endif
#elif defined(__linux__)
    std::cout << "Platform: Linux\n";
#endif
    
    std::cout << "sizeof(wchar_t): " << sizeof(wchar_t) << std::endl;
    std::cout << "sizeof(char): " << sizeof(char) << std::endl;
}

int main() {
    test_unicode_separators();
    return 0;
} 