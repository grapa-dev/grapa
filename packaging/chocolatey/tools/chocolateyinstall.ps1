$ErrorActionPreference = 'Stop'

$packageName = 'grapa'
$toolsDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$installDir = Join-Path $toolsDir 'grapa.exe'

# The binary is already included in the package, just create the shim
Install-BinFile -Name 'grapa' -Path $installDir

Write-Host "Grapa has been installed successfully!"
Write-Host "You can now run 'grapa --version' to verify the installation."
