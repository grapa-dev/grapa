@echo off
REM Grapa CLI Windows Basic Option Test (Non-Interactive Only)
setlocal
set GRAPA=..\..\grapa.exe
set LOG=cli_windows_basic.log

echo Testing .\grapa.exe -h > %LOG%
%GRAPA% -h >> %LOG% 2>&1
echo. >> %LOG%
echo Testing .\grapa.exe --help >> %LOG%
%GRAPA% --help >> %LOG% 2>&1
echo. >> %LOG%
echo Testing .\grapa.exe --version >> %LOG%
%GRAPA% --version >> %LOG% 2>&1
echo. >> %LOG%
echo Testing .\grapa.exe with invalid option >> %LOG%
%GRAPA% --notarealoption >> %LOG% 2>&1
echo. >> %LOG%
echo CLI Windows Basic Option Test Complete. >> %LOG%
endlocal 