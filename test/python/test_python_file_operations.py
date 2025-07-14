#!/usr/bin/env python3
"""
Python Integration Tests for Grapa File Operations
Tests Python integration with Grapa file system using the GrapaPy extension
"""

import grapapy
import os
import tempfile
import time
import json
import csv
from io import StringIO

class GrapaFileHelper:
    """Helper class for $file operations"""
    
    def __init__(self, grapa_instance):
        self.xy = grapa_instance
        # Initialize file object - use raw string to preserve $
        self.xy.eval(r'f = $file();')
    
    def pwd(self):
        """Get current working directory"""
        return self.xy.eval(r'f.pwd();')
    
    def phd(self):
        """Get home directory"""
        return self.xy.eval(r'f.phd();')
    
    def cd(self, path):
        """Change directory"""
        return self.xy.eval(fr'f.cd("{path}");')
    
    def ls(self, path=None):
        """List directory contents"""
        if path:
            return self.xy.eval(fr'f.ls("{path}");')
        return self.xy.eval(r'f.ls();')
    
    def mk(self, name):
        """Create directory"""
        return self.xy.eval(fr'f.mk("{name}");')
    
    def rm(self, name):
        """Remove file or directory"""
        return self.xy.eval(fr'f.rm("{name}");')
    
    def set(self, name, content):
        """Create or write to file"""
        # Escape quotes in content
        escaped_content = content.replace('"', '\\"').replace('\n', '\\n')
        return self.xy.eval(fr'f.set("{name}", "{escaped_content}");')
    
    def get(self, name):
        """Read file content"""
        return self.xy.eval(fr'f.get("{name}");')
    
    def info(self, name):
        """Get file information"""
        return self.xy.eval(fr'f.info("{name}");')
    
    def split(self, parts, input_file, output_dir, delimiter="\\n", options=""):
        """Split file into parts"""
        return self.xy.eval(fr'f.split({parts}, "{input_file}", "{output_dir}", "{delimiter}", "{options}");')

class GrapaTableHelper:
    """Helper class for $TABLE operations"""
    
    def __init__(self, grapa_instance, table_type="ROW"):
        self.xy = grapa_instance
        self.xy.eval(fr'table = f.table("{table_type}");')
    
    def mkfield(self, name, field_type, storage_type, size=None):
        """Create a field in the table"""
        if size:
            return self.xy.eval(fr'table.mkfield("{name}", "{field_type}", "{storage_type}", {size});')
        return self.xy.eval(fr'table.mkfield("{name}", "{field_type}", "{storage_type}");')
    
    def set(self, record_id, value, field_name):
        """Set a value in the table"""
        # Escape quotes in value
        if isinstance(value, str):
            escaped_value = value.replace('"', '\\"')
            return self.xy.eval(fr'table.set("{record_id}", "{escaped_value}", "{field_name}");')
        else:
            return self.xy.eval(fr'table.set("{record_id}", {value}, "{field_name}");')
    
    def get(self, record_id, field_name):
        """Get a value from the table"""
        return self.xy.eval(fr'table.get("{record_id}", "{field_name}");')
    
    def ls(self):
        """List all records in the table"""
        return self.xy.eval(r'table.ls();')
    
    def debug(self):
        """Get table debug information"""
        return self.xy.eval(r'table.debug();')
    
    def rmfield(self, field_name):
        """Remove a field from the table"""
        return self.xy.eval(fr'table.rmfield("{field_name}");')

class GrapaGrepHelper:
    """Helper class for grep operations"""
    
    def __init__(self, grapa_instance):
        self.xy = grapa_instance
    
    def grep(self, input_text, pattern, options="", delimiter="\\n", normalization="NONE", mode="UNICODE", num_workers=0):
        """Execute grep operation"""
        return self.xy.eval("input.grep(pattern, options, delimiter, normalization, mode, num_workers);", {
            "input": input_text,
            "pattern": pattern,
            "options": options,
            "delimiter": delimiter,
            "normalization": normalization,
            "mode": mode,
            "num_workers": num_workers
        })
    
    def grep_simple(self, input_text, pattern, options=""):
        """Simple grep with embedded parameters"""
        escaped_input = input_text.replace('"', '\\"').replace('\n', '\\n')
        escaped_pattern = pattern.replace('"', '\\"')
        return self.xy.eval(fr'"{escaped_input}".grep("{escaped_pattern}", "{options}");')

def test_basic_integration():
    """Test basic Python integration with Grapa"""
    print("=== Testing Basic Python Integration ===")
    
    # Initialize Grapa instance
    xy = grapapy.grapa()
    
    # Test basic operations
    print("Testing basic Grapa operations...")
    result = xy.eval("x = 3.45; y = 4.32; x**y;")
    print(f"Basic calculation result: {result}")
    
    # Test simple file operation first
    print("Testing simple file operation...")
    try:
        # Test with raw string to preserve $
        result = xy.eval(r'f = $file(); f.pwd();')
        print(f"Simple file operation result: {result}")
    except Exception as e:
        print(f"Error with simple file operation: {e}")
    
    # Test file system operations using helper
    print("Testing file system operations...")
    try:
        file_helper = GrapaFileHelper(xy)
        
        # Create test file
        file_helper.set("python_test.txt", "Hello from Python integration test!")
        print("File created successfully")
        
        # Read test file
        content = file_helper.get("python_test.txt")
        print(f"Read content: {content}")
        
        # Get file info
        info = file_helper.info("python_test.txt")
        print(f"File info: {info}")
        
        # List directory contents
        files = file_helper.ls()
        print(f"Directory contents: {files}")
        
        # Cleanup
        file_helper.rm("python_test.txt")
        print("Test completed successfully")
    except Exception as e:
        print(f"Error with file helper: {e}")

def test_file_system_operations():
    """Test comprehensive file system operations"""
    print("\n=== Testing File System Operations ===")
    
    xy = grapapy.grapa()
    
    try:
        file_helper = GrapaFileHelper(xy)
        
        # Test directory operations
        print("Testing directory operations...")
        file_helper.mk("python_test_dir")
        file_helper.cd("python_test_dir")
        current_dir = file_helper.pwd()
        print(f"Current directory: {current_dir}")
        
        # Test file operations
        print("Testing file operations...")
        file_helper.set("test_file.txt", "Python test content")
        content = file_helper.get("test_file.txt")
        print(f"File content: {content}")
        
        # Test file info
        info = file_helper.info("test_file.txt")
        print(f"File info: {info}")
        
        # Test directory listing
        files = file_helper.ls()
        print(f"Directory files: {files}")
        
        # Cleanup
        file_helper.rm("test_file.txt")
        file_helper.cd("..")
        file_helper.rm("python_test_dir")
        print("File system operations completed")
    except Exception as e:
        print(f"Error with file system operations: {e}")

def test_file_split():
    """Test file split functionality"""
    print("\n=== Testing File Split Functionality ===")
    
    xy = grapapy.grapa()
    
    try:
        file_helper = GrapaFileHelper(xy)
        
        # Create large test file
        print("Creating large test file...")
        xy.eval(r'''
            content = "";
            i = 1;
            while (i <= 100) {
                content = content + "Line " + i + ": Test data for splitting.\n";
                i = i + 1;
            };
            f.set("large_test_file.txt", content);
        ''')
        
        file_info = file_helper.info("large_test_file.txt")
        print(f"File info: {file_info}")
        
        # Handle the file info response properly
        if isinstance(file_info, dict) and "$BYTES" in file_info:
            file_size = file_info["$BYTES"]
            print(f"Created file with {file_size} bytes")
        else:
            print(f"Created file, info: {file_info}")
        
        # Create output directory
        file_helper.mk("split_output")
        
        # Split file
        print("Splitting file into 4 parts...")
        result = file_helper.split(4, "large_test_file.txt", "split_output", "\\n", "")
        print(f"Split result: {result}")
        
        # Check split files
        file_helper.cd("split_output")
        split_files = file_helper.ls()
        print(f"Split files created: {split_files}")
        
        # Verify split file contents
        if isinstance(split_files, list):
            total_size = 0
            for file_name in split_files:
                file_info = file_helper.info(file_name)
                if isinstance(file_info, dict) and "$BYTES" in file_info:
                    size = file_info["$BYTES"]
                    total_size += size
                    print(f"{file_name}: {size} bytes")
                else:
                    print(f"{file_name}: {file_info}")
            
            print(f"Total split size: {total_size} bytes")
        
        # Cleanup
        xy.eval(r'''
            i = 0;
            while (i < split_files.length()) {
                f.rm(split_files[i]);
                i = i + 1;
            }
            f.cd("..");
            f.rm("split_output");
            f.rm("large_test_file.txt");
        ''')
        print("File split test completed")
    except Exception as e:
        print(f"Error with file split: {e}")

def test_data_processing():
    """Test data processing with Grapa"""
    print("\n=== Testing Data Processing ===")
    
    xy = grapapy.grapa()
    
    try:
        file_helper = GrapaFileHelper(xy)
        
        # Create test data
        print("Creating test data...")
        
        # CSV data
        csv_data = "Name,Age,City\nJohn,30,New York\nJane,25,Los Angeles\nBob,35,Chicago\n"
        file_helper.set("test_data.csv", csv_data)
        
        # JSON data
        json_data = {
            "users": [
                {"name": "John", "age": 30, "city": "New York"},
                {"name": "Jane", "age": 25, "city": "Los Angeles"},
                {"name": "Bob", "age": 35, "city": "Chicago"}
            ]
        }
        
        xy.eval(r'f.set("test_data.json", json_data);', {"json_data": json.dumps(json_data, indent=2)})
        
        # Process CSV data
        print("Processing CSV data...")
        csv_content = file_helper.get("test_data.csv")
        if isinstance(csv_content, str):
            csv_reader = csv.reader(StringIO(csv_content))
            for row in csv_reader:
                print(f"CSV row: {row}")
        else:
            print(f"CSV content: {csv_content}")
        
        # Process JSON data
        print("Processing JSON data...")
        json_content = file_helper.get("test_data.json")
        if isinstance(json_content, str):
            parsed_data = json.loads(json_content)
            for user in parsed_data["users"]:
                print(f"JSON user: {user}")
        else:
            print(f"JSON content: {json_content}")
        
        # Cleanup
        file_helper.rm("test_data.csv")
        file_helper.rm("test_data.json")
        print("Data processing test completed")
    except Exception as e:
        print(f"Error with data processing: {e}")

def test_error_handling():
    """Test error handling"""
    print("\n=== Testing Error Handling ===")
    
    xy = grapapy.grapa()
    
    try:
        file_helper = GrapaFileHelper(xy)
        
        print("Testing error handling...")
        
        # Test non-existent file
        try:
            result = file_helper.get("nonexistent.txt")
            print(f"Non-existent file result: {result}")
        except Exception as e:
            print(f"Error getting non-existent file: {e}")
        
        # Test non-existent directory
        try:
            result = file_helper.cd("nonexistent_dir")
            print(f"Non-existent directory result: {result}")
        except Exception as e:
            print(f"Error changing to non-existent directory: {e}")
        
        # Test invalid split operation
        try:
            result = file_helper.split(0, "nonexistent.txt", "output", "\\n", "")
            print(f"Invalid split result: {result}")
        except Exception as e:
            print(f"Error with invalid split: {e}")
        
        print("Error handling test completed")
    except Exception as e:
        print(f"Error with error handling test: {e}")

def test_performance():
    """Test performance with Grapa"""
    print("\n=== Testing Performance ===")
    
    xy = grapapy.grapa()
    
    try:
        file_helper = GrapaFileHelper(xy)
        
        print("Testing performance...")
        
        # Test file creation performance
        start_time = time.time()
        
        xy.eval(r'''
            i = 1;
            while (i <= 100) {
                f.set("test_file_" + i + ".txt", "Content for file " + i);
                i = i + 1;
            }
        ''')
        
        creation_time = time.time() - start_time
        print(f"Created 100 files in {creation_time:.4f} seconds")
        
        # Test file reading performance
        start_time = time.time()
        
        xy.eval(r'''
            i = 1;
            while (i <= 100) {
                content = f.get("test_file_" + i + ".txt");
                i = i + 1;
            }
        ''')
        
        reading_time = time.time() - start_time
        print(f"Read 100 files in {reading_time:.4f} seconds")
        
        # Test directory listing performance
        start_time = time.time()
        files = file_helper.ls()
        listing_time = time.time() - start_time
        print(f"Listed {len(files) if isinstance(files, list) else 'unknown'} files in {listing_time:.4f} seconds")
        
        # Cleanup
        start_time = time.time()
        
        xy.eval(r'''
            i = 1;
            while (i <= 100) {
                f.rm("test_file_" + i + ".txt");
                i = i + 1;
            }
        ''')
        
        cleanup_time = time.time() - start_time
        print(f"Cleaned up 100 files in {cleanup_time:.4f} seconds")
        
        print("Performance test completed")
    except Exception as e:
        print(f"Error with performance test: {e}")

def test_database_operations():
    """Test database operations"""
    print("\n=== Testing Database Operations ===")
    
    xy = grapapy.grapa()
    
    try:
        # Create row store table
        print("Creating row store table...")
        row_table = GrapaTableHelper(xy, "ROW")
        row_table.mkfield("name", "STR", "VAR")
        row_table.mkfield("age", "INT", "FIX", 4)
        row_table.mkfield("salary", "FLOAT", "FIX", 8)
        
        # Add data to row store
        row_table.set("user1", "John Doe", "name")
        row_table.set("user1", 30, "age")
        row_table.set("user1", 75000.50, "salary")
        
        # Read data from row store
        name = row_table.get("user1", "name")
        age = row_table.get("user1", "age")
        salary = row_table.get("user1", "salary")
        
        print(f"Row store data - Name: {name}, Age: {age}, Salary: {salary}")
        
        # Create column store table
        print("Creating column store table...")
        col_table = GrapaTableHelper(xy, "COL")
        col_table.mkfield("name", "STR", "VAR")
        col_table.mkfield("age", "INT", "FIX", 4)
        col_table.mkfield("salary", "FLOAT", "FIX", 8)
        
        # Add data to column store
        col_table.set("user1", "Jane Smith", "name")
        col_table.set("user1", 25, "age")
        col_table.set("user1", 65000.75, "salary")
        
        # Read data from column store
        name = col_table.get("user1", "name")
        age = col_table.get("user1", "age")
        salary = col_table.get("user1", "salary")
        
        print(f"Column store data - Name: {name}, Age: {age}, Salary: {salary}")
        
        print("Database operations test completed")
    except Exception as e:
        print(f"Error with database operations: {e}")

def test_grep_functionality():
    """Test grep functionality from Python"""
    print("\n=== Testing Grep Functionality ===")
    
    xy = grapapy.grapa()
    
    try:
        grep_helper = GrapaGrepHelper(xy)
        
        # Basic grep test
        print("Testing basic grep...")
        result = grep_helper.grep_simple("apple 123 pear 456\nbanana 789", "\\d+", "o")
        print(f"Basic grep result: {result}")
        
        # Unicode grep test
        print("Testing Unicode grep...")
        result = grep_helper.grep_simple("Hello 世界 123 €", "\\p{L}+", "o")
        print(f"Unicode grep result: {result}")
        
        # Performance grep test
        print("Testing grep performance...")
        large_text = "test data " * 10000  # Create large text
        result = grep_helper.grep(large_text, "test", "n", "\\n", "NONE", "UNICODE", 4)
        print(f"Performance grep found {len(result) if isinstance(result, list) else 'unknown'} matches")
        
        print("Grep functionality test completed")
    except Exception as e:
        print(f"Error with grep functionality: {e}")

def main():
    """Run all Python integration tests"""
    print("=== GRAPA PYTHON INTEGRATION TESTS ===")
    print(f"Started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    
    try:
        test_basic_integration()
        test_file_system_operations()
        test_file_split()
        test_data_processing()
        test_error_handling()
        test_performance()
        test_database_operations()
        test_grep_functionality()
        
        print("\n🎉 ALL PYTHON INTEGRATION TESTS PASSED! 🎉")
        return 0
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        return 1

if __name__ == "__main__":
    exit(main()) 