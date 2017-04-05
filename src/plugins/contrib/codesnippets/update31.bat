@echo off
del ..\..\..\output31\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
zip -j9 ..\..\..\devel31\share\CodeBlocks\codesnippets.zip manifest.xml > nul 2>&1
md ..\..\..\devel31\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\devel31\share\CodeBlocks\images\codesnippets\ > nul 2>&1
md ..\..\..\output31\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\output31\share\CodeBlocks\images\codesnippets\ > nul 2>&1
