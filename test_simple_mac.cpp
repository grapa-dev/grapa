// Simple test for Mac ARM64 Unicode grep
#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>

using namespace GrapaUnicode;

int main() {
    std::cout << "Simple Mac ARM64 Unicode Grep Test\n";
    std::cout << "===================================\n\n";

    // Test basic Unicode string functionality
    std::string text = "café";
    UnicodeString str(text);
    
    std::cout << "Original text: " << text << " (length: " << text.size() << " bytes)\n";
    std::cout << "Character count: " << str.char_count() << "\n";
    std::cout << "Valid UTF-8: " << (str.is_valid_utf8() ? "YES" : "NO") << "\n\n";

    // Test normalization
    UnicodeString norm = str.normalize(NormalizationForm::NFC);
    std::cout << "Normalized text: " << norm.data() << " (length: " << norm.data().size() << " bytes)\n\n";

    // Test case folding
    UnicodeString folded = str.case_fold();
    std::cout << "Case-folded text: " << folded.data() << " (length: " << folded.data().size() << " bytes)\n\n";

    // Test simple grep
    std::string input = "café\ncoffee\n";
    std::string pattern = "café";
    
    auto matches = grep_unicode(input, pattern, "o", "", ProcessingMode::UNICODE_MODE, NormalizationForm::NFC);
    
    std::cout << "Input: " << input;
    std::cout << "Pattern: " << pattern << "\n";
    std::cout << "Matches found: " << matches.size() << "\n";
    
    for (const auto& match : matches) {
        std::cout << "  Line " << match.line_number << ": offset=" << match.offset 
                  << ", length=" << match.length << "\n";
    }

    std::cout << "\nTest completed successfully!\n";
    return 0;
} 