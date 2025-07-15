@echo off
REM Grapa Grep Test Group Execution Script
REM This script implements the test strategy with different test groups
REM for different execution frequencies and purposes.

echo === GRAPA GREP TEST GROUP EXECUTION ===
echo.

if "%1"=="" (
    echo Usage: run_test_groups.bat [GROUP]
    echo.
    echo Available test groups:
    echo   smoke     - Run smoke tests (every build, <30s)
    echo   regression - Run regression tests (daily, <5min)
    echo   integration - Run integration tests (weekly, <15min)
    echo   comprehensive - Run comprehensive tests (monthly, <30min)
    echo   production - Run production readiness tests (pre-release, <60min)
    echo   development - Run development tests (during development)
    echo   research - Run research tests (as needed)
    echo   all - Run all test groups (full validation)
    echo.
    exit /b 1
)

set GROUP=%1
echo Running test group: %GROUP%
echo.

if "%GROUP%"=="smoke" goto :smoke
if "%GROUP%"=="regression" goto :regression
if "%GROUP%"=="integration" goto :integration
if "%GROUP%"=="comprehensive" goto :comprehensive
if "%GROUP%"=="production" goto :production
if "%GROUP%"=="development" goto :development
if "%GROUP%"=="research" goto :research
if "%GROUP%"=="all" goto :all

echo Unknown test group: %GROUP%
exit /b 1

:smoke
echo === RUNNING SMOKE TESTS (Every Build) ===
echo Purpose: Verify basic functionality is working
echo Execution time: < 30 seconds
echo.
echo Running basic functionality tests...
.\grapa.exe -q -cfile "test/grep/test_smoke_basic.grc"
if errorlevel 1 (
    echo FAILED: Basic functionality smoke test
    exit /b 1
)

echo Running core options tests...
.\grapa.exe -q -cfile "test/grep/test_smoke_options.grc"
if errorlevel 1 (
    echo FAILED: Core options smoke test
    exit /b 1
)

echo Running basic Unicode tests...
.\grapa.exe -q -cfile "test/grep/test_smoke_unicode.grc"
if errorlevel 1 (
    echo FAILED: Basic Unicode smoke test
    exit /b 1
)

echo Running basic custom delimiter tests...
.\grapa.exe -q -cfile "test/grep/test_smoke_custom_delimiters.grc"
if errorlevel 1 (
    echo FAILED: Basic custom delimiter smoke test
    exit /b 1
)

echo.
echo === SMOKE TESTS COMPLETE ===
echo All smoke tests passed successfully.
goto :end

:regression
echo === RUNNING REGRESSION TESTS (Daily) ===
echo Purpose: Ensure no regressions in working functionality
echo Execution time: < 5 minutes
echo.
echo Running core regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_core.grc"
if errorlevel 1 (
    echo FAILED: Core regression test
    exit /b 1
)

echo Running Unicode regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_unicode.grc"
if errorlevel 1 (
    echo FAILED: Unicode regression test
    exit /b 1
)

echo Running options regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_options.grc"
if errorlevel 1 (
    echo FAILED: Options regression test
    exit /b 1
)

echo Running custom delimiter regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_custom_delimiters.grc"
if errorlevel 1 (
    echo FAILED: Custom delimiter regression test
    exit /b 1
)

echo Running context regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_context.grc"
if errorlevel 1 (
    echo FAILED: Context regression test
    exit /b 1
)

echo Running error handling regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_error_handling.grc"
if errorlevel 1 (
    echo FAILED: Error handling regression test
    exit /b 1
)

echo Running edge cases regression tests...
.\grapa.exe -q -cfile "test/grep/test_regression_edge_cases.grc"
if errorlevel 1 (
    echo FAILED: Edge cases regression test
    exit /b 1
)

echo.
echo === REGRESSION TESTS COMPLETE ===
echo All regression tests passed successfully.
goto :end

:integration
echo === RUNNING INTEGRATION TESTS (Weekly) ===
echo Purpose: Test complex interactions and integration scenarios
echo Execution time: < 15 minutes
echo.
echo Running option combinations integration tests...
.\grapa.exe -q -cfile "test/grep/test_integration_option_combinations.grc"
if errorlevel 1 (
    echo FAILED: Option combinations integration test
    exit /b 1
)

echo Running advanced Unicode integration tests...
.\grapa.exe -q -cfile "test/grep/test_integration_unicode_advanced.grc"
if errorlevel 1 (
    echo FAILED: Advanced Unicode integration test
    exit /b 1
)

echo Running advanced custom delimiter integration tests...
.\grapa.exe -q -cfile "test/grep/test_integration_custom_delimiters_advanced.grc"
if errorlevel 1 (
    echo FAILED: Advanced custom delimiter integration test
    exit /b 1
)

echo Running performance integration tests...
.\grapa.exe -q -cfile "test/grep/test_integration_performance.grc"
if errorlevel 1 (
    echo FAILED: Performance integration test
    exit /b 1
)

echo Running ripgrep parity integration tests...
.\grapa.exe -q -cfile "test/grep/test_integration_ripgrep_parity.grc"
if errorlevel 1 (
    echo FAILED: Ripgrep parity integration test
    exit /b 1
)

echo Running large input integration tests...
.\grapa.exe -q -cfile "test/grep/test_integration_large_inputs.grc"
if errorlevel 1 (
    echo FAILED: Large input integration test
    exit /b 1
)

echo.
echo === INTEGRATION TESTS COMPLETE ===
echo All integration tests passed successfully.
goto :end

:comprehensive
echo === RUNNING COMPREHENSIVE TESTS (Monthly) ===
echo Purpose: Complete validation of all functionality
echo Execution time: < 30 minutes
echo.
echo Running comprehensive all features tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_all_features.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive all features test
    exit /b 1
)

echo Running comprehensive edge cases tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_edge_cases.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive edge cases test
    exit /b 1
)

echo Running comprehensive error scenarios tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_error_scenarios.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive error scenarios test
    exit /b 1
)

echo Running comprehensive Unicode full tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_unicode_full.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive Unicode full test
    exit /b 1
)

echo Running comprehensive custom delimiters full tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_custom_delimiters_full.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive custom delimiters full test
    exit /b 1
)

echo Running comprehensive option matrix tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_option_matrix.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive option matrix test
    exit /b 1
)

echo Running comprehensive performance full tests...
.\grapa.exe -q -cfile "test/grep/test_comprehensive_performance_full.grc"
if errorlevel 1 (
    echo FAILED: Comprehensive performance full test
    exit /b 1
)

echo.
echo === COMPREHENSIVE TESTS COMPLETE ===
echo All comprehensive tests passed successfully.
goto :end

:production
echo === RUNNING PRODUCTION READINESS TESTS (Pre-Release) ===
echo Purpose: Validate production readiness and mission-critical scenarios
echo Execution time: < 60 minutes
echo.
echo Running production stress tests...
.\grapa.exe -q -cfile "test/grep/test_production_stress.grc"
if errorlevel 1 (
    echo FAILED: Production stress test
    exit /b 1
)

echo Running production reliability tests...
.\grapa.exe -q -cfile "test/grep/test_production_reliability.grc"
if errorlevel 1 (
    echo FAILED: Production reliability test
    exit /b 1
)

echo Running production security tests...
.\grapa.exe -q -cfile "test/grep/test_production_security.grc"
if errorlevel 1 (
    echo FAILED: Production security test
    exit /b 1
)

echo Running production performance tests...
.\grapa.exe -q -cfile "test/grep/test_production_performance.grc"
if errorlevel 1 (
    echo FAILED: Production performance test
    exit /b 1
)

echo Running production enterprise tests...
.\grapa.exe -q -cfile "test/grep/test_production_enterprise.grc"
if errorlevel 1 (
    echo FAILED: Production enterprise test
    exit /b 1
)

echo Running production compatibility tests...
.\grapa.exe -q -cfile "test/grep/test_production_compatibility.grc"
if errorlevel 1 (
    echo FAILED: Production compatibility test
    exit /b 1
)

echo.
echo === PRODUCTION READINESS TESTS COMPLETE ===
echo All production readiness tests passed successfully.
goto :end

:development
echo === RUNNING DEVELOPMENT TESTS (During Development) ===
echo Purpose: Support development of new features and fixes
echo Execution time: Variable
echo.
echo Running custom delimiter fixes development tests...
.\grapa.exe -q -cfile "test/grep/test_development_custom_delimiters_fixes.grc"
if errorlevel 1 (
    echo FAILED: Custom delimiter fixes development test
    exit /b 1
)

echo Running Unicode fixes development tests...
.\grapa.exe -q -cfile "test/grep/test_development_unicode_fixes.grc"
if errorlevel 1 (
    echo FAILED: Unicode fixes development test
    exit /b 1
)

echo Running option fixes development tests...
.\grapa.exe -q -cfile "test/grep/test_development_option_fixes.grc"
if errorlevel 1 (
    echo FAILED: Option fixes development test
    exit /b 1
)

echo Running performance development tests...
.\grapa.exe -q -cfile "test/grep/test_development_performance.grc"
if errorlevel 1 (
    echo FAILED: Performance development test
    exit /b 1
)

echo Running new features development tests...
.\grapa.exe -q -cfile "test/grep/test_development_new_features.grc"
if errorlevel 1 (
    echo FAILED: New features development test
    exit /b 1
)

echo.
echo === DEVELOPMENT TESTS COMPLETE ===
echo All development tests passed successfully.
goto :end

:research
echo === RUNNING RESEARCH TESTS (As Needed) ===
echo Purpose: Investigate issues and validate hypotheses
echo Execution time: Variable
echo.
echo Running ripgrep comparison research tests...
.\grapa.exe -q -cfile "test/grep/test_research_ripgrep_comparison.grc"
if errorlevel 1 (
    echo FAILED: Ripgrep comparison research test
    exit /b 1
)

echo Running performance analysis research tests...
.\grapa.exe -q -cfile "test/grep/test_research_performance_analysis.grc"
if errorlevel 1 (
    echo FAILED: Performance analysis research test
    exit /b 1
)

echo Running Unicode behavior research tests...
.\grapa.exe -q -cfile "test/grep/test_research_unicode_behavior.grc"
if errorlevel 1 (
    echo FAILED: Unicode behavior research test
    exit /b 1
)

echo Running edge case investigation research tests...
.\grapa.exe -q -cfile "test/grep/test_research_edge_case_investigation.grc"
if errorlevel 1 (
    echo FAILED: Edge case investigation research test
    exit /b 1
)

echo Running option behavior research tests...
.\grapa.exe -q -cfile "test/grep/test_research_option_behavior.grc"
if errorlevel 1 (
    echo FAILED: Option behavior research test
    exit /b 1
)

echo.
echo === RESEARCH TESTS COMPLETE ===
echo All research tests passed successfully.
goto :end

:all
echo === RUNNING ALL TEST GROUPS (Full Validation) ===
echo Purpose: Complete validation of all functionality
echo Execution time: ~2 hours
echo.
echo This will run all test groups in sequence.
echo.

call :smoke
if errorlevel 1 goto :error

call :regression
if errorlevel 1 goto :error

call :integration
if errorlevel 1 goto :error

call :comprehensive
if errorlevel 1 goto :error

call :production
if errorlevel 1 goto :error

call :development
if errorlevel 1 goto :error

call :research
if errorlevel 1 goto :error

echo.
echo === ALL TEST GROUPS COMPLETE ===
echo All test groups passed successfully.
echo Full validation complete.
goto :end

:error
echo.
echo === TEST FAILURE DETECTED ===
echo One or more tests failed. Please review the output above.
exit /b 1

:end
echo.
echo Test group execution complete.
exit /b 0 