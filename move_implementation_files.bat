@echo off
echo ========================================
echo Moving Implementation Files
echo ========================================
echo.

REM Files to move (checking if they exist in target first)
if exist "maintainers-dbx\IMPLEMENTATION\GRAPAFLOAT_IMPLEMENTATION.md" (
    if exist "maintainers\IMPLEMENTATION\GRAPAFLOAT_IMPLEMENTATION.md" (
        echo WARNING: GRAPAFLOAT_IMPLEMENTATION.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\GRAPAFLOAT_IMPLEMENTATION.md" maintainers\IMPLEMENTATION\
        echo Moved: GRAPAFLOAT_IMPLEMENTATION.md
    )
)

if exist "maintainers-dbx\IMPLEMENTATION\GRZ_FORMAT.md" (
    if exist "maintainers\IMPLEMENTATION\GRZ_FORMAT.md" (
        echo WARNING: GRZ_FORMAT.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\GRZ_FORMAT.md" maintainers\IMPLEMENTATION\
        echo Moved: GRZ_FORMAT.md
    )
)

if exist "maintainers-dbx\IMPLEMENTATION\INCLUDE_SYSTEM.md" (
    if exist "maintainers\IMPLEMENTATION\INCLUDE_SYSTEM.md" (
        echo WARNING: INCLUDE_SYSTEM.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\INCLUDE_SYSTEM.md" maintainers\IMPLEMENTATION\
        echo Moved: INCLUDE_SYSTEM.md
    )
)

if exist "maintainers-dbx\IMPLEMENTATION\SYSTEM_FUNCTIONS.md" (
    if exist "maintainers\IMPLEMENTATION\SYSTEM_FUNCTIONS.md" (
        echo WARNING: SYSTEM_FUNCTIONS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\SYSTEM_FUNCTIONS.md" maintainers\IMPLEMENTATION\
        echo Moved: SYSTEM_FUNCTIONS.md
    )
)

if exist "maintainers-dbx\IMPLEMENTATION\UNICODE_GREP_STATUS.md" (
    if exist "maintainers\IMPLEMENTATION\UNICODE_GREP_STATUS.md" (
        echo WARNING: UNICODE_GREP_STATUS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\UNICODE_GREP_STATUS.md" maintainers\IMPLEMENTATION\
        echo Moved: UNICODE_GREP_STATUS.md
    )
)

if exist "maintainers-dbx\IMPLEMENTATION\index.md" (
    if exist "maintainers\IMPLEMENTATION\index.md" (
        echo WARNING: index.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\index.md" maintainers\IMPLEMENTATION\
        echo Moved: index.md
    )
)

echo.
echo ========================================
echo Implementation Files Migration Complete!
echo ========================================
echo.
echo Note: Files that already existed in target were skipped.
echo You may need to manually compare and merge content for:
echo - BASETYPES.md
echo - BINARY_GREP.md  
echo - EXECUTION_TREES.md
echo - FIELD_METADATA_LIFECYCLE.md
echo - GRAPA_BTREE_FILE_STRUCTURE.md
echo - GRAPA_BTREE_IMPLEMENTATION.md
echo - GRAPA_CLI_IMPLEMENTATION.md
echo - GRAPA_DB_IMPLEMENTATION.md
echo.
pause 