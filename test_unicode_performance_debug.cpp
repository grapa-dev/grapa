#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <chrono>
#include <string>

using namespace GrapaUnicode;

void test_performance_bottleneck() {
    std::cout << "Unicode Performance Debug Test\n";
    std::cout << "==============================\n\n";
    
    // Simple test data
    std::string test_input = "café\ncafe\u0301\nhello\nworld";
    std::string pattern = "café";
    
    std::cout << "Test input: " << test_input << "\n";
    std::cout << "Pattern: " << pattern << "\n\n";
    
    // Test 1: Just UnicodeString creation
    std::cout << "Test 1: UnicodeString creation\n";
    auto start = std::chrono::high_resolution_clock::now();
    UnicodeString unicode_str(test_input);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "UnicodeString creation: " << duration.count() << " microseconds\n\n";
    
    // Test 2: Normalization
    std::cout << "Test 2: Normalization\n";
    start = std::chrono::high_resolution_clock::now();
    UnicodeString normalized = unicode_str.normalize(NormalizationForm::NFC);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "NFC normalization: " << duration.count() << " microseconds\n";
    std::cout << "Normalized result: " << normalized.data() << "\n\n";
    
    // Test 3: Case folding
    std::cout << "Test 3: Case folding\n";
    start = std::chrono::high_resolution_clock::now();
    UnicodeString folded = unicode_str.case_fold();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Case folding: " << duration.count() << " microseconds\n";
    std::cout << "Folded result: " << folded.data() << "\n\n";
    
    // Test 4: UnicodeRegex creation
    std::cout << "Test 4: UnicodeRegex creation\n";
    start = std::chrono::high_resolution_clock::now();
    UnicodeRegex regex(pattern, true, NormalizationForm::NFC);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "UnicodeRegex creation: " << duration.count() << " microseconds\n";
    std::cout << "Is ASCII mode: " << (regex.is_ascii_mode() ? "YES" : "NO") << "\n\n";
    
    // Test 5: Simple search
    std::cout << "Test 5: Simple search\n";
    start = std::chrono::high_resolution_clock::now();
    bool found = regex.search(unicode_str);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Simple search: " << duration.count() << " microseconds\n";
    std::cout << "Found: " << (found ? "YES" : "NO") << "\n\n";
    
    // Test 6: Find all matches
    std::cout << "Test 6: Find all matches\n";
    start = std::chrono::high_resolution_clock::now();
    auto matches = regex.find_all(unicode_str);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Find all matches: " << duration.count() << " microseconds\n";
    std::cout << "Number of matches: " << matches.size() << "\n\n";
    
    // Test 7: Direct grep call
    std::cout << "Test 7: Direct grep call\n";
    start = std::chrono::high_resolution_clock::now();
    auto results = grep_unicode(test_input, pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Direct grep call: " << duration.count() << " microseconds\n";
    std::cout << "Number of results: " << results.size() << "\n\n";
    
    // Test 8: ASCII comparison
    std::cout << "Test 8: ASCII comparison\n";
    std::string ascii_input = "hello\nworld\nhello\nworld";
    std::string ascii_pattern = "hello";
    
    start = std::chrono::high_resolution_clock::now();
    auto ascii_results = grep_unicode(ascii_input, ascii_pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "ASCII grep call: " << duration.count() << " microseconds\n";
    std::cout << "Number of results: " << ascii_results.size() << "\n\n";
    
    // Test 9: Binary mode comparison
    std::cout << "Test 9: Binary mode comparison\n";
    start = std::chrono::high_resolution_clock::now();
    auto binary_results = grep_unicode(test_input, pattern, "i", "\n", ProcessingMode::BINARY_MODE, NormalizationForm::NFC);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Binary mode grep call: " << duration.count() << " microseconds\n";
    std::cout << "Number of results: " << binary_results.size() << "\n\n";
    
    std::cout << "Performance debug test completed!\n";
}

int main() {
    test_performance_bottleneck();
    return 0;
} 