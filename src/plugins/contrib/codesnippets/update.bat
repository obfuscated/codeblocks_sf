@echo off
del ..\..\..\output\share\CodeBlocks\plugins\codesnippets.exe > nul 2>&1
zip -j9 ..\..\..\devel\share\CodeBlocks\codesnippets.zip manifest.xml
