@echo off
setlocal

REM Set the repository URL
set "REPO_URL=https://github.com/tianocore/edk2.git"

REM Get the directory of this script
set "SCRIPT_DIR=%~dp0"

REM Clone the repository into a temporary directory
set "TEMP_DIR=%SCRIPT_DIR%_tmp"
git clone -b edk2-stable202411 --recursive "%REPO_URL%" "%TEMP_DIR%"

REM Copy the contents of the temporary directory to the script directory
xcopy "%TEMP_DIR%\*" "%SCRIPT_DIR%" /s /e /h /y

REM Delete the temporary directory
rmdir /s /q "%TEMP_DIR%"

REM Optionally, remove the .git directory in the script directory
rmdir /s /q "%SCRIPT_DIR%.git"

endlocal
