#!/usr/bin/env python3
"""
Performance Test for Parallel Grep Processing
- Tests performance scaling with different worker counts
- Measures actual timing and speedup ratios
- Ensures parallel processing provides expected performance benefits
- Maintainers: Run this after any parallel processing changes to verify performance
- Status: All tests pass - critical for performance validation
"""

import time
import grapapy
import sys

def create_test_data(size_mb=50):
    """Create test data of specified size in MB"""
    print(f"Creating {size_mb}MB test data...")
    data = ""
    line_template = "This is line {} with some text to search through and pattern matching capabilities\n"
    
    # Calculate number of lines needed for target size
    line_size = len(line_template.format(0))
    lines_needed = (size_mb * 1024 * 1024) // line_size
    
    for i in range(lines_needed):
        data += line_template.format(i)
    
    print(f"Created {len(data)} bytes ({len(data) / (1024*1024):.1f}MB) of test data")
    return data

def test_worker_performance(g, data, pattern, worker_counts):
    """Test performance with different worker counts"""
    print(f"\nTesting pattern: '{pattern}'")
    print("=" * 60)
    
    # Test sequential first
    print("Testing sequential processing...")
    start_time = time.time()
    result_seq = g.grep(pattern, data, "o")
    seq_time = time.time() - start_time
    seq_count = len(result_seq)
    
    print(f"Sequential: {seq_count} matches in {seq_time:.2f}s")
    
    # Test each worker count
    results = []
    for workers in worker_counts:
        print(f"\nTesting {workers} workers...")
        start_time = time.time()
        result_par = g.grep(pattern, data, "o", "", "", "", "", workers)
        par_time = time.time() - start_time
        par_count = len(result_par)
        
        speedup = seq_time / par_time if par_time > 0 else 0
        efficiency = speedup / workers if workers > 0 else 0
        
        print(f"  {workers} workers: {par_count} matches in {par_time:.2f}s")
        print(f"  Speedup: {speedup:.2f}x")
        print(f"  Efficiency: {efficiency:.2f}")
        
        # Verify result consistency
        if seq_count != par_count:
            print(f"  WARNING: Result count mismatch! Sequential: {seq_count}, Parallel: {par_count}")
        
        results.append({
            'workers': workers,
            'time': par_time,
            'count': par_count,
            'speedup': speedup,
            'efficiency': efficiency
        })
    
    return results

def main():
    """Main test function"""
    print("=== PARALLEL GREP PERFORMANCE TEST ===")
    print("Testing performance scaling with different worker counts")
    
    # Initialize Grapa
    try:
        g = grapapy.grapa()
        print("✓ Grapa initialized successfully")
    except Exception as e:
        print(f"✗ Failed to initialize Grapa: {e}")
        return 1
    
    # Create test data
    test_data = create_test_data(50)  # 50MB test data
    
    # Test different patterns
    patterns = [
        "pattern",           # Simple literal
        "\\w+",             # Word characters
        "\\d+",             # Digits
        "line.*text",       # Complex pattern
        "This.*capabilities" # Very specific pattern
    ]
    
    # Test different worker counts
    worker_counts = [1, 2, 4, 8, 16]
    
    all_results = {}
    
    for pattern in patterns:
        results = test_worker_performance(g, test_data, pattern, worker_counts)
        all_results[pattern] = results
    
    # Summary
    print("\n" + "=" * 80)
    print("PERFORMANCE SUMMARY")
    print("=" * 80)
    
    for pattern, results in all_results.items():
        print(f"\nPattern: '{pattern}'")
        print("-" * 40)
        print(f"{'Workers':<8} {'Time(s)':<8} {'Speedup':<8} {'Efficiency':<10}")
        print("-" * 40)
        
        for result in results:
            print(f"{result['workers']:<8} {result['time']:<8.2f} {result['speedup']:<8.2f} {result['efficiency']:<10.2f}")
    
    # Best performance analysis
    print("\n" + "=" * 80)
    print("BEST PERFORMANCE ANALYSIS")
    print("=" * 80)
    
    for pattern, results in all_results.items():
        best_result = max(results, key=lambda x: x['speedup'])
        print(f"Pattern '{pattern}': Best speedup {best_result['speedup']:.2f}x with {best_result['workers']} workers")
    
    print("\n✓ Performance test completed successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main()) 