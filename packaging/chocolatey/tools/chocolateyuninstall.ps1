$ErrorActionPreference = 'Stop'

$packageName = 'grapa'
$installDir = 'C:\Program Files\Grapa'
$binDir = Join-Path $installDir 'bin'
$libDir = Join-Path $installDir 'lib'

# Remove from PATH
$currentPath = [Environment]::GetEnvironmentVariable('PATH', 'Machine')
$binPath = $binDir

if ($currentPath -like "*$binPath*") {
    # Remove from PATH
    $newPath = ($currentPath -split ';' | Where-Object { $_ -ne $binPath }) -join ';'
    [Environment]::SetEnvironmentVariable('PATH', $newPath, 'Machine')
    
    Write-Host "Removed $binPath from system PATH"
}

# Remove installed files and directories
if (Test-Path $installDir) {
    Remove-Item -Path $installDir -Recurse -Force
    Write-Host "Removed installation directory: $installDir"
}

Write-Host "Grapa has been uninstalled successfully!"
