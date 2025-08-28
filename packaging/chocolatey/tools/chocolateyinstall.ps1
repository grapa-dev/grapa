$ErrorActionPreference = 'Stop'

$packageName = 'grapa'
$toolsDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$installDir = 'C:\Program Files\Grapa'
$binDir = Join-Path $installDir 'bin'
$libDir = Join-Path $installDir 'lib'

# Create installation directories
New-Item -ItemType Directory -Force -Path $installDir | Out-Null
New-Item -ItemType Directory -Force -Path $binDir | Out-Null
New-Item -ItemType Directory -Force -Path $libDir | Out-Null

# Copy files to installation directory
Copy-Item -Path (Join-Path $toolsDir 'grapa.exe') -Destination $binDir -Force
Copy-Item -Path (Join-Path $toolsDir 'grapa.lib') -Destination $libDir -Force

# Check if bin directory is already in PATH
$currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
$binPath = $binDir

if ($currentPath -notlike "*$binPath*") {
    # Add to PATH
    $newPath = $currentPath + ';' + $binPath
    [Environment]::SetEnvironmentVariable('PATH', $newPath, 'Machine')
    
    # Update current session PATH
    $env:PATH = $env:PATH + ';' + $binPath
    
    Write-Host "Added $binPath to system PATH"
} else {
    Write-Host "$binPath is already in system PATH"
}

Write-Host "Grapa has been installed successfully to $installDir"
Write-Host "Executable: $binDir\grapa.exe"
Write-Host "Library: $libDir\grapa.lib"
Write-Host "You can now run 'grapa --version' to verify the installation."
