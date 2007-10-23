@echo off

REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

echo Creating output directory tree

set CB_DEVEL_RESDIR=devel\share\CodeBlocks
set CB_OUTPUT_RESDIR=output\share\CodeBlocks

if not exist output md output\
if not exist output\share md output\share\
if not exist %CB_OUTPUT_RESDIR% md %CB_OUTPUT_RESDIR%\
if not exist %CB_OUTPUT_RESDIR%\lexers md %CB_OUTPUT_RESDIR%\lexers\
if not exist %CB_OUTPUT_RESDIR%\images md %CB_OUTPUT_RESDIR%\images\
if not exist %CB_OUTPUT_RESDIR%\images\settings md %CB_OUTPUT_RESDIR%\images\settings\
if not exist %CB_OUTPUT_RESDIR%\images\16x16 md %CB_OUTPUT_RESDIR%\images\16x16\
if not exist %CB_OUTPUT_RESDIR%\images\codecompletion md %CB_OUTPUT_RESDIR%\images\codecompletion\
if not exist %CB_OUTPUT_RESDIR%\plugins md %CB_OUTPUT_RESDIR%\plugins\
if not exist %CB_OUTPUT_RESDIR%\templates md %CB_OUTPUT_RESDIR%\templates\
if not exist %CB_OUTPUT_RESDIR%\templates\wizard md %CB_OUTPUT_RESDIR%\templates\wizard\
if not exist %CB_OUTPUT_RESDIR%\scripts md %CB_OUTPUT_RESDIR%\scripts\
if not exist devel md devel\
if not exist devel\share md devel\share\
if not exist %CB_DEVEL_RESDIR% md %CB_DEVEL_RESDIR%\
if not exist %CB_DEVEL_RESDIR%\lexers md %CB_DEVEL_RESDIR%\lexers\
if not exist %CB_DEVEL_RESDIR%\images md %CB_DEVEL_RESDIR%\images\
if not exist %CB_DEVEL_RESDIR%\images\settings md %CB_DEVEL_RESDIR%\images\settings\
if not exist %CB_DEVEL_RESDIR%\images\16x16 md %CB_DEVEL_RESDIR%\images\16x16\
if not exist %CB_DEVEL_RESDIR%\images\codecompletion md %CB_DEVEL_RESDIR%\images\codecompletion\
if not exist %CB_DEVEL_RESDIR%\plugins md %CB_DEVEL_RESDIR%\plugins\
if not exist %CB_DEVEL_RESDIR%\templates md %CB_DEVEL_RESDIR%\templates\
if not exist %CB_DEVEL_RESDIR%\templates\wizard md %CB_DEVEL_RESDIR%\templates\wizard\
if not exist %CB_DEVEL_RESDIR%\scripts md %CB_DEVEL_RESDIR%\scripts\

set ZIPCMD=zip

echo Packing core UI resources
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\resources.zip src\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\manager_resources.zip sdk\resources\*.xrc sdk\resources\images\*.png > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\start_here.zip src\resources\start_here\*.* > nul
echo Packing plugins UI resources
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\astyle.zip plugins\astyle\resources\manifest.xml plugins\astyle\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\autosave.zip plugins\autosave\manifest.xml plugins\autosave\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\classwizard.zip plugins\classwizard\resources\manifest.xml plugins\classwizard\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\codecompletion.zip plugins\codecompletion\resources\manifest.xml plugins\codecompletion\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\compiler.zip plugins\compilergcc\resources\manifest.xml plugins\compilergcc\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\debugger.zip plugins\debuggergdb\resources\manifest.xml plugins\debuggergdb\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\defaultmimehandler.zip plugins\defaultmimehandler\resources\manifest.xml plugins\defaultmimehandler\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\projectsimporter.zip plugins\projectsimporter\resources\manifest.xml plugins\projectsimporter\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\scriptedwizard.zip plugins\scriptedwizard\resources\manifest.xml > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\todo.zip plugins\todo\resources\manifest.xml plugins\todo\resources\*.xrc > nul
%ZIPCMD% -j9 %CB_DEVEL_RESDIR%\xpmanifest.zip plugins\xpmanifest\manifest.xml > nul
echo Packing core UI bitmaps
cd src\resources
%ZIPCMD% -0 -q ..\..\%CB_DEVEL_RESDIR%\resources.zip images\*.png images\16x16\*.png > nul
cd ..\..\sdk\resources
%ZIPCMD% -0 -q ..\..\%CB_DEVEL_RESDIR%\manager_resources.zip images\*.png > nul
echo Packing plugins UI bitmaps
cd ..\..\plugins\compilergcc\resources
%ZIPCMD% -0 -q ..\..\..\%CB_DEVEL_RESDIR%\compiler.zip images\*.png images\16x16\*.png > nul
cd ..\..\..\plugins\debuggergdb\resources
%ZIPCMD% -0 -q ..\..\..\%CB_DEVEL_RESDIR%\debugger.zip images\*.png images\16x16\*.png > nul
cd ..\..\..

echo Copying external exception handler
copy /y setup\exchndl.dll output > nul
copy /y setup\exchndl.dll devel > nul
echo Copying files
copy /y %CB_DEVEL_RESDIR%\*.zip %CB_OUTPUT_RESDIR% > nul
copy /y sdk\resources\lexers\lexer_* %CB_DEVEL_RESDIR%\lexers > nul
copy /y sdk\resources\lexers\lexer_* %CB_OUTPUT_RESDIR%\lexers > nul
copy /y src\resources\images\*.png %CB_DEVEL_RESDIR%\images > nul
copy /y src\resources\images\settings\*.png %CB_DEVEL_RESDIR%\images\settings > nul
copy /y src\resources\images\*.png %CB_OUTPUT_RESDIR%\images > nul
copy /y src\resources\images\settings\*.png %CB_OUTPUT_RESDIR%\images\settings > nul
copy /y src\resources\images\16x16\*.png %CB_DEVEL_RESDIR%\images\16x16 > nul
copy /y src\resources\images\16x16\*.png %CB_OUTPUT_RESDIR%\images\16x16 > nul
copy /y plugins\codecompletion\resources\images\*.png %CB_DEVEL_RESDIR%\images\codecompletion > nul
copy /y plugins\codecompletion\resources\images\*.png %CB_OUTPUT_RESDIR%\images\codecompletion > nul
echo Makefile.am > excludes.txt
echo \.svn\ >> excludes.txt
xcopy /y /s plugins\scriptedwizard\resources\* %CB_DEVEL_RESDIR%\templates\wizard /EXCLUDE:excludes.txt >nul
xcopy /y /s plugins\scriptedwizard\resources\* %CB_OUTPUT_RESDIR%\templates\wizard /EXCLUDE:excludes.txt >nul
xcopy /y templates\common\* %CB_OUTPUT_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /y templates\win32\* %CB_OUTPUT_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /y templates\common\* %CB_DEVEL_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /y templates\win32\* %CB_DEVEL_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /y scripts\* %CB_DEVEL_RESDIR%\scripts /EXCLUDE:excludes.txt > nul
xcopy /y scripts\* %CB_OUTPUT_RESDIR%\scripts /EXCLUDE:excludes.txt > nul
del excludes.txt
copy /y tips.txt %CB_DEVEL_RESDIR% > nul
copy /y tips.txt %CB_OUTPUT_RESDIR% > nul
copy /y tools\ConsoleRunner\cb_console_runner*.exe output > nul
copy /y tools\ConsoleRunner\cb_console_runner*.exe devel > nul
copy /y devel\*.exe output > nul
copy /y devel\*.dll output > nul
copy /y %CB_DEVEL_RESDIR%\plugins\*.dll %CB_OUTPUT_RESDIR%\plugins > nul

echo Stripping debug info from output tree
strip output\*.exe
strip output\*.dll
strip %CB_OUTPUT_RESDIR%\plugins\*.dll
