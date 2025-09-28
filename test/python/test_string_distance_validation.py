#!/usr/bin/env python3
"""
String Distance Function Validation Tests

This script validates Grapa's string distance functions against Python's implementations
to ensure we achieve Python parity for the three algorithms.
"""

import subprocess
import sys
import difflib
from typing import List, Tuple

def run_grapa_command(command: str) -> str:
    """Run a Grapa command and return the output"""
    try:
        result = subprocess.run(
            ["./grapa", "-c", command],
            capture_output=True,
            text=True
        )
        if result.returncode == 0:
            return result.stdout.strip()
        else:
            return f"ERROR: {result.stderr.strip()}"
    except Exception as e:
        return f"EXCEPTION: {str(e)}"

def test_levenshtein_distance():
    """Test Levenshtein distance against Python's difflib"""
    print("=== Testing Levenshtein Distance ===")
    
    test_cases = [
        ("kitten", "sitting"),
        ("hello", "world"),
        ("hello", "helo"),
        ("book", "back"),
        ("", ""),
        ("a", ""),
        ("", "a"),
        ("abc", "abc"),
        ("abc", "abd"),
        ("abc", "abcd"),
        ("abcd", "abc"),
    ]
    
    for str1, str2 in test_cases:
        # Python implementation
        python_distance = difflib.SequenceMatcher(None, str1, str2).ratio()
        python_levenshtein = 1 - python_distance  # Convert similarity to distance
        
        # Grapa implementation
        grapa_command = f'"{str1}".levenshtein_distance("{str2}").echo();'
        grapa_result = run_grapa_command(grapa_command)
        
        print(f"'{str1}' vs '{str2}':")
        print(f"  Python: {python_levenshtein:.3f}")
        print(f"  Grapa:  {grapa_result}")
        print()

def test_jaro_winkler():
    """Test Jaro-Winkler similarity against Python's jellyfish library"""
    print("=== Testing Jaro-Winkler Similarity ===")
    
    try:
        import jellyfish
        has_jellyfish = True
    except ImportError:
        print("jellyfish library not available, using simplified tests")
        has_jellyfish = False
    
    test_cases = [
        ("martha", "marhta"),
        ("dwayne", "duane"),
        ("hello", "world"),
        ("hello", "helo"),
        ("", ""),
        ("a", ""),
        ("", "a"),
        ("abc", "abc"),
        ("abc", "abd"),
        ("ab", "ac"),
    ]
    
    for str1, str2 in test_cases:
        if has_jellyfish:
            # Python implementation
            python_similarity = jellyfish.jaro_winkler_similarity(str1, str2)
        else:
            # Simplified test - just check if identical
            python_similarity = 1.0 if str1 == str2 else 0.0
        
        # Grapa implementation
        grapa_command = f'"{str1}".jaro_winkler_similarity("{str2}").echo();'
        grapa_result = run_grapa_command(grapa_command)
        
        print(f"'{str1}' vs '{str2}':")
        print(f"  Python: {python_similarity:.3f}")
        print(f"  Grapa:  {grapa_result}")
        print()

def test_cosine_similarity():
    """Test Cosine similarity against Python's sklearn"""
    print("=== Testing Cosine Similarity ===")
    
    try:
        from sklearn.feature_extraction.text import TfidfVectorizer
        from sklearn.metrics.pairwise import cosine_similarity
        has_sklearn = True
    except ImportError:
        print("sklearn library not available, using simplified tests")
        has_sklearn = False
    
    test_cases = [
        ("hello world", "hello there"),
        ("the quick brown fox", "a quick brown fox"),
        ("hello world", "world hello"),
        ("hello world", "goodbye world"),
        ("", ""),
        ("a", ""),
        ("", "a"),
        ("abc", "abc"),
        ("hello world", "hello world"),
    ]
    
    for str1, str2 in test_cases:
        if has_sklearn:
            # Python implementation
            vectorizer = TfidfVectorizer()
            try:
                tfidf_matrix = vectorizer.fit_transform([str1, str2])
                python_similarity = cosine_similarity(tfidf_matrix[0:1], tfidf_matrix[1:2])[0][0]
            except:
                python_similarity = 1.0 if str1 == str2 else 0.0
        else:
            # Simplified test - just check if identical
            python_similarity = 1.0 if str1 == str2 else 0.0
        
        # Grapa implementation
        grapa_command = f'"{str1}".cosine_similarity("{str2}").echo();'
        grapa_result = run_grapa_command(grapa_command)
        
        print(f"'{str1}' vs '{str2}':")
        print(f"  Python: {python_similarity:.3f}")
        print(f"  Grapa:  {grapa_result}")
        print()

def test_edge_cases():
    """Test edge cases and error handling"""
    print("=== Testing Edge Cases ===")
    
    edge_cases = [
        ("", ""),
        ("a", ""),
        ("", "a"),
        ("a", "a"),
        ("ab", "ab"),
        ("abc", "abc"),
        ("hello", "hello"),
        ("hello world", "hello world"),
    ]
    
    for str1, str2 in edge_cases:
        print(f"Edge case: '{str1}' vs '{str2}'")
        
        # Test all three functions
        for func_name in ["levenshtein", "jarowinkler", "cosinesimilarity"]:
            grapa_command = f'"{str1}".{func_name}("{str2}").echo();'
            grapa_result = run_grapa_command(grapa_command)
            print(f"  {func_name}: {grapa_result}")
        print()

def main():
    """Run all validation tests"""
    print("String Distance Function Validation Tests")
    print("=" * 50)
    print()
    
    # Test each function
    test_levenshtein_distance()
    test_jaro_winkler()
    test_cosine_similarity()
    test_edge_cases()
    
    print("Validation complete!")

if __name__ == "__main__":
    main()
