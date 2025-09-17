# CLI Integration

This guide explains how to get started with the Grapa CLI, either by downloading pre-built binaries or building from source.

## Option 1: Universal Installer (Recommended)

The fastest way to use the Grapa CLI is to use the universal installer that automatically detects your platform:

- [Grapa Releases on GitHub](https://github.com/grapa-dev/grapa/releases)

**Universal Installer:**
- **All Platforms:** `install-grapa-0.1.55.py` - Automatically detects platform and downloads appropriate package

**Instructions:**
1. Download the universal installer:
   ```bash
   curl -O https://github.com/grapa-dev/grapa/releases/download/v0.1.55/install-grapa-0.1.55.py
   ```
2. Run the installer:
   ```bash
   python3 install-grapa-0.1.55.py
   ```
3. The installer will:
   - Detect your platform automatically
   - Download the appropriate package (10-41 MB depending on platform)
   - Install Grapa to `~/.local/grapa/`
   - Set up the environment for immediate use

## Option 2: Manual Platform-Specific Download

If you prefer to download manually, you can get the specific package for your platform:

**Available Packages:**
- `grapa-0.1.55-mac-arm64.zip` - macOS ARM64 (10.1 MB)
- `grapa-0.1.55-linux-amd64.zip` - Linux AMD64 (13.0 MB)
- `grapa-0.1.55-linux-arm64.zip` - Linux ARM64 (13.8 MB)
- `grapa-0.1.55-win-amd64.zip` - Windows AMD64 (41.3 MB)
- `grapa-0.1.55-aws-amd64.zip` - AWS AMD64 (12.4 MB)
- `grapa-0.1.55-aws-arm64.zip` - AWS ARM64 (12.8 MB)

**Instructions:**
1. Download the package for your platform from the latest release.
2. Extract the archive and navigate to the directory: `cd grapa-0.1.55`
3. Run the installer: `python3 install-grapa.py`
4. The installer will set up Grapa and add it to your PATH.

> **Note:** Grapa requires macOS 15.0+ (Apple Silicon only). Intel Macs are no longer supported due to Apple's discontinuation of Intel Mac support in macOS 15.0.

## Option 3: Build from Source

If you want to build Grapa yourself (or need the latest version), you can build from source. This will also build and install the Python extension if Python is available.

**Instructions:**
1. Clone the repository:
   ```bash
   git clone https://github.com/grapa-dev/grapa.git
   cd grapa
   ```
2. Run the build script:
   ```bash
   python3 build.py
   # or
   ./build.sh
   ```
   The script will detect your platform and build the CLI and libraries.
3. The CLI binary will be created in the appropriate directory for your platform.
4. The Python extension will also be built and installed if Python is available.

For more details, see the [CLI Quickstart](cli_quickstart.md).

---

**Tip:** The build system only builds for the current platform. For multi-platform builds, run the script on each target platform. 