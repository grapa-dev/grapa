#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

using namespace GrapaUnicode;

void debug_emoji_pattern() {
    std::cout << "Emoji Pattern Debug\n";
    std::cout << "===================\n\n";
    
    std::string pattern = "😀+";
    std::cout << "Pattern: " << pattern << "\n";
    std::cout << "Pattern length: " << pattern.length() << " bytes\n";
    
    // Show hex representation
    std::cout << "Pattern hex: ";
    for (unsigned char c : pattern) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test pattern analysis
    UnicodeRegex regex(pattern, false, NormalizationForm::NFC);
    
    std::cout << "Pattern analysis:\n";
    std::cout << "  Is ASCII only: " << (regex.is_ascii_mode() ? "YES" : "NO") << "\n";
    std::cout << "  Is simple repeated pattern: " << (regex.is_simple_repeated_pattern() ? "YES" : "NO") << "\n";
    
    // Test with small input first
    std::string small_input = "😀😀😀";
    std::cout << "\nSmall input test:\n";
    std::cout << "  Input: " << small_input << "\n";
    std::cout << "  Input length: " << small_input.length() << " bytes\n";
    std::cout << "  Input size > 1000: " << (small_input.size() > 1000 ? "YES" : "NO") << "\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    auto matches = regex.find_all(UnicodeString(small_input));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "  Time: " << duration.count() << " microseconds\n";
    std::cout << "  Matches: " << matches.size() << "\n";
    if (!matches.empty()) {
        std::cout << "  First match: offset=" << matches[0].first << ", length=" << matches[0].second << "\n";
    }
    
    // Test fast path directly
    std::cout << "\nFast path test:\n";
    if (regex.is_simple_repeated_pattern()) {
        start = std::chrono::high_resolution_clock::now();
        auto fast_matches = regex.find_all_fast_path(UnicodeString(small_input));
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "  Fast path time: " << duration.count() << " microseconds\n";
        std::cout << "  Fast path matches: " << fast_matches.size() << "\n";
        if (!fast_matches.empty()) {
            std::cout << "  Fast path first match: offset=" << fast_matches[0].first << ", length=" << fast_matches[0].second << "\n";
        }
    }
    
    // Test with larger input
    std::cout << "\nLarge input test:\n";
    std::string large_input = "";
    for (int i = 0; i < 1000; ++i) {
        large_input += "😀";
    }
    std::cout << "  Input length: " << large_input.length() << " bytes\n";
    std::cout << "  Input size > 1000: " << (large_input.size() > 1000 ? "YES" : "NO") << "\n";
    
    start = std::chrono::high_resolution_clock::now();
    matches = regex.find_all(UnicodeString(large_input));
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Time: " << duration.count() << " milliseconds\n";
    std::cout << "  Matches: " << matches.size() << "\n";
    if (!matches.empty()) {
        std::cout << "  First match: offset=" << matches[0].first << ", length=" << matches[0].second << "\n";
    }
}

int main() {
    debug_emoji_pattern();
    return 0;
} 