#include <iostream>
#include <string>
#include <iomanip>

// Define UTF8PROC_STATIC for static linking
#define UTF8PROC_STATIC

// Include utf8proc
#include "source/utf8proc/utf8proc.h"

void print_hex(const std::string& str, const std::string& label) {
    std::cout << label << " (hex): ";
    for (unsigned char c : str) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "Direct utf8proc Test" << std::endl;
    std::cout << "====================" << std::endl;

    // Test NFD to NFC normalization directly with utf8proc
    std::string input_nfd = "cafe\u0301";  // NFD form (e + combining acute)
    
    std::cout << "\nInput NFD: " << input_nfd << std::endl;
    print_hex(input_nfd, "Input NFD");
    
    // Normalize to NFC using utf8proc directly
    utf8proc_uint8_t* result = nullptr;
    utf8proc_ssize_t result_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(input_nfd.c_str()),
        static_cast<utf8proc_ssize_t>(input_nfd.size()),
        &result,
        static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_COMPOSE)
    );
    
    if (result_len >= 0) {
        std::string normalized(reinterpret_cast<const char*>(result), result_len);
        std::cout << "\nAfter NFC normalization: " << normalized << std::endl;
        print_hex(normalized, "Normalized");
        
        // Compare with expected result
        std::string expected = "café";
        bool matches = (normalized == expected);
        std::cout << "\nMatches expected 'café': " << (matches ? "YES" : "NO") << std::endl;
        
        free(result);
    } else {
        std::cout << "\nNormalization failed with error: " << result_len << std::endl;
    }
    
    return 0;
} 