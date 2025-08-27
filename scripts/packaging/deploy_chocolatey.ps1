# Grapa Chocolatey Package Deployment Script
# Automates the process of building and testing Chocolatey packages for Windows

param(
    [switch]$Full,
    [switch]$Test,
    [switch]$Build,
    [switch]$Validate
)

# Set error action preference
$ErrorActionPreference = 'Stop'

# Script configuration
$RepoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$PackageDir = Join-Path $RepoRoot "packaging\chocolatey"
$NuspecFile = Join-Path $PackageDir "grapa.nuspec"
$ToolsDir = Join-Path $PackageDir "tools"
$Version = "0.1.51"
$BinaryName = "grapa-win-amd64.exe"
$BinaryPath = Join-Path $RepoRoot "bin\$BinaryName"

# Colors for output
$Red = "`e[31m"
$Green = "`e[32m"
$Yellow = "`e[33m"
$Blue = "`e[34m"
$Reset = "`e[0m"

function Write-Info { Write-Host "$Blue[INFO]$Reset $args" }
function Write-Success { Write-Host "$Green[SUCCESS]$Reset $args" }
function Write-Warning { Write-Host "$Yellow[WARNING]$Reset $args" }
function Write-Error { Write-Host "$Red[ERROR]$Reset $args" }

function Test-Command {
    param([string]$Command)
    try { Get-Command $Command -ErrorAction Stop | Out-Null; return $true }
    catch { return $false }
}

function Test-Prerequisites {
    Write-Info "Validating prerequisites..."
    
    # Check for Chocolatey (only required for build/test operations)
    if (-not (Test-Command "choco")) {
        Write-Warning "Chocolatey is not installed or not in PATH"
        Write-Info "Chocolatey is required for building and testing packages"
        Write-Info "Please install Chocolatey from https://chocolatey.org/install"
        Write-Info "For validation only, this is not required"
    } else {
        Write-Success "Chocolatey found"
    }
    
    # Check for Git
    if (-not (Test-Command "git")) {
        Write-Error "Git is not installed or not in PATH"
        exit 1
    }
    
    # Check for required files
    if (-not (Test-Path $NuspecFile)) {
        Write-Error "Chocolatey nuspec file not found: $NuspecFile"
        exit 1
    }
    
    if (-not (Test-Path $BinaryPath)) {
        Write-Error "Windows binary not found: $BinaryPath"
        Write-Info "Please ensure the Windows binary is built and available"
        exit 1
    }
    
    Write-Success "Prerequisites validated successfully"
}

function Update-PackageChecksum {
    Write-Info "Updating package checksum..."
    
    if (Test-Path $BinaryPath) {
        # Calculate SHA256 of the binary
        $sha256 = (Get-FileHash -Path $BinaryPath -Algorithm SHA256).Hash.ToLower()
        Write-Info "Calculated SHA256: $sha256"
        
        # Update the install script
        $installScript = Join-Path $ToolsDir "chocolateyinstall.ps1"
        $content = Get-Content $installScript -Raw
        $content = $content -replace 'PLACEHOLDER_SHA256', $sha256
        Set-Content $installScript $content -NoNewline
        
        Write-Success "Updated package with SHA256 checksum"
    } else {
        Write-Warning "Binary not found at $BinaryPath"
        Write-Warning "Please ensure the binary is available and update checksum manually"
    }
}

function Test-PackageStructure {
    Write-Info "Testing package structure..."
    
    # Check nuspec file
    if (-not (Test-Path $NuspecFile)) {
        Write-Error "Nuspec file not found: $NuspecFile"
        return $false
    }
    
    # Check tools directory
    if (-not (Test-Path $ToolsDir)) {
        Write-Error "Tools directory not found: $ToolsDir"
        return $false
    }
    
    # Check install script
    $installScript = Join-Path $ToolsDir "chocolateyinstall.ps1"
    if (-not (Test-Path $installScript)) {
        Write-Error "Install script not found: $installScript"
        return $false
    }
    
    # Check uninstall script
    $uninstallScript = Join-Path $ToolsDir "chocolateyuninstall.ps1"
    if (-not (Test-Path $uninstallScript)) {
        Write-Error "Uninstall script not found: $uninstallScript"
        return $false
    }
    
    Write-Success "Package structure validation passed"
    return $true
}

function Build-Package {
    Write-Info "Building Chocolatey package..."
    
    # Change to package directory
    Push-Location $PackageDir
    
    try {
        # Build the package
        $packageFile = "grapa.$Version.nupkg"
        
        # Remove existing package if it exists
        if (Test-Path $packageFile) {
            Remove-Item $packageFile -Force
        }
        
        # Build package using choco pack
        Write-Info "Running: choco pack"
        choco pack
        
        if (Test-Path $packageFile) {
            Write-Success "Package built successfully: $packageFile"
            $packagePath = Join-Path $PackageDir $packageFile
            return $packagePath
        } else {
            Write-Error "Package build failed"
            return $null
        }
    }
    finally {
        Pop-Location
    }
}

function Test-PackageInstallation {
    param([string]$PackagePath)
    
    Write-Info "Testing package installation..."
    
    if (-not (Test-Path $PackagePath)) {
        Write-Error "Package file not found: $PackagePath"
        return $false
    }
    
    # Test installation
    Write-Info "Testing installation with: choco install $PackagePath -y"
    try {
        choco install $PackagePath -y
        
        # Test if grapa is available
        if (Test-Command "grapa") {
            Write-Info "Testing grapa --version"
            $version = grapa --version
            Write-Info "Version output: $version"
            
            if ($version -match "Version: $Version") {
                Write-Success "Package installation test passed"
                
                # Test basic functionality
                Write-Info "Testing basic functionality: grapa -c '2+2'"
                $result = grapa -c "2+2"
                Write-Info "Basic test result: $result"
                
                if ($result -eq "4") {
                    Write-Success "Basic functionality test passed"
                    
                    # Uninstall for cleanup
                    Write-Info "Uninstalling package for cleanup"
                    choco uninstall grapa -y
                    
                    return $true
                } else {
                    Write-Error "Basic functionality test failed"
                    choco uninstall grapa -y
                    return $false
                }
            } else {
                Write-Error "Version test failed"
                choco uninstall grapa -y
                return $false
            }
        } else {
            Write-Error "grapa command not found after installation"
            choco uninstall grapa -y
            return $false
        }
    }
    catch {
        Write-Error "Package installation test failed: $($_.Exception.Message)"
        choco uninstall grapa -y -ErrorAction SilentlyContinue
        return $false
    }
}

function Create-GitHubRelease {
    param([string]$PackagePath)
    
    Write-Info "Creating GitHub release..."
    
    if (-not (Test-Path $PackagePath)) {
        Write-Error "Package file not found: $PackagePath"
        return $false
    }
    
    # Check if GitHub CLI is available
    if (-not (Test-Command "gh")) {
        Write-Warning "GitHub CLI (gh) is not installed. Skipping GitHub release creation."
        Write-Info "Please install GitHub CLI and authenticate to create releases automatically."
        return $false
    }
    
    # Check authentication
    try {
        gh auth status | Out-Null
    }
    catch {
        Write-Warning "GitHub CLI is not authenticated. Skipping GitHub release creation."
        Write-Info "Please run 'gh auth login' to authenticate."
        return $false
    }
    
    # Create release
    $releaseTag = "v$Version-chocolatey"
    $releaseTitle = "Grapa $Version - Chocolatey Package"
    $releaseNotes = @"
## Grapa $Version - Chocolatey Package

This release provides the Grapa programming language as a Chocolatey package for Windows.

### Installation
```powershell
choco install grapa
```

### Features
- Unlimited precision arithmetic
- Advanced vector and matrix operations
- Comprehensive cryptography with OpenSSL 3.5.2
- Machine learning capabilities
- Cross-platform compatibility
- Python integration via GrapaPy

For more information, visit: https://github.com/grapa-dev/grapa
"@
    
    try {
        # Create release
        gh release create $releaseTag $PackagePath --title $releaseTitle --notes $releaseNotes
        
        Write-Success "GitHub release created: $releaseTag"
        Write-Info "Release URL: https://github.com/grapa-dev/grapa/releases/tag/$releaseTag"
        return $true
    }
    catch {
        Write-Error "Failed to create GitHub release: $($_.Exception.Message)"
        return $false
    }
}

function Show-Usage {
    Write-Host @"
Grapa Chocolatey Package Deployment Script

Usage: .\deploy_chocolatey.ps1 [options]

Options:
    -Full       Run complete deployment (validate, build, test, release)
    -Test       Test package installation
    -Build      Build package only
    -Validate   Validate prerequisites and package structure only

Examples:
    .\deploy_chocolatey.ps1 -Full          # Complete deployment
    .\deploy_chocolatey.ps1 -Validate      # Validate only
    .\deploy_chocolatey.ps1 -Build         # Build package only
    .\deploy_chocolatey.ps1 -Test          # Test installation only
"@
}

# Main execution
if (-not ($Full -or $Test -or $Build -or $Validate)) {
    Show-Usage
    exit 0
}

Write-Info "Starting Grapa Chocolatey package deployment..."

try {
    # Validate prerequisites
    Test-Prerequisites
    
    # Update checksum
    Update-PackageChecksum
    
    # Validate package structure
    if (-not (Test-PackageStructure)) {
        exit 1
    }
    
    if ($Validate) {
        Write-Success "Validation completed successfully"
        exit 0
    }
    
    # Build package
    if ($Build -or $Full) {
        $packagePath = Build-Package
        if (-not $packagePath) {
            exit 1
        }
    }
    
    # Test package
    if ($Test -or $Full) {
        if (-not $packagePath) {
            $packagePath = Join-Path $PackageDir "grapa.$Version.nupkg"
        }
        
        if (-not (Test-PackageInstallation $packagePath)) {
            exit 1
        }
    }
    
    # Create GitHub release
    if ($Full) {
        if (-not $packagePath) {
            $packagePath = Join-Path $PackageDir "grapa.$Version.nupkg"
        }
        
        Create-GitHubRelease $packagePath
    }
    
    Write-Success "Chocolatey package deployment completed successfully!"
    
    if ($packagePath -and (Test-Path $packagePath)) {
        Write-Info "Package location: $packagePath"
        Write-Info "Package size: $((Get-Item $packagePath).Length / 1MB) MB"
    }
}
catch {
    Write-Error "Deployment failed: $($_.Exception.Message)"
    exit 1
}
