# Test Installation Script for Grapa Windows amd64
# This is a test version that bypasses admin checks

param(
    [string]$InstallPath = "C:\temp\Grapa",
    [switch]$Force,
    [switch]$Uninstall,
    [switch]$Help
)

# Show help if requested
if ($Help) {
    Write-Host "Grapa Windows Installation Script (TEST VERSION)" -ForegroundColor Cyan
    Write-Host "Usage:" -ForegroundColor White
    Write-Host "  .\test-install.ps1                    # Standard installation" -ForegroundColor White
    Write-Host "  .\test-install.ps1 -Force            # Force installation (no prompts)" -ForegroundColor White
    Write-Host "  .\test-install.ps1 -Uninstall        # Uninstall Grapa" -ForegroundColor White
    Write-Host "  .\test-install.ps1 -Help             # Show this help" -ForegroundColor White
    Write-Host ""
    Write-Host "Options:" -ForegroundColor White
    Write-Host "  -InstallPath <path>                   # Custom installation path (default: C:\temp\Grapa)" -ForegroundColor White
    Write-Host "  -Force                                # Skip confirmation prompts" -ForegroundColor White
    Write-Host "  -Uninstall                            # Remove Grapa installation" -ForegroundColor White
    Write-Host "  -Help                                 # Show this help message" -ForegroundColor White
    exit 0
}

# Get the directory where this script is located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Uninstall if requested
if ($Uninstall) {
    Write-Host "🗑️  Uninstalling Grapa..." -ForegroundColor Yellow
    
    # Remove installation directory
    if (Test-Path $InstallPath) {
        Remove-Item -Path $InstallPath -Recurse -Force
        Write-Host "✅ Removed installation directory: $InstallPath" -ForegroundColor Green
    }
    
    Write-Host "✅ Uninstallation complete!" -ForegroundColor Green
    exit 0
}

# Check if files exist
$RequiredFiles = @("grapa.exe", "grapa_static.lib")
$MissingFiles = @()

foreach ($file in $RequiredFiles) {
    if (-not (Test-Path "$ScriptDir\$file")) {
        $MissingFiles += $file
    }
}

if ($MissingFiles.Count -gt 0) {
    Write-Host "❌ Missing required files:" -ForegroundColor Red
    foreach ($file in $MissingFiles) {
        Write-Host "   - $file" -ForegroundColor Red
    }
    Write-Host "Please ensure all required files are in the same directory as this script." -ForegroundColor Yellow
    exit 1
}

# Check for existing installation
if (Test-Path $InstallPath) {
    if (-not $Force) {
        Write-Host "⚠️  Grapa is already installed at: $InstallPath" -ForegroundColor Yellow
        $response = Read-Host "Do you want to replace it? (y/N)"
        if ($response -ne "y" -and $response -ne "Y") {
            Write-Host "Installation cancelled." -ForegroundColor Yellow
            exit 0
        }
    }
    Write-Host "🔄 Updating existing installation..." -ForegroundColor Yellow
} else {
    Write-Host "🆕 Installing Grapa..." -ForegroundColor Green
}

# Create installation directories
$BinDir = "$InstallPath\bin"
$LibDir = "$InstallPath\lib"

try {
    New-Item -ItemType Directory -Path $BinDir -Force | Out-Null
    New-Item -ItemType Directory -Path $LibDir -Force | Out-Null
    Write-Host "✅ Created installation directories" -ForegroundColor Green
} catch {
    Write-Host "❌ Failed to create installation directories: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# Copy files
try {
    # Copy executable to bin directory
    Copy-Item "$ScriptDir\grapa.exe" "$BinDir\" -Force
    Write-Host "✅ Copied grapa.exe to $BinDir" -ForegroundColor Green
    
    # Copy static library to lib directory
    Copy-Item "$ScriptDir\grapa_static.lib" "$LibDir\" -Force
    Write-Host "✅ Copied grapa_static.lib to $LibDir" -ForegroundColor Green
    
    # Copy DLL to bin directory if it exists (for PATH access)
    if (Test-Path "$ScriptDir\grapa.dll") {
        Copy-Item "$ScriptDir\grapa.dll" "$BinDir\" -Force
        Write-Host "✅ Copied grapa.dll to $BinDir" -ForegroundColor Green
    } else {
        Write-Host "ℹ️  grapa.dll not found (shared library not available)" -ForegroundColor Cyan
    }
} catch {
    Write-Host "❌ Failed to copy files: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# Installation complete
Write-Host ""
Write-Host "🎉 Installation complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Installation details:" -ForegroundColor Cyan
Write-Host "  Executable: $BinDir\grapa.exe" -ForegroundColor White
Write-Host "  Static Library: $LibDir\grapa_static.lib" -ForegroundColor White
if (Test-Path "$BinDir\grapa.dll") {
    Write-Host "  Shared Library: $BinDir\grapa.dll" -ForegroundColor White
}
Write-Host ""
Write-Host "To uninstall: .\test-install.ps1 -Uninstall" -ForegroundColor Cyan
