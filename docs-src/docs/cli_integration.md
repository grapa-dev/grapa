# CLI Integration

This guide explains how to get started with the Grapa CLI, either by downloading pre-built binaries or building from source.

## Option 1: Download Pre-Built Binaries

The fastest way to use the Grapa CLI is to download a pre-built package for your platform from GitHub releases:

- [Grapa Releases on GitHub](https://github.com/grapa-dev/grapa/releases)

Available packages:
- **macOS (Apple Silicon):** `grapa-0.1.52-mac-arm64.zip`
- **Linux (Intel):** `grapa-0.1.52-linux-amd64.zip`
- **Linux (ARM):** `grapa-0.1.52-linux-arm64.zip`
- **Windows:** `grapa-0.1.52-win-amd64.zip`
- **AWS (Intel):** `grapa-0.1.52-aws-amd64.zip`
- **AWS (ARM):** `grapa-0.1.52-aws-arm64.zip`

**Instructions:**
1. Download the appropriate package for your system from the latest release.
2. Extract the archive:
   - All platforms: Use `unzip` or your preferred tool to extract the `.zip` file.
3. Navigate to the extracted directory: `cd grapa-0.1.52-<platform>-<arch>`
4. Run the automated install script:
   - macOS/Linux/AWS: `./install-grapa.sh`
   - Windows: `.\install-grapa.ps1` (run as Administrator)
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