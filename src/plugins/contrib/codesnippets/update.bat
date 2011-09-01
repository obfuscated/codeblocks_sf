@echo off
del ..\..\..\output\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
zip -j9 ..\..\..\devel\share\CodeBlocks\codesnippets.zip manifest.xml > nul 2>&1
md ..\..\..\devel\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\devel\share\CodeBlocks\images\codesnippets\ > nul 2>&1
md ..\..\..\output\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\output\share\CodeBlocks\images\codesnippets\ > nul 2>&1
