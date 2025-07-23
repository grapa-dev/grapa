# CLI Integration

This guide explains how to get started with the Grapa CLI, either by downloading pre-built binaries or building from source.

## Option 1: Download Pre-Built Binaries

The fastest way to use the Grapa CLI is to download a pre-built binary for your platform from the GitHub bin directory:

- [Grapa Binaries on GitHub](https://github.com/grapa-dev/grapa/tree/main/bin)

Available binaries:
- **macOS (Intel):** `grapa-mac-amd64.tar.gz`
- **macOS (Apple Silicon):** `grapa-mac-arm64.tar.gz`
- **Linux (Intel):** `grapa-linux-amd64.tar.gz`
- **Linux (ARM):** `grapa-linux-arm64.tar.gz`
- **Windows:** `grapa-win-amd64.zip`

**Instructions:**
1. Download the appropriate file for your system.
2. Extract the archive:
   - macOS/Linux: `tar -xzf grapa-<platform>.tar.gz`
   - Windows: Use your preferred tool to extract the `.zip` file.
3. (macOS/Linux) Make the binary executable: `chmod +x grapa`
4. Move the binary to a directory in your PATH (e.g., `/usr/local/bin`) or add its location to your PATH.

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

For more details, see the [CLI Quickstart](cli_quickstart.md) and [Build System Guide](../BUILD_AND_DEPLOYMENT/BUILD_README.md).

---

**Tip:** The build system only builds for the current platform. For multi-platform builds, run the script on each target platform. 