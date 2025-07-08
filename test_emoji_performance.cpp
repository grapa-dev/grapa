#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <chrono>
#include <string>

using namespace GrapaUnicode;

void test_emoji_performance() {
    std::cout << "Emoji Performance Test\n";
    std::cout << "======================\n\n";
    
    // Create a long string with repeated emoji (similar to the Grapa test)
    std::string input = "";
    for (int i = 0; i < 10000; ++i) {
        input += "😀";
    }
    
    std::string pattern = "😀+";
    
    std::cout << "Input length: " << input.length() << " bytes\n";
    std::cout << "Pattern: " << pattern << "\n\n";
    
    // Test 1: Original pattern (should be slow)
    std::cout << "Test 1: Original pattern (😀+)\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    UnicodeRegex regex1(pattern, false, NormalizationForm::NFC);
    auto matches1 = regex1.find_all(UnicodeString(input));
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Time: " << duration1.count() << " milliseconds\n";
    std::cout << "Matches found: " << matches1.size() << "\n";
    if (!matches1.empty()) {
        std::cout << "First match: offset=" << matches1[0].first << ", length=" << matches1[0].second << "\n";
    }
    std::cout << "\n";
    
    // Test 2: Optimized pattern (should be fast)
    std::cout << "Test 2: Optimized pattern (😀++)\n";
    start = std::chrono::high_resolution_clock::now();
    
    UnicodeRegex regex2("😀++", false, NormalizationForm::NFC);
    auto matches2 = regex2.find_all(UnicodeString(input));
    
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Time: " << duration2.count() << " milliseconds\n";
    std::cout << "Matches found: " << matches2.size() << "\n";
    if (!matches2.empty()) {
        std::cout << "First match: offset=" << matches2[0].first << ", length=" << matches2[0].second << "\n";
    }
    std::cout << "\n";
    
    // Test 3: Fast path detection
    std::cout << "Test 3: Fast path detection\n";
    std::cout << "Is simple repeated pattern (😀+): " << (regex1.is_simple_repeated_pattern() ? "YES" : "NO") << "\n";
    std::cout << "Is simple repeated pattern (😀++): " << (regex2.is_simple_repeated_pattern() ? "YES" : "NO") << "\n";
    std::cout << "Input size > 1000: " << (input.size() > 1000 ? "YES" : "NO") << "\n\n";
    
    // Test 4: Direct grep call
    std::cout << "Test 4: Direct grep call\n";
    start = std::chrono::high_resolution_clock::now();
    
    auto results = grep_unicode(input, pattern, "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    
    end = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Time: " << duration3.count() << " milliseconds\n";
    std::cout << "Results found: " << results.size() << "\n";
    if (!results.empty()) {
        std::cout << "First result: offset=" << results[0].offset << ", length=" << results[0].length << "\n";
    }
    std::cout << "\n";
    
    // Test 5: Performance comparison
    std::cout << "Test 5: Performance comparison\n";
    if (duration1.count() > 0 && duration2.count() > 0) {
        double speedup = static_cast<double>(duration1.count()) / duration2.count();
        std::cout << "Speedup: " << speedup << "x\n";
        std::cout << "Optimization working: " << (speedup > 1.5 ? "YES" : "NO") << "\n";
    }
    
    std::cout << "\nEmoji performance test completed!\n";
}

int main() {
    test_emoji_performance();
    return 0;
} 