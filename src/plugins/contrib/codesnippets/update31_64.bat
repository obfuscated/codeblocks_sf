@echo off
del ..\..\..\output31_64\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
zip -j9 ..\..\..\devel31_64\share\CodeBlocks\codesnippets.zip manifest.xml > nul 2>&1
md ..\..\..\devel31_64\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\devel31_64\share\CodeBlocks\images\codesnippets\ > nul 2>&1
md ..\..\..\output31_64\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\output31_64\share\CodeBlocks\images\codesnippets\ > nul 2>&1
