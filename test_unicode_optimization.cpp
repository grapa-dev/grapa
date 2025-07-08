#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <chrono>
#include <string>

using namespace GrapaUnicode;

void test_optimization() {
    std::cout << "Testing Unicode normalization optimizations...\n\n";
    
    // Test data with Unicode characters that need normalization
    std::string test_input = "café\ncafe\u0301\ncafé\ncafe\u0301\ncafé\ncafe\u0301\ncafé\ncafe\u0301\ncafé\ncafe\u0301";
    std::string pattern = "café";
    
    std::cout << "Input text (with mixed NFC/NFD forms):\n";
    std::cout << test_input << "\n\n";
    
    std::cout << "Pattern: " << pattern << "\n\n";
    
    // Test 1: Basic functionality
    std::cout << "Test 1: Basic Unicode matching (NFC normalization)\n";
    auto start = std::chrono::high_resolution_clock::now();
    auto results1 = grep_unicode(test_input, pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Found " << results1.size() << " matches in " << duration1.count() << " microseconds\n";
    for (const auto& match : results1) {
        std::cout << "  Line " << match.line_number << ": offset=" << match.offset << ", length=" << match.length << "\n";
    }
    std::cout << "\n";
    
    // Test 2: NFD normalization
    std::cout << "Test 2: NFD normalization\n";
    start = std::chrono::high_resolution_clock::now();
    auto results2 = grep_unicode(test_input, pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFD);
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Found " << results2.size() << " matches in " << duration2.count() << " microseconds\n";
    for (const auto& match : results2) {
        std::cout << "  Line " << match.line_number << ": offset=" << match.offset << ", length=" << match.length << "\n";
    }
    std::cout << "\n";
    
    // Test 3: Performance comparison - repeated searches
    std::cout << "Test 3: Performance test - repeated searches (should benefit from caching)\n";
    
    // First run (cache miss)
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) {
        grep_unicode(test_input, pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    }
    end = std::chrono::high_resolution_clock::now();
    auto duration3a = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Second run (cache hit)
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) {
        grep_unicode(test_input, pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    }
    end = std::chrono::high_resolution_clock::now();
    auto duration3b = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "First run (cache miss): " << duration3a.count() << " microseconds\n";
    std::cout << "Second run (cache hit): " << duration3b.count() << " microseconds\n";
    std::cout << "Speedup: " << (double)duration3a.count() / duration3b.count() << "x\n\n";
    
    // Test 4: Cache statistics
    std::cout << "Test 4: Cache statistics\n";
    std::cout << "Text cache size: " << UnicodeRegex::get_text_cache_size() << "\n";
    std::cout << "Offset cache size: " << UnicodeRegex::get_offset_cache_size() << "\n\n";
    
    // Test 5: ASCII optimization
    std::cout << "Test 5: ASCII optimization test\n";
    std::string ascii_input = "hello\nworld\nhello\nworld\nhello\nworld\nhello\nworld\nhello\nworld";
    std::string ascii_pattern = "hello";
    
    start = std::chrono::high_resolution_clock::now();
    auto ascii_results = grep_unicode(ascii_input, ascii_pattern, "i", "\n", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    end = std::chrono::high_resolution_clock::now();
    auto ascii_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "ASCII pattern found " << ascii_results.size() << " matches in " << ascii_duration.count() << " microseconds\n";
    std::cout << "ASCII optimization working: " << (ascii_duration.count() < duration1.count() ? "YES" : "NO") << "\n\n";
    
    // Test 6: Cache cleanup
    std::cout << "Test 6: Cache cleanup\n";
    UnicodeRegex::clear_all_caches();
    std::cout << "After cleanup - Text cache size: " << UnicodeRegex::get_text_cache_size() << "\n";
    std::cout << "After cleanup - Offset cache size: " << UnicodeRegex::get_offset_cache_size() << "\n\n";
    
    std::cout << "All optimization tests completed!\n";
}

int main() {
    test_optimization();
    return 0;
} 