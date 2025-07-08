// test_unicode_grep.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "source/grep/grapa_grep_unicode.hpp"
#include <iostream>
#include <string>

using namespace GrapaUnicode;

int main() {
    std::cout << "Unicode Grep Test\n";
    std::cout << "==================\n\n";

    // Test 1: Basic Unicode normalization
    std::cout << "Test 1: Unicode Normalization\n";
    std::string text1 = "café"; // é as single character
    std::string text2 = "cafe\u0301"; // e + combining acute accent
    
    UnicodeString str1(text1);
    UnicodeString str2(text2);
    
    std::cout << "Text 1: " << text1 << " (length: " << text1.size() << " bytes)\n";
    std::cout << "Text 2: " << text2 << " (length: " << text2.size() << " bytes)\n";
    
    UnicodeString norm1 = str1.normalize(NormalizationForm::NFC);
    UnicodeString norm2 = str2.normalize(NormalizationForm::NFC);
    
    std::cout << "Normalized Text 1: " << norm1.data() << " (length: " << norm1.data().size() << " bytes)\n";
    std::cout << "Normalized Text 2: " << norm2.data() << " (length: " << norm2.data().size() << " bytes)\n";
    std::cout << "Equal after normalization: " << (norm1.data() == norm2.data() ? "YES" : "NO") << "\n\n";

    // Test 2: Case folding
    std::cout << "Test 2: Case Folding\n";
    std::string text3 = "İstanbul"; // Turkish capital I with dot
    std::string text4 = "istanbul";
    
    UnicodeString str3(text3);
    UnicodeString str4(text4);
    
    std::cout << "Text 3: " << text3 << "\n";
    std::cout << "Text 4: " << text4 << "\n";
    
    UnicodeString folded3 = str3.case_fold();
    UnicodeString folded4 = str4.case_fold();
    
    std::cout << "Case-folded Text 3: " << folded3.data() << "\n";
    std::cout << "Case-folded Text 4: " << folded4.data() << "\n";
    std::cout << "Equal after case folding: " << (folded3.data() == folded4.data() ? "YES" : "NO") << "\n\n";

    // Test 3: Unicode-aware grep
    std::cout << "Test 3: Unicode-Aware Grep\n";
    std::string input = "café\ncafe\u0301\nİstanbul\nistanbul\n";
    std::string pattern = "café";
    
    std::cout << "Input text:\n" << input;
    std::cout << "Pattern: " << pattern << "\n";
    
    auto matches = grep_unicode(input, pattern, "i", "", NormalizationForm::NFC); // case-insensitive
    
    std::cout << "Matches found: " << matches.size() << "\n";
    for (const auto& match : matches) {
        std::cout << "  Line " << match.line_number << ": offset=" << match.offset 
                  << ", length=" << match.length << "\n";
    }
    std::cout << "\n";

    // Test 4: Unicode character count
    std::cout << "Test 4: Unicode Character Count\n";
    std::string text5 = "café"; // 4 characters, 5 bytes
    UnicodeString str5(text5);
    
    std::cout << "Text: " << text5 << "\n";
    std::cout << "Byte count: " << text5.size() << "\n";
    std::cout << "Character count: " << str5.char_count() << "\n";
    std::cout << "Valid UTF-8: " << (str5.is_valid_utf8() ? "YES" : "NO") << "\n\n";

    // Test 5: Unicode regex with different normalization forms
    std::cout << "Test 5: Unicode Regex with Different Normalization Forms\n";
    std::string test_text = "café\ncafe\u0301\n";
    std::string test_pattern = "café";
    
    std::cout << "Test text:\n" << test_text;
    std::cout << "Pattern: " << test_pattern << "\n";
    
    // Test with NFC normalization
    auto matches_nfc = grep_unicode(test_text, test_pattern, "", "", NormalizationForm::NFC);
    std::cout << "Matches with NFC: " << matches_nfc.size() << "\n";
    
    // Test with NFD normalization
    auto matches_nfd = grep_unicode(test_text, test_pattern, "", "", NormalizationForm::NFD);
    std::cout << "Matches with NFD: " << matches_nfd.size() << "\n";
    
    // Test with NFKC normalization
    auto matches_nfkc = grep_unicode(test_text, test_pattern, "", "", NormalizationForm::NFKC);
    std::cout << "Matches with NFKC: " << matches_nfkc.size() << "\n";
    
    // Test with NFKD normalization
    auto matches_nfkd = grep_unicode(test_text, test_pattern, "", "", NormalizationForm::NFKD);
    std::cout << "Matches with NFKD: " << matches_nfkd.size() << "\n\n";

    std::cout << "All tests completed!\n";
    return 0;
} 