# ONNX Runtime Shared Libraries - aws-arm64

This directory contains ONNX Runtime shared libraries for aws-arm64.

## Files
- `libonnxruntime.1.23.0.dylib` - Versioned shared library
- `libonnxruntime.dylib` - Unversioned symlink/shim

## Usage
These libraries are automatically linked by the Grapa build system.
They must be available at runtime for ONNX functionality to work.

## Installation
The libraries are automatically copied to the installation directory
when using the Grapa installer.
