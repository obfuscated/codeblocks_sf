@set ZIP=zip

@echo Compressing core UI resources
@cd src\resources && %ZIP% ..\..\devel\share\codeblocks\resources.zip *.xrc > nul
@cd ..\..
@cd sdk\resources && %ZIP% ..\..\devel\share\codeblocks\manager_resources.zip *.xrc > nul
@cd ..\..
@echo Compressing plugins UI resources
@cd plugins\astyle\resources && %ZIP% ..\..\..\devel\share\codeblocks\astyle.zip *.xrc > nul
@cd ..\..\..
@cd plugins\pluginwizard\resources && %ZIP% ..\..\..\devel\share\codeblocks\plugin_wizard.zip *.xrc > nul
@cd ..\..\..
@cd plugins\classwizard\resources && %ZIP% ..\..\..\devel\share\codeblocks\class_wizard.zip *.xrc > nul
@cd ..\..\..
@cd plugins\codecompletion\resources && %ZIP% ..\..\..\devel\share\codeblocks\code_completion.zip *.xrc > nul
@cd ..\..\..
@cd plugins\compilergcc\resources && %ZIP% ..\..\..\devel\share\codeblocks\compiler_gcc.zip *.xrc > nul
@cd ..\..\..
@cd plugins\todo\resources && %ZIP% ..\..\..\devel\share\codeblocks\todo.zip *.xrc > nul
@cd ..\..\..
@echo Copying files
@copy /y devel\share\codeblocks\*.zip output\share\codeblocks > nul
@copy /y src\resources\images\*.png devel\share\codeblocks\images > nul
@copy /y src\resources\images\*.png output\share\codeblocks\images > nul
@copy /y plugins\codecompletion\resources\images\*.png devel\share\codeblocks\images\codecompletion > nul
@copy /y plugins\codecompletion\resources\images\*.png output\share\codeblocks\images\codecompletion > nul
@copy /y templates\*.c* output\share\codeblocks\templates > nul
@copy /y templates\*.h* output\share\codeblocks\templates > nul
@copy /y templates\*.template output\share\codeblocks\templates > nul
@copy /y templates\*.png output\share\codeblocks\templates > nul
@copy /y templates\*.c* devel\share\codeblocks\templates > nul
@copy /y templates\*.h* devel\share\codeblocks\templates > nul
@copy /y templates\*.template devel\share\codeblocks\templates > nul
@copy /y templates\*.png devel\share\codeblocks\templates > nul
@copy /y tips.txt devel > nul
@copy /y tips.txt output > nul
@copy /y tools\ConsoleRunner\console_runner*.exe output > nul
@copy /y tools\ConsoleRunner\console_runner*.exe devel > nul
@copy /y devel\*.exe output > nul
@copy /y devel\*.dll output > nul
@copy /y devel\share\codeblocks\plugins\*.dll output\share\codeblocks\plugins > nul
@echo Stripping debug info from output tree
@strip output\*.exe
@strip output\*.dll
@strip output\share\CodeBlocks\plugins\*.dll

