@echo off
mkdir ..\..\..\devel\share\CodeBlocks\lib_finder > nul 2>&1
mkdir ..\..\..\output\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\devel\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\output\share\CodeBlocks\lib_finder > nul 2>&1
zip -j9 ..\..\..\devel\share\CodeBlocks\lib_finder.zip manifest.xml
