@echo off
echo ========================================
echo GrapaDBX Migration Script
echo ========================================
echo.

echo Checking for existing files and migrating content...
echo.

REM Step 1: Move General Implementation Docs (with conflict resolution)
echo [1/6] Processing Implementation Documentation...

REM Files that should be moved to maintainers/IMPLEMENTATION/
if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_ENDIAN_SAFETY.md" (
    if exist "maintainers\IMPLEMENTATION\GRAPA_ENDIAN_SAFETY.md" (
        echo WARNING: GRAPA_ENDIAN_SAFETY.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\GRAPA_ENDIAN_SAFETY.md" maintainers\IMPLEMENTATION\
        echo Moved: GRAPA_ENDIAN_SAFETY.md
    )
)

if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_CROSS_PLATFORM_OBJECTS.md" (
    if exist "maintainers\IMPLEMENTATION\GRAPA_CROSS_PLATFORM_OBJECTS.md" (
        echo WARNING: GRAPA_CROSS_PLATFORM_OBJECTS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\IMPLEMENTATION\GRAPA_CROSS_PLATFORM_OBJECTS.md" maintainers\IMPLEMENTATION\
        echo Moved: GRAPA_CROSS_PLATFORM_OBJECTS.md
    )
)

REM Step 2: Move Testing and Quality Docs (all already exist, skip)
echo [2/6] Processing Testing Documentation...
echo All testing files already exist in target. Skipping...

REM Step 3: Move Build and Deployment Docs (all already exist, skip)
echo [3/6] Processing Build Documentation...
echo All build files already exist in target. Skipping...

REM Step 4: Move Internal Notes (with conflict resolution)
echo [4/6] Processing Internal Notes...

if exist "maintainers-dbx\INTERNAL_NOTES\FORMULA_SEARCH_INDEXING_ANALYSIS.md" (
    if exist "maintainers\INTERNAL_NOTES\FORMULA_SEARCH_INDEXING_ANALYSIS.md" (
        echo WARNING: FORMULA_SEARCH_INDEXING_ANALYSIS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\INTERNAL_NOTES\FORMULA_SEARCH_INDEXING_ANALYSIS.md" maintainers\INTERNAL_NOTES\
        echo Moved: FORMULA_SEARCH_INDEXING_ANALYSIS.md
    )
)

if exist "maintainers-dbx\INTERNAL_NOTES\FORMULA_VERSION_COMPATIBILITY_ANALYSIS.md" (
    if exist "maintainers\INTERNAL_NOTES\FORMULA_VERSION_COMPATIBILITY_ANALYSIS.md" (
        echo WARNING: FORMULA_VERSION_COMPATIBILITY_ANALYSIS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\INTERNAL_NOTES\FORMULA_VERSION_COMPATIBILITY_ANALYSIS.md" maintainers\INTERNAL_NOTES\
        echo Moved: FORMULA_VERSION_COMPATIBILITY_ANALYSIS.md
    )
)

if exist "maintainers-dbx\INTERNAL_NOTES\LIBRARY_SPECIFICATION_ANALYSIS.md" (
    if exist "maintainers\INTERNAL_NOTES\LIBRARY_SPECIFICATION_ANALYSIS.md" (
        echo WARNING: LIBRARY_SPECIFICATION_ANALYSIS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\INTERNAL_NOTES\LIBRARY_SPECIFICATION_ANALYSIS.md" maintainers\INTERNAL_NOTES\
        echo Moved: LIBRARY_SPECIFICATION_ANALYSIS.md
    )
)

if exist "maintainers-dbx\INTERNAL_NOTES\BUILTIN_NAMESPACE_DEFINITIONS_ANALYSIS.md" (
    if exist "maintainers\INTERNAL_NOTES\BUILTIN_NAMESPACE_DEFINITIONS_ANALYSIS.md" (
        echo WARNING: BUILTIN_NAMESPACE_DEFINITIONS_ANALYSIS.md already exists in target. Skipping...
    ) else (
        move "maintainers-dbx\INTERNAL_NOTES\BUILTIN_NAMESPACE_DEFINITIONS_ANALYSIS.md" maintainers\INTERNAL_NOTES\
        echo Moved: BUILTIN_NAMESPACE_DEFINITIONS_ANALYSIS.md
    )
)

REM Step 5: Move DBX-Specific Content to Archive
echo [5/6] Moving DBX-Specific Content to Archive...

REM DBX-specific testing
if exist "maintainers-dbx\TESTING_AND_QUALITY\GRAPADBX_TESTING_FRAMEWORK.md" (
    move "maintainers-dbx\TESTING_AND_QUALITY\GRAPADBX_TESTING_FRAMEWORK.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPADBX_TESTING_FRAMEWORK.md
)

REM DBX-specific implementation files
if exist "maintainers-dbx\IMPLEMENTATION\GRAPADBX_*.md" (
    move "maintainers-dbx\IMPLEMENTATION\GRAPADBX_*.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPADBX_*.md files
)

if exist "maintainers-dbx\IMPLEMENTATION\*UNIFIED*.md" (
    move "maintainers-dbx\IMPLEMENTATION\*UNIFIED*.md" maintainers\DBXARCHIVE\
    echo Moved: *UNIFIED*.md files
)

if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_GROUP_IMPLEMENTATION.md" (
    move "maintainers-dbx\IMPLEMENTATION\GRAPA_GROUP_IMPLEMENTATION.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPA_GROUP_IMPLEMENTATION.md
)

if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_FORMULA_EXECUTION_ANALYSIS.md" (
    move "maintainers-dbx\IMPLEMENTATION\GRAPA_FORMULA_EXECUTION_ANALYSIS.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPA_FORMULA_EXECUTION_ANALYSIS.md
)

if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_RAW_AND_NAMESPACE_ANALYSIS.md" (
    move "maintainers-dbx\IMPLEMENTATION\GRAPA_RAW_AND_NAMESPACE_ANALYSIS.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPA_RAW_AND_NAMESPACE_ANALYSIS.md
)

if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_UNIFIED_SYSTEM_ANALYSIS.md" (
    move "maintainers-dbx\IMPLEMENTATION\GRAPA_UNIFIED_SYSTEM_ANALYSIS.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPA_UNIFIED_SYSTEM_ANALYSIS.md
)

if exist "maintainers-dbx\IMPLEMENTATION\GRAPA_WIDGET_IMPLEMENTATION.md" (
    move "maintainers-dbx\IMPLEMENTATION\GRAPA_WIDGET_IMPLEMENTATION.md" maintainers\DBXARCHIVE\
    echo Moved: GRAPA_WIDGET_IMPLEMENTATION.md
)

if exist "maintainers-dbx\IMPLEMENTATION\ORIGIN_STORY.md" (
    move "maintainers-dbx\IMPLEMENTATION\ORIGIN_STORY.md" maintainers\DBXARCHIVE\
    echo Moved: ORIGIN_STORY.md
)

if exist "maintainers-dbx\IMPLEMENTATION\UNIVERSAL_PATH_SYSTEM.md" (
    move "maintainers-dbx\IMPLEMENTATION\UNIVERSAL_PATH_SYSTEM.md" maintainers\DBXARCHIVE\
    echo Moved: UNIVERSAL_PATH_SYSTEM.md
)

REM Remaining DBX-specific files
if exist "maintainers-dbx\DEVELOPMENT\index.md" (
    move "maintainers-dbx\DEVELOPMENT\index.md" maintainers\DBXARCHIVE\
    echo Moved: DEVELOPMENT\index.md
)

if exist "maintainers-dbx\DEVELOPMENT\README.md" (
    move "maintainers-dbx\DEVELOPMENT\README.md" maintainers\DBXARCHIVE\
    echo Moved: DEVELOPMENT\README.md
)

if exist "maintainers-dbx\index.md" (
    move "maintainers-dbx\index.md" maintainers\DBXARCHIVE\
    echo Moved: main index.md
)

if exist "maintainers-dbx\README.md" (
    move "maintainers-dbx\README.md" maintainers\DBXARCHIVE\
    echo Moved: main README.md
)

REM Step 6: Clean up empty directories
echo [6/6] Cleaning up empty directories...

if exist "maintainers-dbx" (
    echo Checking if maintainers-dbx is empty...
    dir "maintainers-dbx" /b > temp_check.txt
    findstr /v "^$" temp_check.txt > temp_nonempty.txt
    for %%A in (temp_nonempty.txt) do if %%~zA==0 (
        echo maintainers-dbx is empty, removing...
        rmdir "maintainers-dbx" /s
    ) else (
        echo maintainers-dbx still contains files, skipping removal
        type temp_nonempty.txt
    )
    del temp_check.txt temp_nonempty.txt
)

echo.
echo ========================================
echo Migration Complete!
echo ========================================
echo.
echo Next Steps:
echo 1. Review maintainers\DBXARCHIVE\ for DBX-specific content
echo 2. Audit maintainers\IMPLEMENTATION\ for any DBX references
echo 3. Audit maintainers\TESTING_AND_QUALITY\ for any DBX references
echo 4. Audit maintainers\BUILD_AND_DEPLOYMENT\ for any DBX references
echo 5. Audit maintainers\INTERNAL_NOTES\ for any DBX references
echo.
pause 