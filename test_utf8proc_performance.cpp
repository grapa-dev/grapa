#include "source/utf8proc/utf8proc.h"
#include <iostream>
#include <chrono>
#include <string>

int main() {
    std::cout << "UTF8PROC Performance Test\n";
    std::cout << "=========================\n\n";
    
    // Test data
    std::string test_input = "café";
    std::string test_input_nfd = "cafe\u0301";
    
    std::cout << "Test input (NFC): " << test_input << "\n";
    std::cout << "Test input (NFD): " << test_input_nfd << "\n\n";
    
    // Test 1: Simple utf8proc_map call
    std::cout << "Test 1: Simple utf8proc_map (NFC)\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    utf8proc_uint8_t* result = nullptr;
    utf8proc_ssize_t result_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(test_input.c_str()),
        static_cast<utf8proc_ssize_t>(test_input.size()),
        &result,
        static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_COMPOSE)
    );
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "utf8proc_map time: " << duration.count() << " microseconds\n";
    std::cout << "Result length: " << result_len << "\n";
    if (result_len > 0) {
        std::string normalized(reinterpret_cast<const char*>(result), result_len);
        std::cout << "Normalized result: " << normalized << "\n";
    }
    free(result);
    std::cout << "\n";
    
    // Test 2: NFD normalization
    std::cout << "Test 2: NFD normalization\n";
    start = std::chrono::high_resolution_clock::now();
    
    result = nullptr;
    result_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(test_input.c_str()),
        static_cast<utf8proc_ssize_t>(test_input.size()),
        &result,
        static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_DECOMPOSE)
    );
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "utf8proc_map NFD time: " << duration.count() << " microseconds\n";
    std::cout << "Result length: " << result_len << "\n";
    if (result_len > 0) {
        std::string normalized(reinterpret_cast<const char*>(result), result_len);
        std::cout << "NFD result: " << normalized << "\n";
    }
    free(result);
    std::cout << "\n";
    
    // Test 3: Case folding
    std::cout << "Test 3: Case folding\n";
    start = std::chrono::high_resolution_clock::now();
    
    result = nullptr;
    result_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(test_input.c_str()),
        static_cast<utf8proc_ssize_t>(test_input.size()),
        &result,
        static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_CASEFOLD)
    );
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "utf8proc_map case fold time: " << duration.count() << " microseconds\n";
    std::cout << "Result length: " << result_len << "\n";
    if (result_len > 0) {
        std::string folded(reinterpret_cast<const char*>(result), result_len);
        std::cout << "Case folded result: " << folded << "\n";
    }
    free(result);
    std::cout << "\n";
    
    // Test 4: Multiple iterations
    std::cout << "Test 4: Multiple iterations (1000x)\n";
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        result = nullptr;
        result_len = utf8proc_map(
            reinterpret_cast<const utf8proc_uint8_t*>(test_input.c_str()),
            static_cast<utf8proc_ssize_t>(test_input.size()),
            &result,
            static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_COMPOSE)
        );
        free(result);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "1000 iterations time: " << duration.count() << " microseconds\n";
    std::cout << "Average per iteration: " << duration.count() / 1000.0 << " microseconds\n\n";
    
    std::cout << "UTF8PROC performance test completed!\n";
    return 0;
} 