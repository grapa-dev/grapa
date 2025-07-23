# Grapa Documentation Build Script (Windows PowerShell Version)
# Builds documentation from /docs-src to /docs, with full validation.

param(
    [switch]$Help
)

$ErrorActionPreference = "Stop"

$REPO_ROOT = Split-Path -Parent $PSScriptRoot
$SRC_DIR = Join-Path $REPO_ROOT "docs-src"
$USER_CONFIG = Join-Path $SRC_DIR "mkdocs.yml"
$SITE_DIR = Join-Path $SRC_DIR "site"
$DEST_DIR = Join-Path $REPO_ROOT "docs"

# Colors for output
$RED = "Red"
$GREEN = "Green"
$YELLOW = "Yellow"
$BLUE = "Blue"

function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor $BLUE
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor $GREEN
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor $YELLOW
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor $RED
    exit 1
}

function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

function Validate-Prerequisites {
    Write-Info "Validating prerequisites..."
    
    if (-not (Test-Command "python")) {
        Write-Error "Python is not installed or not in PATH"
    }
    
    try {
        python -c "import mkdocs" 2>$null
        if ($LASTEXITCODE -ne 0) {
            Write-Error "MkDocs is not installed. Please install it with: pip install mkdocs-material"
        }
    }
    catch {
        Write-Error "MkDocs is not installed. Please install it with: pip install mkdocs-material"
    }
    
    if (-not (Test-Path $SRC_DIR)) {
        Write-Error "Source directory not found: $SRC_DIR"
    }
    
    if (-not (Test-Path $USER_CONFIG)) {
        Write-Error "User MkDocs configuration not found: $USER_CONFIG"
    }
    
    Write-Success "Prerequisites validated successfully"
}

function Build-UserDocs {
    Write-Info "Building user documentation..."
    
    Push-Location $SRC_DIR
    
    if (Test-Path $SITE_DIR) {
        Write-Info "Cleaning previous build..."
        Remove-Item $SITE_DIR -Recurse -Force
    }
    
    Write-Info "Running MkDocs build..."
    python -m mkdocs build --clean
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to build user documentation"
    }
    
    if (-not (Test-Path (Join-Path $SITE_DIR "index.html"))) {
        Write-Error "Build failed: index.html not found"
    }
    
    Write-Success "User documentation built successfully"
    Write-Info "Build verification completed"
    
    Pop-Location
}

function Copy-SiteFiles {
    Write-Info "Copying site files to $DEST_DIR..."
    
    if (Test-Path $DEST_DIR) {
        Remove-Item $DEST_DIR -Recurse -Force
    }
    
    New-Item -ItemType Directory -Path $DEST_DIR -Force | Out-Null
    Copy-Item (Join-Path $SITE_DIR "*") $DEST_DIR -Recurse -Force
    
    Write-Success "Site files copied to $DEST_DIR"
}

function Commit-AndPushDocs {
    Write-Info "Staging and committing /docs and /docs-src/site deletions..."
    
    git add docs/
    git add -u docs-src/site
    
    $changes = git diff --cached --quiet
    if ($LASTEXITCODE -eq 0) {
        Write-Info "No changes to commit."
    }
    else {
        git commit -m "docs: Update user documentation and clean up build artifacts"
        git push origin main
        Write-Success "Changes committed and pushed to main."
    }
}

function Show-Usage {
    Write-Host "Usage: .\deploy_docs.ps1"
    Write-Host "Builds user documentation from /docs-src to /docs with validation."
    Write-Host "After running, commit and push the /docs directory to main to deploy."
}

function Main {
    if ($Help) {
        Show-Usage
        exit 0
    }
    
    Write-Info "Starting Grapa documentation build (user docs only)..."
    Write-Info "Source: $SRC_DIR"
    Write-Info "Destination: $DEST_DIR"
    
    Validate-Prerequisites
    Build-UserDocs
    Copy-SiteFiles
    Commit-AndPushDocs
    
    Write-Success "Documentation build and deploy completed!"
    Write-Host "`nGitHub Pages will update automatically from /docs on main."
}

Main 