@set ZIPCMD=zip -j9

@echo Compressing core UI resources
@%ZIPCMD% devel\share\codeblocks\resources.zip src\resources\*.xrc > nul
@%ZIPCMD% devel\share\codeblocks\manager_resources.zip sdk\resources\*.xrc > nul
@echo Compressing plugins UI resources
@%ZIPCMD% devel\share\codeblocks\astyle.zip plugins\astyle\resources\*.xrc > nul
@%ZIPCMD% devel\share\codeblocks\plugin_wizard.zip plugins\pluginwizard\resources\*.xrc > nul
@%ZIPCMD% devel\share\codeblocks\class_wizard.zip plugins\classwizard\resources\*.xrc > nul
@%ZIPCMD% devel\share\codeblocks\code_completion.zip plugins\codecompletion\resources\*.xrc > nul
@%ZIPCMD% devel\share\codeblocks\compiler_gcc.zip plugins\compilergcc\resources\*.xrc > nul
@%ZIPCMD% devel\share\codeblocks\todo.zip plugins\todo\resources\*.xrc > nul

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

@set ZIPCMD=

