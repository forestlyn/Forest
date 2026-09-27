@echo off
setlocal
if /I "%~1"=="--help" goto help
if not "%~1"=="" goto usage_error

rem Always resolve source/build paths relative to this script.
pushd "%~dp0"
if errorlevel 1 exit /b 1

echo [1/3] Configure Visual Studio 2022 - Debug
cmake -S . -B build -G "Visual Studio 17 2022"
if errorlevel 1 goto failed

echo [2/3] Clean and rebuild all default targets and runtime resources
cmake --build build --config Debug --clean-first --parallel
if errorlevel 1 goto failed

echo [3/3] Build sample C# scripts using each project's ScriptBuildConfiguration
cmake --build build --config Debug --target Sandbox TestProject --parallel
if errorlevel 1 goto failed

echo.
echo Build succeeded.
echo Editor:  %CD%\build\bin\Debug\EngineEditor_debug.exe
echo Runtime: %CD%\build\runtime\Debug\ForestRuntime_debug.exe
popd
exit /b 0

:failed
set "forest_build_exit=%errorlevel%"
echo.
echo Build failed with exit code %forest_build_exit%.
popd
exit /b %forest_build_exit%

:usage_error
echo Unsupported argument: %~1
echo Usage: Rebuild_Debug.bat [--help]
exit /b 2

:help
echo Usage: Rebuild_Debug.bat [--help]
echo Rebuilds the complete Debug project, stages runtime resources,
echo and builds the Sandbox and TestProject C# scripts.
echo Script configuration comes from each .forestproj, independently of this BAT.
echo Requires CMake 3.21+, Visual Studio 2022 C++/C# tools,
echo initialized repository dependencies and VULKAN_SDK.
echo Close running Forest applications before rebuilding.
echo Integration test targets are not included.
exit /b 0
