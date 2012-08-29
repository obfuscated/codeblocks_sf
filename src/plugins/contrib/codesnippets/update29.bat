@echo off
del ..\..\..\output29\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
zip -j9 ..\..\..\devel29\share\CodeBlocks\codesnippets.zip manifest.xml > nul 2>&1
md ..\..\..\devel29\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\devel29\share\CodeBlocks\images\codesnippets\ > nul 2>&1
md ..\..\..\output29\share\CodeBlocks\images\codesnippets > nul 2>&1
copy .\resources\*.png ..\..\..\output29\share\CodeBlocks\images\codesnippets\ > nul 2>&1
