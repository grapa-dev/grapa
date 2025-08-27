# Grapa RPM Package Installation

## Quick Install

### Amazon Linux ARM64
```bash
# Download and install
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-rpm/grapa_0.1.51_arm64.rpm
sudo rpm -i grapa_0.1.51_arm64.rpm
```

### Amazon Linux AMD64
```bash
# Download and install
wget https://github.com/grapa-dev/grapa/releases/download/v0.1.51-rpm/grapa_0.1.51_amd64.rpm
sudo rpm -i grapa_0.1.51_amd64.rpm
```

## Verification

After installation, verify Grapa is working:

```bash
# Check version
grapa --version

# Test basic functionality
grapa -c "2+2"
grapa -c "'Hello World'.echo()"
```

## Uninstall

To remove Grapa:

```bash
sudo rpm -e grapa
```

## Package Information

- **Package Name**: grapa
- **Version**: 0.1.51
- **Architecture**: ARM64/AMD64
- **License**: MIT
- **Homepage**: https://github.com/grapa-dev/grapa

## Features

- Unlimited precision arithmetic
- Advanced vector and matrix operations
- Comprehensive cryptography with OpenSSL 3.5.2
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy

## Documentation

For more information, visit: https://grapa-dev.github.io/grapa/
