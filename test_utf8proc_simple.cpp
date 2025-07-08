#include <iostream>
#include <string>

// Define UTF8PROC_STATIC for static linking
#define UTF8PROC_STATIC

// Include utf8proc
#include "source/utf8proc/utf8proc.h"

int main() {
    std::cout << "Testing utf8proc functions..." << std::endl;
    
    // Test basic utf8proc functions
    const char* test_str = "Hello World";
    utf8proc_uint8_t* result = nullptr;
    
    utf8proc_ssize_t result_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(test_str),
        strlen(test_str),
        &result,
        UTF8PROC_STABLE
    );
    
    if (result_len >= 0) {
        std::cout << "utf8proc_map succeeded! Result length: " << result_len << std::endl;
        std::cout << "Result: " << result << std::endl;
        free(result);
    } else {
        std::cout << "utf8proc_map failed with error: " << result_len << std::endl;
    }
    
    // Test case folding
    const char* test_case = "HELLO";
    utf8proc_uint8_t* folded = nullptr;
    
    utf8proc_ssize_t folded_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(test_case),
        strlen(test_case),
        &folded,
        static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_CASEFOLD)
    );
    
    if (folded_len >= 0) {
        std::cout << "Case folding succeeded! Result: " << folded << std::endl;
        free(folded);
    } else {
        std::cout << "Case folding failed with error: " << folded_len << std::endl;
    }
    
    std::cout << "utf8proc test completed!" << std::endl;
    return 0;
} 