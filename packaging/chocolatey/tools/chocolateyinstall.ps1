$ErrorActionPreference = 'Stop'

$packageName = 'grapa'
$url = 'https://github.com/grapa/grapa/releases/download/v0.1.51/grapa-win-amd64.exe'
$checksum = 'PLACEHOLDER_SHA256' # This will be updated with actual SHA256
$checksumType = 'sha256'

$toolsDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$installDir = Join-Path $toolsDir 'grapa.exe'

# Download and install the executable
Get-ChocolateyWebFile -PackageName $packageName `
                      -FileFullPath $installDir `
                      -Url $url `
                      -Checksum $checksum `
                      -ChecksumType $checksumType

# Create a shim for the executable
Install-BinFile -Name 'grapa' -Path $installDir

Write-Host "Grapa has been installed successfully!"
Write-Host "You can now run 'grapa --version' to verify the installation."
