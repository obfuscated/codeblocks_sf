@echo off

REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

echo Creating output29 directory tree

set CB_DEVEL_RESDIR=devel29\share\CodeBlocks
set CB_OUTPUT_RESDIR=output29\share\CodeBlocks

if not exist output29 md output29\
if not exist output29\share md output29\share\
if not exist %CB_OUTPUT_RESDIR% md %CB_OUTPUT_RESDIR%\
if not exist %CB_OUTPUT_RESDIR%\compilers md %CB_OUTPUT_RESDIR%\compilers\
if not exist %CB_OUTPUT_RESDIR%\lexers md %CB_OUTPUT_RESDIR%\lexers\
if not exist %CB_OUTPUT_RESDIR%\images md %CB_OUTPUT_RESDIR%\images\
if not exist %CB_OUTPUT_RESDIR%\images\settings md %CB_OUTPUT_RESDIR%\images\settings\
if not exist %CB_OUTPUT_RESDIR%\images\16x16 md %CB_OUTPUT_RESDIR%\images\16x16\
if not exist %CB_OUTPUT_RESDIR%\images\codecompletion md %CB_OUTPUT_RESDIR%\images\codecompletion\
if not exist %CB_OUTPUT_RESDIR%\plugins md %CB_OUTPUT_RESDIR%\plugins\
if not exist %CB_OUTPUT_RESDIR%\templates md %CB_OUTPUT_RESDIR%\templates\
if not exist %CB_OUTPUT_RESDIR%\templates\wizard md %CB_OUTPUT_RESDIR%\templates\wizard\
if not exist %CB_OUTPUT_RESDIR%\scripts md %CB_OUTPUT_RESDIR%\scripts\
if not exist devel29 md devel29\
if not exist devel29\share md devel29\share\
if not exist %CB_DEVEL_RESDIR% md %CB_DEVEL_RESDIR%\
if not exist %CB_DEVEL_RESDIR%\compilers md %CB_DEVEL_RESDIR%\compilers\
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

echo Compressing core UI resources
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\resources.zip src\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\manager_resources.zip sdk\resources\*.xrc sdk\resources\images\*.png > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\start_here.zip src\resources\start_here\*.html src\resources\start_here\*.png > nul
echo Compressing plugins UI resources
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\astyle.zip plugins\astyle\resources\manifest.xml plugins\astyle\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\autosave.zip plugins\autosave\manifest.xml plugins\autosave\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\classwizard.zip plugins\classwizard\resources\manifest.xml plugins\classwizard\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\codecompletion.zip plugins\codecompletion\resources\manifest.xml plugins\codecompletion\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\compiler.zip plugins\compilergcc\resources\manifest.xml plugins\compilergcc\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\debugger.zip plugins\debuggergdb\resources\manifest.xml plugins\debuggergdb\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\defaultmimehandler.zip plugins\defaultmimehandler\resources\manifest.xml plugins\defaultmimehandler\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\occurrenceshighlighting.zip plugins\occurrenceshighlighting\resources\*.xrc plugins\occurrenceshighlighting\resources\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\openfileslist.zip plugins\openfileslist\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\projectsimporter.zip plugins\projectsimporter\resources\manifest.xml plugins\projectsimporter\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\scriptedwizard.zip plugins\scriptedwizard\resources\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\todo.zip plugins\todo\resources\manifest.xml plugins\todo\resources\*.xrc > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\xpmanifest.zip plugins\xpmanifest\manifest.xml > nul
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\abbreviations.zip plugins\abbreviations\resources\manifest.xml plugins\abbreviations\resources\*.xrc > nul
echo Packing core UI bitmaps
cd src\resources
%ZIPCMD% -0 -qu ..\..\%CB_DEVEL_RESDIR%\resources.zip images\*.png images\12x12\*.png images\16x16\*.png > nul
cd ..\..\sdk\resources
%ZIPCMD% -0 -qu ..\..\%CB_DEVEL_RESDIR%\manager_resources.zip images\*.png images\12x12\*.png images\16x16\*.png > nul
echo Packing plugins UI bitmaps
cd ..\..\plugins\compilergcc\resources
%ZIPCMD% -0 -qu ..\..\..\%CB_DEVEL_RESDIR%\compiler.zip images\*.png images\16x16\*.png > nul
cd ..\..\..\plugins\debuggergdb\resources
%ZIPCMD% -0 -qu ..\..\..\%CB_DEVEL_RESDIR%\debugger.zip images\*.png images\16x16\*.png > nul
cd ..\..\..

echo Copying external exception handler
if exist devel29\exchndl.dll xcopy /D /y devel29\exchndl.dll output29 > nul
if exist devel29\exchndl.dll xcopy /D /y devel29\exchndl.dll devel29 > nul
echo Copying files
xcopy /D /y %CB_DEVEL_RESDIR%\*.zip %CB_OUTPUT_RESDIR% > nul
xcopy /D /y sdk\resources\lexers\lexer_* %CB_DEVEL_RESDIR%\lexers > nul
xcopy /D /y sdk\resources\lexers\lexer_* %CB_OUTPUT_RESDIR%\lexers > nul
xcopy /D /y src\resources\images\*.png %CB_DEVEL_RESDIR%\images > nul
xcopy /D /y src\resources\images\settings\*.png %CB_DEVEL_RESDIR%\images\settings > nul
xcopy /D /y src\resources\images\*.png %CB_OUTPUT_RESDIR%\images > nul
xcopy /D /y src\resources\images\settings\*.png %CB_OUTPUT_RESDIR%\images\settings > nul
xcopy /D /y src\resources\images\16x16\*.png %CB_DEVEL_RESDIR%\images\16x16 > nul
xcopy /D /y src\resources\images\16x16\*.png %CB_OUTPUT_RESDIR%\images\16x16 > nul
xcopy /D /y plugins\codecompletion\resources\images\*.png %CB_DEVEL_RESDIR%\images\codecompletion > nul
xcopy /D /y plugins\codecompletion\resources\images\*.png %CB_OUTPUT_RESDIR%\images\codecompletion > nul
xcopy /D /y plugins\compilergcc\resources\compilers\*.xml %CB_DEVEL_RESDIR%\compilers > nul
xcopy /D /y plugins\compilergcc\resources\compilers\*.xml %CB_OUTPUT_RESDIR%\compilers > nul
echo Makefile.am >  excludes.txt
echo Makefile.in >> excludes.txt
echo \.svn\      >> excludes.txt
echo *.gdb       >> excludes.txt
xcopy /D /y /s plugins\scriptedwizard\resources\* %CB_DEVEL_RESDIR%\templates\wizard /EXCLUDE:excludes.txt >nul
xcopy /D /y /s plugins\scriptedwizard\resources\* %CB_OUTPUT_RESDIR%\templates\wizard /EXCLUDE:excludes.txt >nul
xcopy /D /y templates\common\* %CB_OUTPUT_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /D /y templates\win32\* %CB_OUTPUT_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /D /y templates\common\* %CB_DEVEL_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /D /y templates\win32\* %CB_DEVEL_RESDIR%\templates /EXCLUDE:excludes.txt > nul
xcopy /D /y scripts\*.gdb %CB_OUTPUT_RESDIR% > nul
xcopy /D /y scripts\* %CB_DEVEL_RESDIR%\scripts /EXCLUDE:excludes.txt > nul
xcopy /D /y scripts\* %CB_OUTPUT_RESDIR%\scripts /EXCLUDE:excludes.txt > nul
del excludes.txt
xcopy /D /y tips.txt %CB_DEVEL_RESDIR% > nul
xcopy /D /y tips.txt %CB_OUTPUT_RESDIR% > nul
xcopy /D /y tools\ConsoleRunner\cb_console_runner29.exe output29 > nul
xcopy /D /y tools\ConsoleRunner\cb_console_runner29.exe devel29 > nul
if exist devel29\cb_console_runner.exe del devel29\cb_console_runner.exe > nul
ren devel29\cb_console_runner29.exe cb_console_runner.exe > nul
if exist output29\cb_console_runner.exe del output29\cb_console_runner.exe > nul
ren output29\cb_console_runner29.exe cb_console_runner.exe > nul
xcopy /D /y devel29\*.exe output29 > nul
xcopy /D /y devel29\*.dll output29 > nul
xcopy /D /y %CB_DEVEL_RESDIR%\plugins\*.dll %CB_OUTPUT_RESDIR%\plugins > nul

echo Stripping debug info from output tree
strip output29\*.exe
strip output29\*.dll
strip %CB_OUTPUT_RESDIR%\plugins\*.dll
