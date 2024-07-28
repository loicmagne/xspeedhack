@echo off
setlocal

REM Define directories
set BUILD_DIR=build
set BIN_DIR=xspeedhack\bin

REM Create necessary directories
if not exist %BIN_DIR% mkdir %BIN_DIR%

REM Build for x86 architecture
cmake -S . -B %BUILD_DIR%\x86 -A Win32
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cmake --build %BUILD_DIR%\x86 --config Release
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

REM Build for x64 architecture
cmake -S . -B %BUILD_DIR%\x64 -A x64
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cmake --build %BUILD_DIR%\x64 --config Release
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

REM Copy executables
copy /y %BUILD_DIR%\x86\Release\injector.exe %BIN_DIR%\injector32.exe
copy /y %BUILD_DIR%\x86\Release\speedhack.dll %BIN_DIR%\speedhack32.dll
copy /y %BUILD_DIR%\x64\Release\injector.exe %BIN_DIR%\injector64.exe
copy /y %BUILD_DIR%\x64\Release\speedhack.dll %BIN_DIR%\speedhack64.dll
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

endlocal
@echo on
