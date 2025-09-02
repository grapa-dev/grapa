# Grapa Installation Script for Windows amd64
# Run this script as Administrator

param(
    [string]$InstallPath = "C:\Program Files\Grapa",
    [switch]$Force,
    [switch]$Uninstall,
    [switch]$Help
)

# Show help if requested
if ($Help) {
    Write-Host "Grapa Windows Installation Script" -ForegroundColor Cyan
    Write-Host "Usage:" -ForegroundColor White
    Write-Host "  .\install-grapa.ps1                    # Standard installation" -ForegroundColor White
    Write-Host "  .\install-grapa.ps1 -Force            # Force installation (no prompts)" -ForegroundColor White
    Write-Host "  .\install-grapa.ps1 -Uninstall        # Uninstall Grapa" -ForegroundColor White
    Write-Host "  .\install-grapa.ps1 -Help             # Show this help" -ForegroundColor White
    Write-Host ""
    Write-Host "Options:" -ForegroundColor White
    Write-Host "  -InstallPath <path>                   # Custom installation path (default: C:\Program Files\Grapa)" -ForegroundColor White
    Write-Host "  -Force                                # Skip confirmation prompts" -ForegroundColor White
    Write-Host "  -Uninstall                            # Remove Grapa installation" -ForegroundColor White
    Write-Host "  -Help                                 # Show this help message" -ForegroundColor White
    exit 0
}

# Get the directory where this script is located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Check if running as Administrator
if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "❌ This script must be run as Administrator!" -ForegroundColor Red
    Write-Host "Please right-click PowerShell and select 'Run as Administrator'" -ForegroundColor Yellow
    exit 1
}

# Uninstall if requested
if ($Uninstall) {
    Write-Host "🗑️  Uninstalling Grapa..." -ForegroundColor Yellow
    
    # Remove from PATH
    $CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
    if ($CurrentPath -like "*$InstallPath*") {
        $NewPath = ($CurrentPath -split ';' | Where-Object { $_ -ne $InstallPath }) -join ';'
        [Environment]::SetEnvironmentVariable("PATH", $NewPath, "Machine")
        Write-Host "✅ Removed Grapa from system PATH" -ForegroundColor Green
    }
    
    # Remove installation directory
    if (Test-Path $InstallPath) {
        Remove-Item -Path $InstallPath -Recurse -Force
        Write-Host "✅ Removed installation directory: $InstallPath" -ForegroundColor Green
    }
    
    Write-Host "✅ Uninstallation complete!" -ForegroundColor Green
    Write-Host "Please restart your terminal for PATH changes to take effect." -ForegroundColor Yellow
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
    Write-Host "🔄 Replacing existing installation..." -ForegroundColor Yellow
    
    # Remove old installation completely to ensure clean replacement
    try {
        Remove-Item -Path $InstallPath -Recurse -Force
        Write-Host "✅ Removed old installation" -ForegroundColor Green
    } catch {
        Write-Host "❌ Failed to remove old installation: $($_.Exception.Message)" -ForegroundColor Red
        Write-Host "Please manually remove $InstallPath and try again" -ForegroundColor Yellow
        exit 1
    }
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
    
    # Clean up any DLL files that might be in the wrong location from previous installations
    if (Test-Path "$BinDir\grapa.dll") {
        Remove-Item -Path "$BinDir\grapa.dll" -Force
        Write-Host "🧹 Cleaned up old DLL from bin directory" -ForegroundColor Cyan
    }
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
    
    # Copy DLL to lib directory if it exists (shared library for runtime linking)
    if (Test-Path "$ScriptDir\grapa.dll") {
        Copy-Item "$ScriptDir\grapa.dll" "$LibDir\" -Force
        Write-Host "✅ Copied grapa.dll to $LibDir" -ForegroundColor Green
    } else {
        Write-Host "ℹ️  grapa.dll not found (shared library not available)" -ForegroundColor Cyan
    }
} catch {
    Write-Host "❌ Failed to copy files: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# Add to PATH
try {
    $CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
    if ($CurrentPath -notlike "*$BinDir*") {
        $NewPath = "$CurrentPath;$BinDir"
        [Environment]::SetEnvironmentVariable("PATH", $NewPath, "Machine")
        Write-Host "✅ Added Grapa to system PATH" -ForegroundColor Green
    } else {
        Write-Host "ℹ️  Grapa already in system PATH" -ForegroundColor Cyan
    }
} catch {
    Write-Host "❌ Failed to update PATH: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "You may need to manually add $BinDir to your PATH" -ForegroundColor Yellow
}

# Installation complete
Write-Host ""
Write-Host "🎉 Installation complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Installation details:" -ForegroundColor Cyan
Write-Host "  Executable: $BinDir\grapa.exe" -ForegroundColor White
Write-Host "  Static Library: $LibDir\grapa_static.lib" -ForegroundColor White
if (Test-Path "$LibDir\grapa.dll") {
    Write-Host "  Shared Library: $LibDir\grapa.dll" -ForegroundColor White
}
Write-Host "  System PATH: Updated" -ForegroundColor White
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Restart your terminal for PATH changes to take effect" -ForegroundColor White
Write-Host "  2. Verify installation: grapa --version" -ForegroundColor White
Write-Host "  3. Test functionality: grapa -c '2+2'" -ForegroundColor White
Write-Host ""
Write-Host "To uninstall: .\install-grapa.ps1 -Uninstall" -ForegroundColor Cyan
