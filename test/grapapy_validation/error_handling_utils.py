#!/usr/bin/env python3
"""
GrapaPy Error Handling Utilities
Provides comprehensive error handling for GrapaPy operations
"""

import json
import logging
from typing import Any, Dict, Optional, Union, Callable
from functools import wraps

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class GrapaPyError(Exception):
    """Custom exception for GrapaPy errors"""
    def __init__(self, message: str, error_code: int = -1, operation: str = "", context: Dict = None):
        self.message = message
        self.error_code = error_code
        self.operation = operation
        self.context = context or {}
        super().__init__(self.message)

class GrapaPyErrorHandler:
    """Comprehensive error handler for GrapaPy operations"""
    
    def __init__(self, grapa_instance):
        self.grapa = grapa_instance
        self.error_count = 0
        self.last_error = None
    
    def is_error_response(self, result: Any) -> bool:
        """Check if result is an error response"""
        if isinstance(result, dict):
            return result.get("error") == -1
        return False
    
    def safe_eval(self, code: str, variables: Dict = None, fallback: Any = None) -> Any:
        """
        Safely evaluate Grapa code with error handling
        
        Args:
            code: Grapa code to execute
            variables: Variables to pass to the execution context
            fallback: Value to return if execution fails
            
        Returns:
            Result of execution or fallback value
        """
        try:
            result = self.grapa.eval(code, variables or {})
            
            if self.is_error_response(result):
                self.error_count += 1
                self.last_error = {
                    "code": code,
                    "result": result,
                    "variables": variables
                }
                logger.warning(f"GrapaPy error response: {result} for code: {code}")
                return fallback
            
            return result
            
        except Exception as e:
            self.error_count += 1
            self.last_error = {
                "code": code,
                "exception": str(e),
                "variables": variables
            }
            logger.error(f"GrapaPy exception: {e} for code: {code}")
            return fallback
    
    def safe_file_operation(self, operation: str, filename: str, content: str = None) -> Any:
        """
        Safely perform file operations with error handling
        
        Args:
            operation: File operation ('get', 'set', 'remove', 'info')
            filename: Name of the file
            content: Content for 'set' operations
            
        Returns:
            Result of operation or None if failed
        """
        if operation == "get":
            result = self.safe_eval(f"fs.get('{filename}');")
            if result and isinstance(result, bytes):
                try:
                    return result.decode('utf-8')
                except UnicodeDecodeError:
                    logger.warning(f"Failed to decode file content for {filename}")
                    return None
            return result
            
        elif operation == "set":
            if content is None:
                logger.error("Content required for set operation")
                return None
            return self.safe_eval(f"fs.set('{filename}', content);", {"content": content})
            
        elif operation == "remove":
            return self.safe_eval(f"fs.remove('{filename}');")
            
        elif operation == "info":
            return self.safe_eval(f"fs.info('{filename}');")
            
        else:
            logger.error(f"Unknown file operation: {operation}")
            return None
    
    def retry_operation(self, operation: Callable, max_retries: int = 3, delay: float = 0.1) -> Any:
        """
        Retry an operation with exponential backoff
        
        Args:
            operation: Function to retry
            max_retries: Maximum number of retry attempts
            delay: Initial delay between retries
            
        Returns:
            Result of operation or None if all retries failed
        """
        import time
        
        for attempt in range(max_retries):
            try:
                result = operation()
                if not self.is_error_response(result):
                    return result
                    
                logger.warning(f"Operation failed with error response, attempt {attempt + 1}/{max_retries}")
                
            except Exception as e:
                logger.warning(f"Operation failed with exception, attempt {attempt + 1}/{max_retries}: {e}")
            
            if attempt < max_retries - 1:
                time.sleep(delay * (2 ** attempt))  # Exponential backoff
        
        return None
    
    def get_error_summary(self) -> Dict:
        """Get summary of errors encountered"""
        return {
            "error_count": self.error_count,
            "last_error": self.last_error,
            "has_errors": self.error_count > 0
        }

def error_handler(max_retries: int = 3):
    """
    Decorator for automatic error handling and retry logic
    
    Args:
        max_retries: Maximum number of retry attempts
    """
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            handler = None
            
            # Find GrapaPyErrorHandler in args
            for arg in args:
                if isinstance(arg, GrapaPyErrorHandler):
                    handler = arg
                    break
            
            if not handler:
                # Create a new handler if none provided
                for arg in args:
                    if hasattr(arg, 'eval'):  # GrapaPy instance
                        handler = GrapaPyErrorHandler(arg)
                        break
            
            if handler:
                return handler.retry_operation(lambda: func(*args, **kwargs), max_retries)
            else:
                return func(*args, **kwargs)
        
        return wrapper
    return decorator

def validate_grapapy_installation() -> bool:
    """
    Validate GrapaPy installation and basic functionality
    
    Returns:
        True if installation is valid, False otherwise
    """
    try:
        import grapapy
        
        # Test basic functionality
        xy = grapapy.grapa()
        result = xy.eval("2 + 2;")
        
        if result == 4:
            logger.info("GrapaPy installation validated successfully")
            return True
        else:
            logger.error(f"GrapaPy basic test failed: expected 4, got {result}")
            return False
            
    except ImportError as e:
        logger.error(f"GrapaPy import failed: {e}")
        return False
    except Exception as e:
        logger.error(f"GrapaPy validation failed: {e}")
        return False

def create_error_report(handler: GrapaPyErrorHandler) -> str:
    """
    Create a detailed error report
    
    Args:
        handler: GrapaPyErrorHandler instance
        
    Returns:
        Formatted error report string
    """
    summary = handler.get_error_summary()
    
    if not summary["has_errors"]:
        return "✅ No errors encountered"
    
    report = f"""
❌ Error Report
==============
Total Errors: {summary['error_count']}
Last Error: {json.dumps(summary['last_error'], indent=2)}
"""
    
    return report

# Example usage and testing
if __name__ == "__main__":
    print("Testing GrapaPy Error Handling Utilities")
    print("=" * 50)
    
    # Test installation validation
    if not validate_grapapy_installation():
        print("❌ GrapaPy installation validation failed")
        exit(1)
    
    print("✅ GrapaPy installation validated")
    
    # Test error handler
    import grapapy
    xy = grapapy.grapa()
    handler = GrapaPyErrorHandler(xy)
    
    # Test safe evaluation
    result = handler.safe_eval("1 + 1;")
    print(f"Safe eval result: {result}")
    
    # Test error detection
    error_result = handler.safe_eval("fs.get('nonexistent.txt');")
    print(f"Error result: {error_result}")
    
    # Test error summary
    summary = handler.get_error_summary()
    print(f"Error summary: {summary}")
    
    print("✅ Error handling utilities test completed") 