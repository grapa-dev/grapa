#include <iostream>
#include <string>
#include <chrono>

// Simple test to check emoji pattern detection
void test_emoji_pattern() {
    std::cout << "Simple Emoji Pattern Test\n";
    std::cout << "=========================\n\n";
    
    std::string pattern = "😀+";
    std::cout << "Pattern: " << pattern << "\n";
    std::cout << "Pattern length: " << pattern.length() << " bytes\n";
    
    // Show hex representation
    std::cout << "Pattern hex: ";
    for (unsigned char c : pattern) {
        printf("%02X ", c);
    }
    std::cout << "\n\n";
    
    // Check if it ends with +
    bool ends_with_plus = (pattern.size() >= 2 && pattern.back() == '+');
    std::cout << "Ends with +: " << (ends_with_plus ? "YES" : "NO") << "\n";
    
    if (ends_with_plus) {
        std::string base_char = pattern.substr(0, pattern.size() - 1);
        std::cout << "Base character: " << base_char << "\n";
        std::cout << "Base character length: " << base_char.length() << " bytes\n";
        
        // Check if it's a single Unicode character
        bool is_single_char = false;
        if (base_char.size() == 1) {
            is_single_char = true;
        } else if (base_char.size() <= 4) {
            // Check UTF-8 validity
            size_t i = 0;
            while (i < base_char.size()) {
                unsigned char c = static_cast<unsigned char>(base_char[i]);
                if ((c & 0x80) == 0) {
                    i += 1;
                } else if ((c & 0xE0) == 0xC0 && i + 1 < base_char.size()) {
                    i += 2;
                } else if ((c & 0xF0) == 0xE0 && i + 2 < base_char.size()) {
                    i += 3;
                } else if ((c & 0xF8) == 0xF0 && i + 3 < base_char.size()) {
                    i += 4;
                } else {
                    break;
                }
            }
            is_single_char = (i == base_char.size());
        }
        
        std::cout << "Is single Unicode character: " << (is_single_char ? "YES" : "NO") << "\n";
    }
    
    // Test with actual emoji string
    std::string emoji_input = "";
    for (int i = 0; i < 1000; ++i) {
        emoji_input += "😀";
    }
    
    std::cout << "\nEmoji input length: " << emoji_input.length() << " bytes\n";
    std::cout << "Input size > 10000: " << (emoji_input.size() > 10000 ? "YES" : "NO") << "\n";
    
    // Test simple string matching
    std::string base_char = "😀";
    std::cout << "\nSimple string matching test:\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Check if entire string consists of the base character
    bool all_same = true;
    for (size_t i = 0; i < emoji_input.size(); i += base_char.size()) {
        if (i + base_char.size() > emoji_input.size() || 
            emoji_input.compare(i, base_char.size(), base_char) != 0) {
            all_same = false;
            break;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "All characters same: " << (all_same ? "YES" : "NO") << "\n";
    std::cout << "Time: " << duration.count() << " microseconds\n";
    
    std::cout << "\nTest completed!\n";
}

int main() {
    test_emoji_pattern();
    return 0;
} 