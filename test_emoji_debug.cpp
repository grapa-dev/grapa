#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <string>

using namespace GrapaUnicode;

void test_minimal_emoji() {
    std::cout << "Minimal Emoji Debug Test\n";
    std::cout << "========================\n\n";
    
    try {
        // Test 1: Basic Unicode string creation
        std::cout << "Test 1: Basic Unicode string creation\n";
        UnicodeString test_str("Hello");
        std::cout << "String created successfully\n";
        std::cout << "Size: " << test_str.size() << " bytes\n\n";
        
        // Test 2: Simple emoji string
        std::cout << "Test 2: Simple emoji string\n";
        UnicodeString emoji_str("😀");
        std::cout << "Emoji string created successfully\n";
        std::cout << "Size: " << emoji_str.size() << " bytes\n\n";
        
        // Test 3: Unicode normalization
        std::cout << "Test 3: Unicode normalization\n";
        UnicodeString normalized = emoji_str.normalize(NormalizationForm::NFC);
        std::cout << "Normalization successful\n";
        std::cout << "Normalized size: " << normalized.size() << " bytes\n\n";
        
        // Test 4: Simple regex creation
        std::cout << "Test 4: Simple regex creation\n";
        UnicodeRegex regex("a", false, NormalizationForm::NFC);
        std::cout << "Regex created successfully\n\n";
        
        // Test 5: Emoji regex creation
        std::cout << "Test 5: Emoji regex creation\n";
        UnicodeRegex emoji_regex("😀", false, NormalizationForm::NFC);
        std::cout << "Emoji regex created successfully\n\n";
        
        // Test 6: Simple search
        std::cout << "Test 6: Simple search\n";
        bool found = regex.search(UnicodeString("abc"));
        std::cout << "Search result: " << (found ? "found" : "not found") << "\n\n";
        
        // Test 7: Emoji search
        std::cout << "Test 7: Emoji search\n";
        bool emoji_found = emoji_regex.search(emoji_str);
        std::cout << "Emoji search result: " << (emoji_found ? "found" : "not found") << "\n\n";
        
        // Test 8: Small repeated emoji
        std::cout << "Test 8: Small repeated emoji\n";
        std::string small_emoji = "😀😀😀";
        UnicodeString small_emoji_unicode(small_emoji);
        std::cout << "Small emoji string created: " << small_emoji_unicode.size() << " bytes\n\n";
        
        // Test 9: Emoji pattern matching
        std::cout << "Test 9: Emoji pattern matching\n";
        UnicodeRegex emoji_pattern("😀+", false, NormalizationForm::NFC);
        bool pattern_found = emoji_pattern.search(small_emoji_unicode);
        std::cout << "Pattern match result: " << (pattern_found ? "found" : "not found") << "\n\n";
        
        // Test 10: Fast path detection
        std::cout << "Test 10: Fast path detection\n";
        bool is_simple = emoji_pattern.is_simple_repeated_pattern();
        std::cout << "Is simple repeated pattern: " << (is_simple ? "YES" : "NO") << "\n\n";
        
        std::cout << "All tests completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in minimal emoji test: " << e.what() << std::endl;
        std::cout << "Test failed due to exception\n";
    } catch (...) {
        std::cerr << "Unknown error in minimal emoji test" << std::endl;
        std::cout << "Test failed due to unknown error\n";
    }
}

int main() {
    test_minimal_emoji();
    return 0;
} 