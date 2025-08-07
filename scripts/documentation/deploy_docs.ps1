# Grapa Documentation Build Script (Windows PowerShell Version)
# Builds documentation from /docs-src to /docs, with full validation.

param(
    [switch]$Help,
    [switch]$Push
)

$ErrorActionPreference = "Stop"

$REPO_ROOT = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
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
        Write-Success "Documentation changes committed."
    }
}

function Show-Usage {
    Write-Host "Usage: .\deploy_docs.ps1 [OPTIONS]"
    Write-Host "Builds user documentation from /docs-src to /docs with validation."
    Write-Host ""
    Write-Host "OPTIONS:"
    Write-Host "  -Push        Push changes to main branch (triggers GitHub Pages deployment)"
    Write-Host "  -Help        Show this help message"
    Write-Host ""
    Write-Host "EXAMPLES:"
    Write-Host "  .\deploy_docs.ps1              Build and commit documentation (no push)"
    Write-Host "  .\deploy_docs.ps1 -Push        Build, commit, and push to trigger GitHub Pages deployment"
    Write-Host ""
    Write-Host "Note: Use -Push only when you want to deploy documentation to GitHub Pages."
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
    
    if ($Push) {
        Write-Info "Pushing changes to main branch..."
        git push origin main
        Write-Success "Changes pushed to main. GitHub Pages deployment triggered."
    }
    else {
        Write-Info "Changes committed locally. Run 'git push origin main' to deploy to GitHub Pages."
    }
    
    Write-Success "Documentation build completed!"
}

# Verify documentation separation policy: no links from docs-src to maintainers or outside docs-src, except in deep_expert_implementation_overview.md
$allDocs = Get-ChildItem -Path 'docs-src/**/*.md' -Recurse
$docsToCheck = $allDocs | Where-Object { $_.Name -ne 'deep_expert_implementation_overview.md' }
$violations = Select-String -Path $docsToCheck.FullName -Pattern '\.\./maintainers/|\.\./\.\./maintainers/|/maintainers/'
if ($violations) {
    Write-Host "ERROR: User-facing docs in docs-src must not link to or reference anything outside docs-src (including maintainers/), except in deep_expert_implementation_overview.md." -ForegroundColor Red
    $violations | ForEach-Object { Write-Host $_.Path ':' $_.LineNumber ':' $_.Line }
    exit 1
}

# Check for orphan docs (not linked from index.md or any other .md in docs-src, or included in mkdocs.yml nav)
$allDocs = Get-ChildItem -Path 'docs-src/docs/' -Filter *.md -Recurse | Where-Object { $_.Name -ne 'index.md' }
$docsRoot = (Resolve-Path 'docs-src/docs/').Path.ToLower()

# Print debug info for path resolution
if ($allDocs.Count -gt 0) {
    Write-Host "DEBUG: Sample $allDocs[0].FullName = $($allDocs[0].FullName)" -ForegroundColor Yellow
}

# Extract all .md files linked from Markdown files
$linkedDocs = Select-String -Path 'docs-src/docs/**/*.md' -Pattern '\(([^)]+\.md)\)' | ForEach-Object { $_.Matches.Groups[1].Value.TrimStart('./') } | Select-Object -Unique

# Extract all .md files listed in mkdocs.yml nav (recursively, robust to nesting)
function Get-MkdocsNavDocs {
    param([object[]]$navTree, [string]$prefix = "")
    $result = @()
    foreach ($item in $navTree) {
        if ($item -is [System.Collections.Hashtable]) {
            foreach ($key in $item.Keys) {
                $value = $item[$key]
                if ($value -is [string] -and $value.ToLower().EndsWith('.md')) {
                    if ($prefix) {
                        $result += (Join-Path $prefix $value).Replace('/', '\').ToLower()
                    } else {
                        $result += $value.Replace('/', '\').ToLower()
                    }
                } elseif ($value -is [System.Collections.IEnumerable]) {
                    $result += Get-MkdocsNavDocs -navTree $value -prefix $prefix
                }
            }
        } elseif ($item -is [string] -and $item.ToLower().EndsWith('.md')) {
            if ($prefix) {
                $result += (Join-Path $prefix $item).Replace('/', '\').ToLower()
            } else {
                $result += $item.Replace('/', '\').ToLower()
            }
        }
    }
    return $result
}

# Parse mkdocs.yml nav section robustly
$mkdocsYaml = Get-Content 'docs-src/mkdocs.yml' -Raw
$mkdocsObj = ConvertFrom-Yaml $mkdocsYaml
$mkdocsNavDocs = Get-MkdocsNavDocs -navTree $mkdocsObj.nav
$mkdocsNavDocs = $mkdocsNavDocs | ForEach-Object { $_ -replace '^docs[\\/]*', '' } | Select-Object -Unique

# Merge both sets for discoverable docs
$linkedDocsLower = $linkedDocs | ForEach-Object { $_.ToLower() }
$discoverableDocs = $linkedDocsLower + $mkdocsNavDocs | Select-Object -Unique

# Normalize $docName and $discoverableDocs to be relative to docs-src/docs/
$allDocs | Select-Object -First 10 | ForEach-Object {
    $docName = $_.FullName.ToLower() -replace [regex]::Escape($docsRoot), ''
    $docName = $docName.TrimStart('\','/')
    Write-Host $docName
}
$discoverableDocsNormalized = $discoverableDocs | ForEach-Object {
    $_ -replace '^docs-src/docs[\\/]*', ''
}
Write-Host "DEBUG: Sample discoverableDocsNormalized values:" -ForegroundColor Yellow
$discoverableDocsNormalized | Select-Object -First 20 | ForEach-Object { Write-Host $_ }

# Check for orphans (case-insensitive)
$orphans = $allDocs | Where-Object {
    $docName = $_.FullName.ToLower() -replace [regex]::Escape($docsRoot), ''
    $docName = $docName.TrimStart('\','/')
    $discoverableDocsNormalized -notcontains $docName
}
if ($orphans) {
    Write-Host "ERROR: The following docs in docs-src are not discoverable (not linked from index.md, any other .md, or mkdocs.yml nav):" -ForegroundColor Red
    $orphans | ForEach-Object { Write-Host $_.FullName }
    exit 1
}

Main 