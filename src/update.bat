@echo off

REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

SETLOCAL ENABLEEXTENSIONS

echo Creating output directory tree

set CB_DEVEL_DIR=devel%1%
set CB_OUTPUT_DIR=output%1%
set CB_DEVEL_RESDIR=%CB_DEVEL_DIR%\share\CodeBlocks
set CB_OUTPUT_RESDIR=%CB_OUTPUT_DIR%\share\CodeBlocks
set CB_HANDLER_DIR=exchndl\win32\bin
set %i% | find "_64" > nul && set CB_HANDLER_DIR=exchndl\win64\bin

call:mkdirSilent %CB_DEVEL_RESDIR%\compilers
call:mkdirSilent %CB_DEVEL_RESDIR%\lexers
call:mkdirSilent %CB_DEVEL_RESDIR%\images\settings
call:mkdirSilent %CB_DEVEL_RESDIR%\images\codecompletion
call:mkdirSilent %CB_DEVEL_RESDIR%\images\12x12
call:mkdirSilent %CB_DEVEL_RESDIR%\images\16x16
call:mkdirSilent %CB_DEVEL_RESDIR%\images\22x22
call:mkdirSilent %CB_DEVEL_RESDIR%\images\32x32
call:mkdirSilent %CB_DEVEL_RESDIR%\plugins
call:mkdirSilent %CB_DEVEL_RESDIR%\templates
call:mkdirSilent %CB_DEVEL_RESDIR%\templates\wizard
call:mkdirSilent %CB_DEVEL_RESDIR%\scripts\tests

call:mkdirSilent %CB_OUTPUT_RESDIR%\compilers
call:mkdirSilent %CB_OUTPUT_RESDIR%\lexers
call:mkdirSilent %CB_OUTPUT_RESDIR%\images\settings
call:mkdirSilent %CB_OUTPUT_RESDIR%\images\codecompletion
call:mkdirSilent %CB_OUTPUT_RESDIR%\images\12x12
call:mkdirSilent %CB_OUTPUT_RESDIR%\images\16x16
call:mkdirSilent %CB_OUTPUT_RESDIR%\images\22x22
call:mkdirSilent %CB_OUTPUT_RESDIR%\images\32x32
call:mkdirSilent %CB_OUTPUT_RESDIR%\plugins
call:mkdirSilent %CB_OUTPUT_RESDIR%\templates
call:mkdirSilent %CB_OUTPUT_RESDIR%\templates\wizard
call:mkdirSilent %CB_OUTPUT_RESDIR%\scripts\tests

rem exit /b

set ZIPCMD=zip

echo Compressing core UI resources
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\resources.zip src\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\manager_resources.zip sdk\resources\*.xrc sdk\resources\images\*.png > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\start_here.zip src\resources\start_here\* > nul
echo Compressing plugins UI resources
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\Astyle.zip plugins\astyle\resources\manifest.xml plugins\astyle\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\autosave.zip plugins\autosave\manifest.xml plugins\autosave\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\classwizard.zip plugins\classwizard\resources\manifest.xml plugins\classwizard\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\codecompletion.zip plugins\codecompletion\resources\manifest.xml plugins\codecompletion\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\compiler.zip plugins\compilergcc\resources\manifest.xml plugins\compilergcc\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\debugger.zip plugins\debuggergdb\resources\manifest.xml plugins\debuggergdb\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\defaultmimehandler.zip plugins\defaultmimehandler\resources\manifest.xml plugins\defaultmimehandler\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\occurrenceshighlighting.zip plugins\occurrenceshighlighting\resources\*.xrc plugins\occurrenceshighlighting\resources\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\openfileslist.zip plugins\openfileslist\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\projectsimporter.zip plugins\projectsimporter\resources\*.xrc plugins\projectsimporter\resources\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\scriptedwizard.zip plugins\scriptedwizard\resources\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\todo.zip plugins\todo\resources\manifest.xml plugins\todo\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\abbreviations.zip plugins\abbreviations\resources\manifest.xml plugins\abbreviations\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\xpmanifest.zip plugins\xpmanifest\manifest.xml > nul
echo Packing core UI bitmaps
cd src\resources
%ZIPCMD% -0 -qu ..\..\%CB_DEVEL_RESDIR%\resources.zip images\*.png images\12x12\*.png images\16x16\*.png images\22x22\*.png  images\32x32\*.png> nul
cd ..\..\sdk\resources
%ZIPCMD% -0 -qu ..\..\%CB_DEVEL_RESDIR%\manager_resources.zip images\*.png images\12x12\*.png images\16x16\*.png > nul
echo Packing plugins UI bitmaps
cd ..\..\plugins\compilergcc\resources
%ZIPCMD% -0 -qu ..\..\..\%CB_DEVEL_RESDIR%\compiler.zip images\16x16\*.png images\22x22\*.png images\32x32\*.png > nul
cd ..\..\..

echo Copying files
REM  Create an exclude pattern file
echo \.svn\      > excludes%1%.txt
echo Makefile >> excludes%1%.txt
echo Makefile.am >> excludes%1%.txt
echo Makefile.in >> excludes%1%.txt

xcopy /D /y sdk\resources\lexers\lexer_* %CB_DEVEL_RESDIR%\lexers > nul
xcopy /D /y src\resources\images\*.png %CB_DEVEL_RESDIR%\images > nul
xcopy /D /y src\resources\images\settings\*.png %CB_DEVEL_RESDIR%\images\settings > nul
xcopy /D /y src\resources\images\16x16\*.png %CB_DEVEL_RESDIR%\images\16x16 > nul
xcopy /D /y src\resources\images\22x22\*.png %CB_DEVEL_RESDIR%\images\22x22 > nul
xcopy /D /y src\resources\images\32x32\*.png %CB_DEVEL_RESDIR%\images\32x32 > nul
xcopy /D /y plugins\codecompletion\resources\images\*.png %CB_DEVEL_RESDIR%\images\codecompletion > nul
xcopy /D /y plugins\compilergcc\resources\compilers\*.xml %CB_DEVEL_RESDIR%\compilers > nul
xcopy /D /y /s plugins\scriptedwizard\resources\* %CB_DEVEL_RESDIR%\templates\wizard /EXCLUDE:excludes%1%.txt >nul
xcopy /D /y templates\common\* %CB_DEVEL_RESDIR%\templates /EXCLUDE:excludes%1%.txt > nul
xcopy /D /y templates\win32\* %CB_DEVEL_RESDIR%\templates /EXCLUDE:excludes%1%.txt > nul
xcopy /D /y scripts\* %CB_DEVEL_RESDIR%\scripts /EXCLUDE:excludes%1%.txt > nul
xcopy /D /y scripts\tests\* %CB_DEVEL_RESDIR%\scripts\tests /EXCLUDE:excludes%1%.txt > nul

xcopy /D /y %CB_DEVEL_RESDIR%\*.zip %CB_OUTPUT_RESDIR% > nul
xcopy /D /y sdk\resources\lexers\lexer_* %CB_OUTPUT_RESDIR%\lexers > nul
xcopy /D /y src\resources\images\*.png %CB_OUTPUT_RESDIR%\images > nul
xcopy /D /y src\resources\images\settings\*.png %CB_OUTPUT_RESDIR%\images\settings > nul
xcopy /D /y src\resources\images\16x16\*.png %CB_OUTPUT_RESDIR%\images\16x16 > nul
xcopy /D /y src\resources\images\22x22\*.png %CB_OUTPUT_RESDIR%\images\22x22 > nul
xcopy /D /y src\resources\images\32x32\*.png %CB_OUTPUT_RESDIR%\images\32x32 > nul
xcopy /D /y plugins\codecompletion\resources\images\*.png %CB_OUTPUT_RESDIR%\images\codecompletion > nul
xcopy /D /y plugins\compilergcc\resources\compilers\*.xml %CB_OUTPUT_RESDIR%\compilers > nul
xcopy /D /y /s plugins\scriptedwizard\resources\* %CB_OUTPUT_RESDIR%\templates\wizard /EXCLUDE:excludes%1%.txt >nul
xcopy /D /y templates\common\* %CB_OUTPUT_RESDIR%\templates /EXCLUDE:excludes%1%.txt > nul
xcopy /D /y templates\win32\* %CB_OUTPUT_RESDIR%\templates /EXCLUDE:excludes%1%.txt > nul
xcopy /D /y scripts\tests\* %CB_OUTPUT_RESDIR%\scripts\tests /EXCLUDE:excludes%1%.txt > nul
xcopy /D /y scripts\* %CB_OUTPUT_RESDIR%\scripts /EXCLUDE:excludes%1%.txt > nul

del excludes%1%.txt

REM several contrib plugins
if exist %CB_DEVEL_RESDIR%\images\codesnippets (
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\codesnippets
    xcopy /D /y %CB_DEVEL_RESDIR%\images\codesnippets\*.png %CB_OUTPUT_RESDIR%\images\codesnippets > nul
)

if exist %CB_DEVEL_RESDIR%\images\DoxyBlocks (
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\DoxyBlocks\32x32
    xcopy /D /y %CB_DEVEL_RESDIR%\images\DoxyBlocks\32x32\*.png %CB_OUTPUT_RESDIR%\images\DoxyBlocks/32x32 > nul
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\DoxyBlocks\22x22
    xcopy /D /y %CB_DEVEL_RESDIR%\images\DoxyBlocks\22x22\*.png %CB_OUTPUT_RESDIR%\images\DoxyBlocks/22x22 > nul
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\DoxyBlocks\16x16
    xcopy /D /y %CB_DEVEL_RESDIR%\images\DoxyBlocks\16x16\*.png %CB_OUTPUT_RESDIR%\images\DoxyBlocks/16x16 > nul
)

if exist %CB_DEVEL_RESDIR%\images\ThreadSearch (
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\ThreadSearch\32x32
    xcopy /D /y %CB_DEVEL_RESDIR%\images\ThreadSearch\32x32\*.png %CB_OUTPUT_RESDIR%\images\ThreadSearch/32x32 > nul
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\ThreadSearch\22x22
    xcopy /D /y %CB_DEVEL_RESDIR%\images\ThreadSearch\22x22\*.png %CB_OUTPUT_RESDIR%\images\ThreadSearch/22x22 > nul
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\ThreadSearch\16x16
    xcopy /D /y %CB_DEVEL_RESDIR%\images\ThreadSearch\16x16\*.png %CB_OUTPUT_RESDIR%\images\ThreadSearch/16x16 > nul
)

if exist %CB_DEVEL_RESDIR%\images\wxsmith (
    call:mkdirSilent %CB_OUTPUT_RESDIR%\images\wxsmith
    xcopy /D /y %CB_DEVEL_RESDIR%\images\wxsmith\*.png %CB_OUTPUT_RESDIR%\images\wxsmith > nul
)

if exist %CB_DEVEL_RESDIR%\lib_finder (
    call:mkdirSilent %CB_OUTPUT_RESDIR%\lib_finder
    xcopy /D /y %CB_DEVEL_RESDIR%\lib_finder\*.xml %CB_OUTPUT_RESDIR%\lib_finder > nul
)

REM misc. contrib plugin settings:
xcopy /D /y %CB_DEVEL_RESDIR%\images\16x16\*.png    %CB_OUTPUT_RESDIR%\images\16x16    > nul
xcopy /D /y %CB_DEVEL_RESDIR%\images\22x22\*.png    %CB_OUTPUT_RESDIR%\images\22x22    > nul
xcopy /D /y %CB_DEVEL_RESDIR%\images\32x32\*.png    %CB_OUTPUT_RESDIR%\images\32x32    > nul
xcopy /D /y %CB_DEVEL_RESDIR%\images\settings\*.png %CB_OUTPUT_RESDIR%\images\settings > nul

REM =============================================
REM =============================================
REM =============================================
REM =============================================

copy tips.txt %CB_DEVEL_RESDIR% > nul
copy tools\ConsoleRunner\cb_console_runner%1%.exe %CB_DEVEL_DIR%\cb_console_runner.exe > nul

copy tips.txt %CB_OUTPUT_RESDIR% > nul
copy tools\ConsoleRunner\cb_console_runner%1%.exe %CB_OUTPUT_DIR%\cb_console_runner.exe > nul

xcopy /D /y %CB_DEVEL_DIR%\*.exe %CB_OUTPUT_DIR% > nul
xcopy /D /y %CB_DEVEL_DIR%\*.dll %CB_OUTPUT_DIR% > nul
xcopy /D /y %CB_DEVEL_RESDIR%\plugins\*.dll %CB_OUTPUT_RESDIR%\plugins > nul

echo Stripping debug info from output tree
strip %CB_OUTPUT_DIR%\*.exe
strip %CB_OUTPUT_DIR%\*.dll
strip %CB_OUTPUT_RESDIR%\plugins\*.dll

REM Copy these files later as stripping symbols would corrupt them
echo Copying crash handler files
xcopy /y %CB_HANDLER_DIR%\*.dll %CB_DEVEL_DIR%  > nul
xcopy /y %CB_HANDLER_DIR%\*.yes %CB_DEVEL_DIR%  > nul
xcopy /y %CB_HANDLER_DIR%\*.dll %CB_OUTPUT_DIR% > nul
xcopy /y %CB_HANDLER_DIR%\*.yes %CB_OUTPUT_DIR% > nul


::--------------------------------------------------------
::-- Function section starts below here
::--------------------------------------------------------
GOTO:EOF

:mkdirSilent - create a directory if it doesn't exists
echo make dir %~1
if not exist "%~1" mkdir "%~1"
GOTO:EOF
