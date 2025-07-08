#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <stdexcept>

using namespace GrapaUnicode;

void test_emoji_performance() {
    std::cout << "Emoji Performance Test (Safe Version)\n";
    std::cout << "=====================================\n\n";
    
    try {
        // Test 1: Simple ASCII test first
        std::cout << "Test 1: Simple ASCII test\n";
        std::string ascii_input = "";
        for (int i = 0; i < 1000; ++i) {
            ascii_input += "a";
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ascii_results = grep_unicode(ascii_input, "a+", "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "ASCII test time: " << duration.count() << " milliseconds\n";
        std::cout << "ASCII results: " << ascii_results.size() << "\n\n";
        
        // Test 2: Simple Unicode test (not emoji)
        std::cout << "Test 2: Simple Unicode test (é)\n";
        std::string unicode_input = "";
        for (int i = 0; i < 500; ++i) {
            unicode_input += "é";  // 2-byte UTF-8 character
        }
        
        start = std::chrono::high_resolution_clock::now();
        auto unicode_results = grep_unicode(unicode_input, "é+", "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Unicode test time: " << duration.count() << " milliseconds\n";
        std::cout << "Unicode results: " << unicode_results.size() << "\n\n";
        
        // Test 3: Small emoji test
        std::cout << "Test 3: Small emoji test (100 emoji)\n";
        std::string small_emoji_input = "";
        for (int i = 0; i < 100; ++i) {
            small_emoji_input += "😀";
        }
        
        start = std::chrono::high_resolution_clock::now();
        auto small_emoji_results = grep_unicode(small_emoji_input, "😀+", "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Small emoji test time: " << duration.count() << " milliseconds\n";
        std::cout << "Small emoji results: " << small_emoji_results.size() << "\n\n";
        
        // Test 4: Medium emoji test (only if small test succeeds)
        if (duration.count() < 1000) {  // Only if small test is fast
            std::cout << "Test 4: Medium emoji test (500 emoji)\n";
            std::string medium_emoji_input = "";
            for (int i = 0; i < 500; ++i) {
                medium_emoji_input += "😀";
            }
            
            start = std::chrono::high_resolution_clock::now();
            auto medium_emoji_results = grep_unicode(medium_emoji_input, "😀+", "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "Medium emoji test time: " << duration.count() << " milliseconds\n";
            std::cout << "Medium emoji results: " << medium_emoji_results.size() << "\n\n";
            
            // Test 5: Large emoji test (only if medium test succeeds)
            if (duration.count() < 5000) {  // Only if medium test is reasonable
                std::cout << "Test 5: Large emoji test (1000 emoji)\n";
                std::string large_emoji_input = "";
                for (int i = 0; i < 1000; ++i) {
                    large_emoji_input += "😀";
                }
                
                start = std::chrono::high_resolution_clock::now();
                auto large_emoji_results = grep_unicode(large_emoji_input, "😀+", "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "Large emoji test time: " << duration.count() << " milliseconds\n";
                std::cout << "Large emoji results: " << large_emoji_results.size() << "\n\n";
            } else {
                std::cout << "Skipping large emoji test (medium test too slow)\n\n";
            }
        } else {
            std::cout << "Skipping medium emoji test (small test too slow)\n\n";
        }
        
        // Test 6: Fast path detection test
        std::cout << "Test 6: Fast path detection\n";
        UnicodeRegex regex1("😀+", false, NormalizationForm::NFC);
        UnicodeRegex regex2("😀++", false, NormalizationForm::NFC);
        
        std::cout << "Is simple repeated pattern (😀+): " << (regex1.is_simple_repeated_pattern() ? "YES" : "NO") << "\n";
        std::cout << "Is simple repeated pattern (😀++): " << (regex2.is_simple_repeated_pattern() ? "YES" : "NO") << "\n";
        
        std::cout << "\nEmoji performance test completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in emoji performance test: " << e.what() << std::endl;
        std::cout << "Test failed due to exception\n";
    } catch (...) {
        std::cerr << "Unknown error in emoji performance test" << std::endl;
        std::cout << "Test failed due to unknown error\n";
    }
}

int main() {
    test_emoji_performance();
    return 0;
} 