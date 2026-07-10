@echo off

rem Set output directory
set TARGET=Targets\%1

rem Create directory if it doesn't exist
if not exist "%TARGET%" mkdir "%TARGET%"

rem Activate virtual environment (Windows path for venv is Scripts instead of bin)
if exist "flow\Scripts\activate.bat" (
    call flow\Scripts\activate.bat
)

rem Install Conan dependencies using modern Conan 2 syntax
rem (Note: sudo=True was removed as it does not apply to Windows)
conan install . --output-folder="%TARGET%" --build=missing -s build_type=%1 -c tools.system.package_manager:mode=install

rem Generate CMake build files with the new toolchain location and compile commands
cmake -S . -B "%TARGET%" -DCMAKE_BUILD_TYPE=%1 -DCMAKE_TOOLCHAIN_FILE="%TARGET%\conan_toolchain.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

rem Fix for clangd LSP not finding compile_commands.json
rem Windows symlinks (mklink) require Admin rights, so copying is the safest and easiest workaround
if exist "%TARGET%\compile_commands.json" (
    copy /Y "%TARGET%\compile_commands.json" compile_commands.json >nul
)

rem Compile CMake build files
cmake --build "%TARGET%" --config %1
