$ErrorActionPreference = 'Stop'

$packageName = 'grapa'

# Remove the shim
Uninstall-BinFile -Name 'grapa'

Write-Host "Grapa has been uninstalled successfully!"
