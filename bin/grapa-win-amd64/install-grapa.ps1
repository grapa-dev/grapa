# Grapa Installation Script for Windows
# This script installs Grapa to C:\Program Files\Grapa\

param(
    [switch]$Uninstall,
    [switch]$Help,
    [switch]$Force
)

function Show-Help {
    Write-Host "Grapa Installation Script" -ForegroundColor Green
    Write-Host "=========================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Usage:" -ForegroundColor Yellow
    Write-Host "  .\install-grapa.ps1                    # Install Grapa"
    Write-Host "  .\install-grapa.ps1 -Uninstall         # Uninstall Grapa"
    Write-Host "  .\install-grapa.ps1 -Force             # Install without confirmation"
    Write-Host "  .\install-grapa.ps1 -Help              # Show this help"
    Write-Host ""
    Write-Host "Requirements:" -ForegroundColor Yellow
    Write-Host "  - Run PowerShell as Administrator"
    Write-Host "  - Windows 10 or later"
    Write-Host "  - grapa.exe and grapa.lib must be in the same directory as this script"
    Write-Host ""
}

function Get-GrapaVersion {
    param([string]$ExePath)
    try {
        $version = & $ExePath --version 2>&1
        return $version.Trim()
    } catch {
        return "Unknown"
    }
}

function Install-Grapa {
    Write-Host "Installing Grapa..." -ForegroundColor Green
    
    # Check if running as administrator
    if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
        Write-Host "Error: This script must be run as Administrator" -ForegroundColor Red
        Write-Host "Please right-click PowerShell and select 'Run as Administrator'" -ForegroundColor Yellow
        exit 1
    }
    
    # Check if required files exist
    if (-not (Test-Path "grapa.exe")) {
        Write-Host "Error: grapa.exe not found in current directory" -ForegroundColor Red
        Write-Host "Please ensure grapa.exe is in the same directory as this script" -ForegroundColor Yellow
        exit 1
    }
    
    if (-not (Test-Path "grapa.lib")) {
        Write-Host "Error: grapa.lib not found in current directory" -ForegroundColor Red
        Write-Host "Please ensure grapa.lib is in the same directory as this script" -ForegroundColor Yellow
        exit 1
    }
    
    # Define installation paths
    $installDir = "C:\Program Files\Grapa"
    $binDir = Join-Path $installDir "bin"
    $libDir = Join-Path $installDir "lib"
    $existingExe = Join-Path $binDir "grapa.exe"
    
    # Check for existing installation
    if (Test-Path $existingExe) {
        $existingVersion = Get-GrapaVersion $existingExe
        $newVersion = Get-GrapaVersion "grapa.exe"
        
        Write-Host "Existing Grapa installation detected:" -ForegroundColor Yellow
        Write-Host "  Current version: $existingVersion" -ForegroundColor White
        Write-Host "  New version: $newVersion" -ForegroundColor White
        Write-Host ""
        
        if (-not $Force) {
            $response = Read-Host "Do you want to replace the existing installation? (y/N)"
            if ($response -notmatch "^[Yy]$") {
                Write-Host "Installation cancelled." -ForegroundColor Yellow
                exit 0
            }
        }
        
        Write-Host "Replacing existing installation..." -ForegroundColor Yellow
    }
    
    try {
        # Create installation directories
        Write-Host "Creating installation directories..." -ForegroundColor Yellow
        New-Item -ItemType Directory -Force -Path $installDir | Out-Null
        New-Item -ItemType Directory -Force -Path $binDir | Out-Null
        New-Item -ItemType Directory -Force -Path $libDir | Out-Null
        
        # Copy files
        Write-Host "Copying files..." -ForegroundColor Yellow
        Copy-Item -Path "grapa.exe" -Destination $binDir -Force
        Copy-Item -Path "grapa.lib" -Destination $libDir -Force
        
        # Add to PATH
        Write-Host "Adding to system PATH..." -ForegroundColor Yellow
        $currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
        if ($currentPath -notlike "*$binDir*") {
            [Environment]::SetEnvironmentVariable('PATH', $currentPath + ";" + $binDir, 'Machine')
            Write-Host "Added $binDir to system PATH" -ForegroundColor Green
        } else {
            Write-Host "Grapa is already in system PATH" -ForegroundColor Yellow
        }
        
        # Verify installation
        Write-Host "Verifying installation..." -ForegroundColor Yellow
        $grapaExe = Join-Path $binDir "grapa.exe"
        if (Test-Path $grapaExe) {
            $version = & $grapaExe --version 2>&1
            Write-Host "Installation successful!" -ForegroundColor Green
            Write-Host "Grapa version: $version" -ForegroundColor Green
            Write-Host ""
            Write-Host "Installation location:" -ForegroundColor Cyan
            Write-Host "  Executable: $binDir\grapa.exe" -ForegroundColor White
            Write-Host "  Library: $libDir\grapa.lib" -ForegroundColor White
            Write-Host ""
            Write-Host "Note: You may need to restart your terminal for PATH changes to take effect" -ForegroundColor Yellow
        } else {
            Write-Host "Error: Installation verification failed" -ForegroundColor Red
            exit 1
        }
        
    } catch {
        Write-Host "Error during installation: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
}

function Uninstall-Grapa {
    Write-Host "Uninstalling Grapa..." -ForegroundColor Green
    
    # Check if running as administrator
    if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
        Write-Host "Error: This script must be run as Administrator" -ForegroundColor Red
        Write-Host "Please right-click PowerShell and select 'Run as Administrator'" -ForegroundColor Yellow
        exit 1
    }
    
    # Define installation paths
    $installDir = "C:\Program Files\Grapa"
    $binDir = Join-Path $installDir "bin"
    $existingExe = Join-Path $binDir "grapa.exe"
    
    # Check if Grapa is installed
    if (-not (Test-Path $existingExe)) {
        Write-Host "Grapa is not installed at $installDir" -ForegroundColor Yellow
        Write-Host "Nothing to uninstall." -ForegroundColor Yellow
        return
    }
    
    # Show version being uninstalled
    $version = Get-GrapaVersion $existingExe
    Write-Host "Uninstalling Grapa version: $version" -ForegroundColor Yellow
    
    if (-not $Force) {
        $response = Read-Host "Are you sure you want to uninstall Grapa? (y/N)"
        if ($response -notmatch "^[Yy]$") {
            Write-Host "Uninstallation cancelled." -ForegroundColor Yellow
            exit 0
        }
    }
    
    try {
        # Remove from PATH
        Write-Host "Removing from system PATH..." -ForegroundColor Yellow
        $currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
        if ($currentPath -like "*$binDir*") {
            $newPath = ($currentPath -split ';' | Where-Object { $_ -ne $binDir }) -join ';'
            [Environment]::SetEnvironmentVariable('PATH', $newPath, 'Machine')
            Write-Host "Removed $binDir from system PATH" -ForegroundColor Green
        } else {
            Write-Host "Grapa was not found in system PATH" -ForegroundColor Yellow
        }
        
        # Remove installation directory
        if (Test-Path $installDir) {
            Write-Host "Removing installation directory..." -ForegroundColor Yellow
            Remove-Item -Path $installDir -Recurse -Force
            Write-Host "Removed installation directory: $installDir" -ForegroundColor Green
        } else {
            Write-Host "Installation directory not found: $installDir" -ForegroundColor Yellow
        }
        
        Write-Host "Uninstallation completed successfully!" -ForegroundColor Green
        
    } catch {
        Write-Host "Error during uninstallation: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
}

# Main script logic
if ($Help) {
    Show-Help
} elseif ($Uninstall) {
    Uninstall-Grapa
} else {
    Install-Grapa
}
