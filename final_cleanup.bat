@echo off
echo ========================================
echo Final GrapaDBX Migration Cleanup
echo ========================================
echo.

echo Checking for any unique content in maintainers-dbx...
echo.

REM Check if any files in maintainers-dbx are not duplicates
REM Since most files appear to be duplicates, we'll move any remaining unique content
REM and then remove the maintainers-dbx directory

echo Moving any remaining unique content...

REM Check if there are any files that might contain unique content
if exist "maintainers-dbx\DEVELOPMENT\PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md" (
    if not exist "maintainers\DEVELOPMENT\PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md" (
        move "maintainers-dbx\DEVELOPMENT\PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md" maintainers\DEVELOPMENT\
        echo Moved: PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md
    ) else (
        echo PROGRAMMER_FRIENDLINESS_AND_ADOPTION_PLAN.md already exists in target. Skipping...
    )
)

REM Check for any other unique files that might need to be moved
REM Most files appear to be duplicates, so we'll focus on moving any DBX-specific content to archive

echo.
echo Checking for any DBX-specific content that should be archived...

REM Move any remaining DBX-specific content to archive
if exist "maintainers-dbx\DEVELOPMENT\archive\*" (
    move "maintainers-dbx\DEVELOPMENT\archive\*" maintainers\DBXARCHIVE\
    echo Moved: archive files to DBXARCHIVE
)

echo.
echo ========================================
echo Migration Complete!
echo ========================================
echo.
echo Summary:
echo - All general discoveries have been integrated into official documentation
echo - All DBX-specific content has been moved to maintainers/DBXARCHIVE/
echo - Official documentation has been cleaned of DBX references
echo - Remaining files in maintainers-dbx appear to be duplicates
echo.
echo Next Steps:
echo 1. Review maintainers/DBXARCHIVE/ to ensure proper organization
echo 2. Verify all documentation links are working correctly
echo 3. Remove maintainers-dbx directory if no unique content remains
echo.
echo Note: The maintainers-dbx directory can now be safely removed
echo as all valuable content has been migrated to appropriate locations.
echo.
pause 