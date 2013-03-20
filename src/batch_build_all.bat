@echo off
rem ----------------------------------------
rem Setup C::B root folder of *binaries* (!)
rem ----------------------------------------
if not defined CB_ROOT set CB_ROOT=C:\Devel\CodeBlocks
rem ------------------------------------------
rem Setup GCC root folder with "bin" subfolder
rem ------------------------------------------
if not defined GCC_ROOT set GCC_ROOT=C:\Devel\GCC46TDM
rem change this name to suit your needs
if not defined CB_RUN_UPDATE_BAT set CB_RUN_UPDATE_BAT=0

rem -------------------------------------------
rem Usually below here no changes are required.
rem -------------------------------------------
if not exist "%CB_ROOT%"  goto ErrNoCB
if not exist "%GCC_ROOT%" goto ErrNoGCC
rem quotes are here to prevent problems with spaces in paths
set "PATH=%CB_ROOT%;%GCC_ROOT%;%PATH%"

set BUILD_TYPE=--build
if "%1"=="r"        set BUILD_TYPE=--rebuild
if "%1"=="-r"       set BUILD_TYPE=--rebuild
if "%1"=="rebuild"  set BUILD_TYPE=--rebuild
if "%1"=="-rebuild" set BUILD_TYPE=--rebuild

if not defined START_CMD set START_CMD=start "Code::Blocks Build" /D"%~dp0" /min /b
set CB_EXE="%CB_ROOT%\codeblocks.exe"
if not defined CB_PARAMS set CB_PARAMS=--batch-build-notify --no-batch-window-close
set CB_CMD=%BUILD_TYPE% "%~dp0CodeBlocks.workspace"

if not defined CB_TARGET set CB_TARGET=--target=All
%START_CMD% %CB_EXE% %CB_PARAMS% %CB_TARGET% %CB_CMD%
echo.
if %CB_RUN_UPDATE_BAT% NEQ 0 call "%~dp0update.bat" else echo Do not forget to run "update.bat" after successful build!
goto TheEnd

:ErrNoCB
echo Error: C::B root folder not found. Adjust batch file accordingly
goto TheEnd

:ErrNoGCC
echo Error: GCC root folder not found. Adjust batch file accordingly
goto TheEnd

:TheEnd
