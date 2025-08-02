#!/usr/bin/env python3
"""
GrapaPy Validation Runner
Runs comprehensive validation tests for GrapaPy installation
"""

import sys
import subprocess
import importlib.util

def check_grapapy_installation():
    """Check if GrapaPy is properly installed"""
    print("=== Checking GrapaPy Installation ===")
    
    try:
        import grapapy
        print("   ✅ GrapaPy imported successfully")
        
        # Test basic instantiation
        xy = grapapy.grapa()
        result = xy.eval("2 + 2;")
        if result == 4:
            print("   ✅ Basic evaluation works")
        else:
            print(f"   ❌ Basic evaluation failed: expected 4, got {result}")
            return False
            
        return True
        
    except ImportError as e:
        print(f"   ❌ GrapaPy import failed: {e}")
        print("   💡 Try: pip install grapapy==0.0.49")
        return False
    except Exception as e:
        print(f"   ❌ GrapaPy initialization failed: {e}")
        return False

def run_test_module(module_name, description):
    """Run a test module and report results"""
    print(f"\n=== Running {description} ===")
    
    try:
        # Import and run the test module
        spec = importlib.util.spec_from_file_location(module_name, f"{module_name}.py")
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        
        if hasattr(module, 'main'):
            module.main()
            print(f"   ✅ {description} completed successfully")
            return True
        else:
            print(f"   ❌ {description} has no main() function")
            return False
            
    except Exception as e:
        print(f"   ❌ {description} failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def run_individual_tests():
    """Run individual test files"""
    tests = [
        ("test_basic_operations", "Basic Operations Test"),
        ("test_functional_methods", "Functional Methods Test"),
        ("test_file_operations", "File Operations Test"),
    ]
    
    results = []
    
    for module_name, description in tests:
        success = run_test_module(module_name, description)
        results.append((description, success))
    
    return results

def print_summary(results):
    """Print test summary"""
    print("\n" + "="*60)
    print("VALIDATION SUMMARY")
    print("="*60)
    
    passed = 0
    total = len(results)
    
    for description, success in results:
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"{status} {description}")
        if success:
            passed += 1
    
    print(f"\nResults: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! GrapaPy is working correctly.")
        return True
    else:
        print("⚠️  Some tests failed. Check the output above for details.")
        return False

def main():
    """Main validation runner"""
    print("GrapaPy Validation Runner")
    print("=" * 60)
    
    # Check installation first
    if not check_grapapy_installation():
        print("\n❌ GrapaPy installation check failed. Please fix installation issues first.")
        sys.exit(1)
    
    # Run individual tests
    results = run_individual_tests()
    
    # Print summary
    success = print_summary(results)
    
    if success:
        print("\n✅ Validation completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Validation completed with failures.")
        sys.exit(1)

if __name__ == "__main__":
    main() 