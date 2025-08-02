#!/usr/bin/env python3
"""
Test script to verify GrapaPy installation
"""

try:
    import grapapy
    print("✅ GrapaPy imported successfully!")
    
    # Test basic functionality
    try:
        result = grapapy.eval("2 + 2")
        print(f"✅ Basic eval test: 2 + 2 = {result}")
    except Exception as e:
        print(f"❌ Eval test failed: {e}")
    
    # Test more complex functionality
    try:
        result = grapapy.eval("'Hello ' + 'World'")
        print(f"✅ String eval test: 'Hello ' + 'World' = {result}")
    except Exception as e:
        print(f"❌ String eval test failed: {e}")
        
    print("🎉 GrapaPy installation test completed successfully!")
    
except ImportError as e:
    print(f"❌ Failed to import grapapy: {e}")
except Exception as e:
    print(f"❌ Unexpected error: {e}") 