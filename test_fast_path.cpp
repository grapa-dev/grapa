#include <iostream>
#include <string>
#include <chrono>

// Simple test to verify the fast path logic
void test_fast_path() {
    std::cout << "Fast Path Test\n";
    std::cout << "==============\n\n";
    
    std::string pattern = "😀+";
    std::cout << "Pattern: " << pattern << "\n";
    std::cout << "Pattern ends with +: " << (pattern.size() >= 2 && pattern.back() == '+') << "\n";
    
    if (pattern.size() >= 2 && pattern.back() == '+') {
        std::string base_char = pattern.substr(0, pattern.size() - 1);
        std::cout << "Base character: " << base_char << "\n";
        std::cout << "Base character length: " << base_char.length() << " bytes\n";
        
        // Create test input (10,000 emojis)
        std::string input = "";
        for (int i = 0; i < 10000; ++i) {
            input += "😀";
        }
        
        std::cout << "Input length: " << input.length() << " bytes\n";
        std::cout << "Input size > 10000: " << (input.size() > 10000) << "\n\n";
        
        // Test the fast path logic
        std::cout << "Testing fast path logic:\n";
        auto start = std::chrono::high_resolution_clock::now();
        
        bool all_same = true;
        for (size_t i = 0; i < input.size(); i += base_char.size()) {
            if (i + base_char.size() > input.size() || 
                input.compare(i, base_char.size(), base_char) != 0) {
                all_same = false;
                break;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "All characters same: " << (all_same ? "YES" : "NO") << "\n";
        std::cout << "Time: " << duration.count() << " microseconds\n";
        
        if (all_same) {
            std::cout << "Fast path would return: offset=0, length=" << input.size() << "\n";
        }
    }
    
    std::cout << "\nTest completed!\n";
}

int main() {
    test_fast_path();
    return 0;
} 