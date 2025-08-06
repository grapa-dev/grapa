# Monitor Windows Workflow and Download Artifacts
# This script monitors the Windows GitHub Actions workflow and downloads artifacts when it completes

Write-Host "🪟 Monitoring Windows GitHub Actions workflow..." -ForegroundColor Green
Write-Host ""

# Check if we have the GitHub CLI installed
try {
    $null = gh --version
    Write-Host "✅ GitHub CLI is available" -ForegroundColor Green
} catch {
    Write-Host "❌ GitHub CLI (gh) is not installed" -ForegroundColor Red
    Write-Host "   Please install it from: https://cli.github.com/" -ForegroundColor Yellow
    exit 1
}

# Check if we're authenticated
try {
    $null = gh auth status
    Write-Host "✅ GitHub CLI is authenticated" -ForegroundColor Green
} catch {
    Write-Host "❌ Not authenticated with GitHub CLI" -ForegroundColor Red
    Write-Host "   Please run: gh auth login" -ForegroundColor Yellow
    exit 1
}

Write-Host ""

# Function to get the latest workflow run
function Get-LatestRun {
    gh run list --workflow="Build Windows AMD64" --limit=1 --json databaseId,status,conclusion --jq '.[0]'
}

# Function to download artifacts
function Download-Artifacts {
    param($RunId)
    Write-Host ""
    Write-Host "📥 Downloading artifacts for run $RunId..." -ForegroundColor Green
    
    # Create temporary directory for download
    $TempDir = New-TemporaryFile | ForEach-Object { Remove-Item $_; New-Item -ItemType Directory -Path $_ }
    Write-Host "📁 Using temporary directory: $TempDir" -ForegroundColor Cyan
    
    # Download artifacts to temporary directory
    gh run download "$RunId" --name="grapa-windows-amd64" --dir="$TempDir"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "✅ Artifacts downloaded successfully!" -ForegroundColor Green
        Write-Host ""
        
        # Create necessary directories
        New-Item -ItemType Directory -Force -Path "source/grapa-lib/win-amd64" | Out-Null
        New-Item -ItemType Directory -Force -Path "source/grapa-other/win-amd64" | Out-Null
        New-Item -ItemType Directory -Force -Path "bin" | Out-Null
        
        # Copy files with force overwrite
        Write-Host "📁 Copying files with force overwrite..." -ForegroundColor Green
        
        # Copy executable
        if (Test-Path "$TempDir/grapa.exe") {
            Copy-Item "$TempDir/grapa.exe" "." -Force
            Write-Host "  ✅ Copied grapa.exe" -ForegroundColor Green
        } else {
            Write-Host "  ❌ grapa.exe not found in artifacts" -ForegroundColor Red
        }
        
        # Copy static libraries
        if (Test-Path "$TempDir/source/grapa-lib/win-amd64") {
            Copy-Item "$TempDir/source/grapa-lib/win-amd64/*" "source/grapa-lib/win-amd64/" -Force -Recurse
            Write-Host "  ✅ Copied static libraries" -ForegroundColor Green
        } else {
            Write-Host "  ❌ Static libraries not found in artifacts" -ForegroundColor Red
        }
        
        # Copy shared libraries
        if (Test-Path "$TempDir/source/grapa-other/win-amd64") {
            Copy-Item "$TempDir/source/grapa-other/win-amd64/*" "source/grapa-other/win-amd64/" -Force -Recurse
            Write-Host "  ✅ Copied shared libraries" -ForegroundColor Green
        } else {
            Write-Host "  ❌ Shared libraries not found in artifacts" -ForegroundColor Red
        }
        
        # Copy compressed package
        if (Test-Path "$TempDir/bin/grapa-win-amd64.tar.gz") {
            Copy-Item "$TempDir/bin/grapa-win-amd64.tar.gz" "bin/" -Force
            Write-Host "  ✅ Copied compressed package" -ForegroundColor Green
        } elseif (Test-Path "$TempDir/bin/grapa-win-amd64.zip") {
            Copy-Item "$TempDir/bin/grapa-win-amd64.zip" "bin/" -Force
            Write-Host "  ✅ Copied compressed package (zip)" -ForegroundColor Green
        } else {
            Write-Host "  ❌ Compressed package not found in artifacts" -ForegroundColor Red
        }
        
        # Clean up temporary directory
        Remove-Item "$TempDir" -Recurse -Force
        Write-Host ""
        Write-Host "🎉 Windows build artifacts are now available!" -ForegroundColor Green
        Write-Host "   You can now run: ./scripts/validation/check_platform_status.sh" -ForegroundColor Cyan
        
        # Show what was downloaded
        Write-Host ""
        Write-Host "📁 Downloaded files:" -ForegroundColor Green
        if (Test-Path "grapa.exe") {
            Get-ChildItem "grapa.exe" | ForEach-Object { Write-Host "  ✅ $($_.Name) ($($_.Length) bytes)" -ForegroundColor Green }
        } else {
            Write-Host "  ❌ grapa.exe (missing)" -ForegroundColor Red
        }
        
        if (Test-Path "source/grapa-lib/win-amd64") {
            Get-ChildItem "source/grapa-lib/win-amd64" | ForEach-Object { Write-Host "  ✅ $($_.Name) ($($_.Length) bytes)" -ForegroundColor Green }
        } else {
            Write-Host "  ❌ source/grapa-lib/win-amd64/ (missing)" -ForegroundColor Red
        }
        
        if (Test-Path "source/grapa-other/win-amd64") {
            Get-ChildItem "source/grapa-other/win-amd64" | ForEach-Object { Write-Host "  ✅ $($_.Name) ($($_.Length) bytes)" -ForegroundColor Green }
        } else {
            Write-Host "  ❌ source/grapa-other/win-amd64/ (missing)" -ForegroundColor Red
        }
        
        if (Test-Path "bin/grapa-win-amd64.*") {
            Get-ChildItem "bin/grapa-win-amd64.*" | ForEach-Object { Write-Host "  ✅ $($_.Name) ($($_.Length) bytes)" -ForegroundColor Green }
        } else {
            Write-Host "  ❌ bin/grapa-win-amd64.* (missing)" -ForegroundColor Red
        }
        
        return $true
    } else {
        Write-Host "❌ Failed to download artifacts" -ForegroundColor Red
        return $false
    }
}

# Get the latest run
Write-Host "🔍 Finding latest Windows build workflow run..." -ForegroundColor Green
$LatestRun = Get-LatestRun

if ($LatestRun -eq "null") {
    Write-Host "❌ No Windows build workflow runs found" -ForegroundColor Red
    Write-Host "   Please trigger the workflow first:" -ForegroundColor Yellow
    Write-Host "   1. Go to: https://github.com/grapa-dev/grapa/actions" -ForegroundColor Cyan
    Write-Host "   2. Click 'Build Windows AMD64'" -ForegroundColor Cyan
    Write-Host "   3. Click 'Run workflow'" -ForegroundColor Cyan
    exit 1
}

$RunId = ($LatestRun | ConvertFrom-Json).databaseId
$Status = ($LatestRun | ConvertFrom-Json).status
$Conclusion = ($LatestRun | ConvertFrom-Json).conclusion

Write-Host "📋 Latest run ID: $RunId" -ForegroundColor Cyan
Write-Host "📋 Status: $Status" -ForegroundColor Cyan
Write-Host "📋 Conclusion: $Conclusion" -ForegroundColor Cyan
Write-Host ""

# If already completed successfully, download immediately
if ($Status -eq "completed" -and $Conclusion -eq "success") {
    Write-Host "✅ Workflow already completed successfully!" -ForegroundColor Green
    Download-Artifacts $RunId
    exit $LASTEXITCODE
}

# If failed or cancelled, exit
if ($Status -eq "completed" -and $Conclusion -ne "success") {
    Write-Host "❌ Workflow failed or was cancelled" -ForegroundColor Red
    Write-Host "   Please check the workflow run for errors" -ForegroundColor Yellow
    exit 1
}

# If still running, monitor it
if ($Status -ne "completed") {
    Write-Host "⏳ Workflow is still running... monitoring progress" -ForegroundColor Yellow
    Write-Host "   You can view the live logs at: https://github.com/grapa-dev/grapa/actions/runs/$RunId" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "🔄 Monitoring for completion (checking every 30 seconds)..." -ForegroundColor Green
    Write-Host "   Press Ctrl+C to stop monitoring" -ForegroundColor Yellow
    Write-Host ""
    
    # Monitor the workflow
    while ($true) {
        Start-Sleep 30
        
        # Get updated status
        $LatestRun = Get-LatestRun
        $Status = ($LatestRun | ConvertFrom-Json).status
        $Conclusion = ($LatestRun | ConvertFrom-Json).conclusion
        
        Write-Host "$(Get-Date -Format 'HH:mm:ss') - Status: $Status, Conclusion: $Conclusion" -ForegroundColor Gray
        
        if ($Status -eq "completed") {
            if ($Conclusion -eq "success") {
                Write-Host ""
                Write-Host "✅ Workflow completed successfully!" -ForegroundColor Green
                Download-Artifacts $RunId
                exit $LASTEXITCODE
            } else {
                Write-Host ""
                Write-Host "❌ Workflow failed or was cancelled" -ForegroundColor Red
                Write-Host "   Please check the workflow run for errors" -ForegroundColor Yellow
                exit 1
            }
        }
    }
} 