# CLI Integration

This guide explains how to get started with the Grapa CLI, either by downloading pre-built binaries or building from source.

## Option 1: Download Universal Development Kit

The fastest way to use the Grapa CLI is to download the universal development kit from GitHub releases:

- [Grapa Releases on GitHub](https://github.com/grapa-dev/grapa/releases)

**Universal Package:**
- **All Platforms:** `grapa-0.1.53.zip` - Universal development kit containing everything for all platforms

**Instructions:**
1. Download the universal package from the latest release.
2. Extract the archive:
   - All platforms: Use `unzip` or your preferred tool to extract the `.zip` file.
3. Navigate to the extracted directory: `cd grapa-0.1.53`
4. Run the universal installer:
   - All platforms: `python3 install-grapa.py` (or `python install-grapa.py` on Windows)
5. The installer will automatically add Grapa to your PATH and set up the necessary files.

> **Note:** Grapa requires macOS 15.0+ (Apple Silicon only). Intel Macs are no longer supported due to Apple's discontinuation of Intel Mac support in macOS 15.0.

## Option 2: Build from Source

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