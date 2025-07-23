# Debug Statement Cleanup Script for Grapa
# Removes all debug printf statements and debug blocks

$files = @('source/grapa/GrapaDB.cpp', 'source/grapa/GrapaBtree.cpp')

foreach ($file in $files) {
    Write-Host "Cleaning $file..."
    
    if (Test-Path $file) {
        $content = Get-Content $file -Raw
        
        # Remove debug printf statements
        $content = $content -replace 'printf\s*\(\s*"\[DEBUG[^"]*"[^)]*\)\s*;?\s*', ''
        
        # Remove debug blocks
        $content = $content -replace '#ifdef DEBUG_BTREE_INVESTIGATION\s*\r?\n(?:[^#]|#(?!endif))*?#endif\s*\r?\n?', ''
        $content = $content -replace '#ifdef DEBUG_COMPARE_KEY\s*\r?\n(?:[^#]|#(?!endif))*?#endif\s*\r?\n?', ''
        $content = $content -replace '#ifdef DEBUG_INDEX_OPERATIONS\s*\r?\n(?:[^#]|#(?!endif))*?#endif\s*\r?\n?', ''
        
        # Remove empty lines that might be left
        $content = $content -replace '\r?\n\s*\r?\n\s*\r?\n', "`r`n`r`n"
        
        Set-Content $file $content -NoNewline
        Write-Host "Cleaned $file"
    } else {
        Write-Host "File not found: $file"
    }
}

Write-Host "Debug cleanup complete!" 