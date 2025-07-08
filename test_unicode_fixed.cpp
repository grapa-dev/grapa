#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <string>
#include <iomanip>

void print_hex(const std::string& str, const std::string& label) {
    std::cout << label << " (hex): ";
    for (unsigned char c : str) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "Unicode Fixed Test" << std::endl;
    std::cout << "==================" << std::endl;

    // Use actual UTF-8 characters instead of escape sequences
    std::string input_nfc = "café\n";  // NFC form
    std::string input_nfd = "café\n";  // NFD form (e + combining acute) - actual UTF-8
    std::string pattern = "café";
    
    std::cout << "\nInput NFC: " << input_nfc;
    std::cout << "Input NFD: " << input_nfd;
    std::cout << "Pattern: " << pattern << std::endl;
    
    // Test normalization
    GrapaUnicode::UnicodeString unicode_nfc(input_nfc);
    GrapaUnicode::UnicodeString unicode_nfd(input_nfd);
    GrapaUnicode::UnicodeString unicode_pattern(pattern);
    
    // Check if strings are valid UTF-8
    std::cout << "\nUTF-8 validation:" << std::endl;
    std::cout << "NFC input valid: " << (unicode_nfc.is_valid_utf8() ? "YES" : "NO") << std::endl;
    std::cout << "NFD input valid: " << (unicode_nfd.is_valid_utf8() ? "YES" : "NO") << std::endl;
    std::cout << "Pattern valid: " << (unicode_pattern.is_valid_utf8() ? "YES" : "NO") << std::endl;
    
    // Check character counts
    std::cout << "\nCharacter counts:" << std::endl;
    std::cout << "NFC input chars: " << unicode_nfc.char_count() << std::endl;
    std::cout << "NFD input chars: " << unicode_nfd.char_count() << std::endl;
    std::cout << "Pattern chars: " << unicode_pattern.char_count() << std::endl;
    
    auto norm_nfc = unicode_nfc.normalize(GrapaUnicode::NormalizationForm::NFC);
    auto norm_nfd = unicode_nfd.normalize(GrapaUnicode::NormalizationForm::NFC);
    auto norm_pattern = unicode_pattern.normalize(GrapaUnicode::NormalizationForm::NFC);
    
    std::cout << "\nAfter NFC normalization:" << std::endl;
    std::cout << "NFC input: " << norm_nfc.data() << std::endl;
    std::cout << "NFD input: " << norm_nfd.data() << std::endl;
    std::cout << "Pattern: " << norm_pattern.data() << std::endl;
    
    print_hex(norm_nfc.data(), "NFC input");
    print_hex(norm_nfd.data(), "NFD input");
    print_hex(norm_pattern.data(), "Pattern");
    
    // Check if normalization changed anything
    std::cout << "\nNormalization changes:" << std::endl;
    std::cout << "NFC changed: " << (norm_nfc.data() != unicode_nfc.data() ? "YES" : "NO") << std::endl;
    std::cout << "NFD changed: " << (norm_nfd.data() != unicode_nfd.data() ? "YES" : "NO") << std::endl;
    std::cout << "Pattern changed: " << (norm_pattern.data() != unicode_pattern.data() ? "YES" : "NO") << std::endl;
    
    // Test direct string comparison
    bool nfc_matches = (norm_nfc.data() == norm_pattern.data());
    bool nfd_matches = (norm_nfd.data() == norm_pattern.data());
    
    std::cout << "\nDirect comparison:" << std::endl;
    std::cout << "NFC matches pattern: " << (nfc_matches ? "YES" : "NO") << std::endl;
    std::cout << "NFD matches pattern: " << (nfd_matches ? "YES" : "NO") << std::endl;
    
    // Test regex matching
    GrapaUnicode::UnicodeRegex regex(pattern, false, GrapaUnicode::NormalizationForm::NFC);
    
    bool nfc_regex = regex.search(unicode_nfc);
    bool nfd_regex = regex.search(unicode_nfd);
    
    std::cout << "\nRegex matching:" << std::endl;
    std::cout << "NFC regex match: " << (nfc_regex ? "YES" : "NO") << std::endl;
    std::cout << "NFD regex match: " << (nfd_regex ? "YES" : "NO") << std::endl;
    
    return 0;
} 