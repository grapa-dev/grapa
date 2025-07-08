#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <chrono>
#include <string>

using namespace GrapaUnicode;

void test_grep_fast_path() {
    std::cout << "Grep Fast Path Test\n";
    std::cout << "===================\n\n";
    
    // Create the exact same test case as the Grapa test
    std::string input = "";
    for (int i = 0; i < 10000; ++i) {
        input += "😀";
    }
    
    std::string pattern = "😀+";
    
    std::cout << "Input length: " << input.length() << " bytes\n";
    std::cout << "Pattern: " << pattern << "\n\n";
    
    // Test the grep function
    std::cout << "Testing grep_unicode with fast path:\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    auto results = grep_unicode(input, pattern, "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Time: " << duration.count() << " milliseconds\n";
    std::cout << "Results found: " << results.size() << "\n";
    
    if (!results.empty()) {
        std::cout << "First result: offset=" << results[0].offset << ", length=" << results[0].length << "\n";
        std::cout << "Expected: offset=0, length=" << input.size() << "\n";
        std::cout << "Match correct: " << (results[0].offset == 0 && results[0].length == input.size() ? "YES" : "NO") << "\n";
    }
    
    // Test extract matches function
    std::cout << "\nTesting grep_extract_matches_unicode:\n";
    start = std::chrono::high_resolution_clock::now();
    
    auto extracted = grep_extract_matches_unicode(input, pattern, "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Time: " << duration.count() << " milliseconds\n";
    std::cout << "Extracted matches: " << extracted.size() << "\n";
    
    if (!extracted.empty()) {
        std::cout << "First extracted: length=" << extracted[0].length() << "\n";
        std::cout << "Expected: length=" << input.size() << "\n";
        std::cout << "Extract correct: " << (extracted[0].length() == input.size() ? "YES" : "NO") << "\n";
    }
    
    std::cout << "\nFast path test completed!\n";
    std::cout << "Expected performance: < 1 second (instead of 3+ minutes)\n";
}

int main() {
    test_grep_fast_path();
    return 0;
} 