# ONNX Runtime Shared Libraries for mac-arm64

This directory contains ONNX Runtime shared libraries downloaded from the official Microsoft releases.

## Files:
- libonnxruntime.1.23.0.dylib
- libonnxruntime.dylib

## Usage:
- The Grapa application will link against these shared libraries at runtime
- Users must have ONNX Runtime installed on their system
- If ONNX Runtime is not available, the .load() method for "onnx" will fail gracefully

## Installation Requirements:
- macOS: No additional installation required (libraries included)
- Linux: May require additional dependencies (libprotobuf, etc.)
- Windows: May require Visual C++ Redistributable

## Runtime Dependencies:
- The shared libraries must be available in the system library path
- For development, ensure the libraries are in the same directory as the executable
- For production, consider bundling the libraries with the application
