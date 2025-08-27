# Grapa Amazon Linux Repository Submission

## Package Information
- **Package Name**: grapa
- **Version**: 0.1.51
- **Architecture**: ARM64, AMD64
- **License**: MIT
- **Homepage**: https://github.com/grapa-dev/grapa

## Description
Grapa is a modern, high-performance programming language with unlimited precision arithmetic, advanced vector operations, and comprehensive cryptographic capabilities.

## Features
- Unlimited precision arithmetic for integers and floating-point numbers
- Advanced vector and matrix operations with optimized algorithms
- Comprehensive cryptography with OpenSSL 3.5.2 integration
- Machine learning capabilities including linear regression
- Unicode string support and advanced text processing
- Cross-platform compatibility (Linux, macOS, Windows)
- Python integration via GrapaPy
- Command-line interface with script execution
- Database integration with unified file/database API

## Technical Details
- **Dependencies**: None (static binary)
- **Build System**: FLTK 1.4.4, OpenSSL 3.5.2
- **Target Platforms**: Amazon Linux 2, Amazon Linux 2023
- **Architectures**: ARM64 (Graviton), AMD64 (x86_64)

## Quality Assurance
- ✅ Package builds successfully on target architectures
- ✅ Package installs and uninstalls correctly
- ✅ Binary executes and responds to commands
- ✅ Basic functionality tests pass
- ✅ No external dependencies required

## Installation Instructions
```bash
# For ARM64 (Graviton)
sudo rpm -i grapa_0.1.51_arm64.rpm

# For AMD64 (x86_64)
sudo rpm -i grapa_0.1.51_amd64.rpm
```

## Verification
```bash
grapa --version
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

## Project Information
- **Repository**: https://github.com/grapa-dev/grapa
- **Documentation**: https://grapa-dev.github.io/grapa/
- **License**: MIT
- **Maintainer**: Grapa Team <team@grapa-dev>

## Release Notes
- Initial release of Grapa programming language
- Features unlimited precision arithmetic
- Advanced vector and matrix operations
- Comprehensive cryptography with OpenSSL 3.5.2
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy
