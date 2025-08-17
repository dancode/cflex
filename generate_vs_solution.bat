@echo off
REM This script generates a Visual Studio 2022 solution for the cflex project.

REM Define the source and build directories.
set SOURCE_DIR=cflex
set BUILD_DIR=%SOURCE_DIR%/build

echo Generating Visual Studio solution in %BUILD_DIR%...

REM Generate the Visual Studio solution for 64-bit builds.
cmake -B %BUILD_DIR% -S %SOURCE_DIR% -G "Visual Studio 17 2022" -A x64

REM Check if cmake was successful.
if %errorlevel% neq 0 (
    echo.
    echo CMake generation failed.
    pause
    exit /b %errorlevel%
)

echo.
echo Project generation complete.
echo You can now open the solution file: %BUILD_DIR%/cflex.sln
pause
