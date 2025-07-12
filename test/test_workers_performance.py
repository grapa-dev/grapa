#!/usr/bin/env python3
"""
Test different num_workers values with large inputs using GrapaPy's xy.eval and the grep scripting interface.
"""

import grapapy
import time
import sys

def create_large_input(size_mb=50):
    """Create a large string with repeated patterns, about size_mb megabytes."""
    lines = []
    for i in range(size_mb * 1000):
        lines.append(f"Line {i:06d}: This is a test line with pattern 'test' and some random text")
        lines.append(f"Line {i:06d}: Another line without the pattern")
        lines.append(f"Line {i:06d}: test pattern appears here")
        lines.append(f"Line {i:06d}: More content without pattern")
    return '\n'.join(lines)

def test_workers_performance():
    print("Testing num_workers performance with large input using GrapaPy...")
    xy = grapapy.grapa()
    input_text = create_large_input(size_mb=50)  # Increased to 50MB for longer baseline
    pattern = "test"
    options = "n"
    delimiter = "\n"
    normalization = "NONE"
    mode = "UNICODE"
    worker_counts = [1, 2, 4, 8, 16]
    results = {}

    for workers in worker_counts:
        print(f"\nTesting with num_workers={workers}...")
        start_time = time.time()
        result = xy.eval(
            "input.grep(pattern, options, delimiter, normalization, mode, num_workers);",
            {
                "input": input_text,
                "pattern": pattern,
                "options": options,
                "delimiter": delimiter,
                "normalization": normalization,
                "mode": mode,
                "num_workers": workers,
            }
        )
        end_time = time.time()
        duration = end_time - start_time
        match_count = len(result) if isinstance(result, list) else 0
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Matches found: {match_count}")
        results[workers] = {
            'duration': duration,
            'matches': match_count,
        }

    # Print summary
    print("\n" + "="*60)
    print("PERFORMANCE SUMMARY")
    print("="*60)
    print(f"{'Workers':<8} {'Duration':<12} {'Matches':<10}")
    print("-" * 60)
    for workers, result in results.items():
        print(f"{workers:<8} {result['duration']:<12.2f} {result['matches']:<10}")
    if 1 in results:
        single_worker_time = results[1]['duration']
        print(f"\nSingle worker baseline: {single_worker_time:.2f} seconds")
        for workers in [2, 4, 8, 16]:
            if workers in results:
                speedup = single_worker_time / results[workers]['duration']
                print(f"{workers} workers: {speedup:.2f}x speedup")
    match_counts = [r['matches'] for r in results.values()]
    if len(set(match_counts)) == 1:
        print(f"\n✓ All worker counts produced same number of matches: {match_counts[0]}")
    else:
        print(f"\n✗ Inconsistent match counts: {match_counts}")

def test_workers_edge_cases():
    print("\n" + "="*60)
    print("TESTING WORKER EDGE CASES")
    print("="*60)
    xy = grapapy.grapa()
    input_text = create_large_input(size_mb=10)  # Smaller for edge cases
    pattern = "test"
    options = "n"
    delimiter = "\n"
    normalization = "NONE"
    mode = "UNICODE"
    edge_cases = [0, -1, 999, 1000]
    for workers in edge_cases:
        print(f"\nTesting edge case: num_workers={workers}")
        try:
            result = xy.eval(
                "input.grep(pattern, options, delimiter, normalization, mode, num_workers);",
                {
                    "input": input_text,
                    "pattern": pattern,
                    "options": options,
                    "delimiter": delimiter,
                    "normalization": normalization,
                    "mode": mode,
                    "num_workers": workers,
                }
            )
            match_count = len(result) if isinstance(result, list) else 0
            print(f"  Matches found: {match_count}")
        except Exception as e:
            print(f"  Error: {e}")

if __name__ == "__main__":
    print("GRAPA WORKERS PERFORMANCE TEST (Python API)")
    print("="*60)
    try:
        import grapapy
    except ImportError:
        print("Error: grapapy module not found. Please install with 'pip install grapapy'.")
        sys.exit(1)
    test_workers_performance()
    test_workers_edge_cases()
    print("\n" + "="*60)
    print("TEST COMPLETED")
    print("="*60) 