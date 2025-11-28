@echo off
echo ========================================
echo Acute Drawing Software - Build Script
echo ========================================
echo.

REM Set vcpkg path (adjust if your vcpkg is elsewhere)
set "VCPKG_ROOT=%CD%\vcpkg"
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo ERROR: vcpkg not found at: %VCPKG_ROOT%
    echo.
    echo Please either:
    echo   1. Clone vcpkg to this directory: git clone https://github.com/Microsoft/vcpkg.git
    echo   2. Or set VCPKG_ROOT to your vcpkg location
    echo.
    pause
    exit /b 1
)

echo Found vcpkg at: %VCPKG_ROOT%
echo.

REM Check if packages are installed
echo Checking for dependencies...
if not exist "%VCPKG_ROOT%\installed\x64-windows\share\sdl2\SDL2Config.cmake" (
    echo SDL2 not found. Installing dependencies...
    echo.
    cd "%VCPKG_ROOT%"
    call vcpkg.exe install
    if errorlevel 1 (
        echo.
        echo ERROR: Failed to install dependencies
        cd "%CD%"
        pause
        exit /b 1
    )
    cd "%CD%"
    echo.
) else (
    echo OK: Dependencies already installed
    echo.
)

REM Detect Visual Studio
set "VS_GEN="
if exist "C:\Program Files\Microsoft Visual Studio\18" (
    set "VS_GEN=Visual Studio 18 2026"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" (
    set "VS_GEN=Visual Studio 16 2019"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017" (
    set "VS_GEN=Visual Studio 15 2017"
) else (
    echo ERROR: Visual Studio not found!
    echo Please install Visual Studio 2026 Build Tools
    echo Download: https://visualstudio.microsoft.com/downloads/
    pause
    exit /b 1
)

echo Using generator: %VS_GEN%
echo.

REM Create build directory
if exist build (
    echo Cleaning old build...
    rmdir /s /q build
)
mkdir build
cd build

REM Configure with vcpkg toolchain
echo Configuring with CMake...
cmake .. -G "%VS_GEN%" -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"

if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed
    echo.
    echo Make sure:
    echo   1. vcpkg packages are installed: cd vcpkg ^&^& vcpkg.exe install
    echo   2. VCPKG_ROOT is set correctly
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config Release

if errorlevel 1 (
    echo.
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

cd ..
echo.
echo ========================================
echo SUCCESS! Build completed
echo ========================================
echo.
echo Executable: build\Release\AcuteDrawing.exe
echo.
echo To run:
echo   build\Release\AcuteDrawing.exe
echo.
pause

