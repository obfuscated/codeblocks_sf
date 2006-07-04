@echo off
echo Creating output directory tree

if not exist output md output\
if not exist output\share md output\share\
if not exist output\share\CodeBlocks md output\share\CodeBlocks\
if not exist output\share\CodeBlocks\lexers md output\share\CodeBlocks\lexers\
if not exist output\share\CodeBlocks\images md output\share\CodeBlocks\images\
if not exist output\share\CodeBlocks\images\settings md output\share\CodeBlocks\images\settings\
if not exist output\share\CodeBlocks\images\16x16 md output\share\CodeBlocks\images\16x16\
if not exist output\share\CodeBlocks\images\codecompletion md output\share\CodeBlocks\images\codecompletion\
if not exist output\share\CodeBlocks\plugins md output\share\CodeBlocks\plugins\
if not exist output\share\CodeBlocks\templates md output\share\CodeBlocks\templates\
if not exist output\share\CodeBlocks\templates\wizard md output\share\CodeBlocks\templates\wizard\
if not exist output\share\CodeBlocks\scripts md output\share\CodeBlocks\scripts\
if not exist devel md devel\
if not exist devel\share md devel\share\
if not exist devel\share\CodeBlocks md devel\share\CodeBlocks\
if not exist devel\share\CodeBlocks\lexers md devel\share\CodeBlocks\lexers\
if not exist devel\share\CodeBlocks\images md devel\share\CodeBlocks\images\
if not exist devel\share\CodeBlocks\images\settings md devel\share\CodeBlocks\images\settings\
if not exist devel\share\CodeBlocks\images\16x16 md devel\share\CodeBlocks\images\16x16\
if not exist devel\share\CodeBlocks\images\codecompletion md devel\share\CodeBlocks\images\codecompletion\
if not exist devel\share\CodeBlocks\plugins md devel\share\CodeBlocks\plugins\
if not exist devel\share\CodeBlocks\templates md devel\share\CodeBlocks\templates\
if not exist devel\share\CodeBlocks\templates\wizard md devel\share\CodeBlocks\templates\wizard\
if not exist devel\share\CodeBlocks\scripts md devel\share\CodeBlocks\scripts\

set ZIPCMD=zip
set RESDIR=devel\share\CodeBlocks

echo Packing core UI resources
%ZIPCMD% -j9 %RESDIR%\resources.zip src\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\manager_resources.zip sdk\resources\*.xrc sdk\resources\images\*.png > nul
%ZIPCMD% -j9 %RESDIR%\start_here.zip src\resources\start_here\*.* > nul
echo Packing plugins UI resources
%ZIPCMD% -j9 %RESDIR%\astyle.zip plugins\astyle\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\plugin_wizard.zip plugins\pluginwizard\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\class_wizard.zip plugins\classwizard\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\code_completion.zip plugins\codecompletion\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\compiler_gcc.zip plugins\compilergcc\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\debugger_gdb.zip plugins\debuggergdb\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\defaultmimehandler.zip plugins\defaultmimehandler\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\todo.zip plugins\todo\resources\*.xrc > nul
%ZIPCMD% -j9 %RESDIR%\autosave.zip plugins\autosave\*.xrc > nul
echo Packing core UI bitmaps
cd src\resources
%ZIPCMD% -0 -q ..\..\%RESDIR%\resources.zip images\*.png images\16x16\*.png > nul
cd ..\..\sdk\resources
%ZIPCMD% -0 -q ..\..\%RESDIR%\manager_resources.zip images\*.png > nul
echo Packing plugins UI bitmaps
cd ..\..\plugins\compilergcc\resources
%ZIPCMD% -0 -q ..\..\..\%RESDIR%\compiler_gcc.zip images\*.png images\16x16\*.png > nul
cd ..\..\..\plugins\debuggergdb\resources
%ZIPCMD% -0 -q ..\..\..\%RESDIR%\debugger_gdb.zip images\*.png images\16x16\*.png > nul
cd ..\..\..

echo Copying external exception handler
copy /y setup\exchndl.dll output > nul
copy /y setup\exchndl.dll devel > nul
echo Copying files
copy /y %RESDIR%\*.zip output\share\codeblocks > nul
copy /y sdk\resources\lexers\lexer_* %RESDIR%\lexers > nul
copy /y sdk\resources\lexers\lexer_* output\share\codeblocks\lexers > nul
copy /y src\resources\images\*.png %RESDIR%\images > nul
copy /y src\resources\images\settings\*.png %RESDIR%\images\settings > nul
copy /y src\resources\images\*.png output\share\codeblocks\images > nul
copy /y src\resources\images\settings\*.png output\share\codeblocks\images\settings > nul
copy /y src\resources\images\16x16\*.png %RESDIR%\images\16x16 > nul
copy /y src\resources\images\16x16\*.png output\share\codeblocks\images\16x16 > nul
copy /y plugins\codecompletion\resources\images\*.png %RESDIR%\images\codecompletion > nul
copy /y plugins\codecompletion\resources\images\*.png output\share\codeblocks\images\codecompletion > nul
echo Makefile.am > excludes.txt
xcopy /y /s plugins\projectwizard\resources\* %RESDIR%\templates\wizard /EXCLUDE:excludes.txt >nul
xcopy /y /s plugins\projectwizard\resources\* output\share\codeblocks\templates\wizard /EXCLUDE:excludes.txt >nul
del excludes.txt
copy /y templates\common\* output\share\codeblocks\templates > nul
copy /y templates\win32\* output\share\codeblocks\templates > nul
copy /y templates\common\* %RESDIR%\templates > nul
copy /y templates\win32\* %RESDIR%\templates > nul
copy /y scripts\* %RESDIR%\scripts > nul
copy /y scripts\* output\share\codeblocks\scripts > nul
copy /y tips.txt devel\share\CodeBlocks > nul
copy /y tips.txt output\share\CodeBlocks > nul
copy /y tools\ConsoleRunner\cb_console_runner*.exe output > nul
copy /y tools\ConsoleRunner\cb_console_runner*.exe devel > nul
copy /y devel\*.exe output > nul
copy /y devel\*.dll output > nul
copy /y %RESDIR%\plugins\*.dll output\share\codeblocks\plugins > nul

echo Stripping debug info from output tree
strip output\*.exe
strip output\*.dll
strip output\share\CodeBlocks\plugins\*.dll

set ZIPCMD=
set RESDIR=
