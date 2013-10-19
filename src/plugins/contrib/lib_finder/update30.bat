@echo off
mkdir ..\..\..\devel30\share\CodeBlocks\lib_finder > nul 2>&1
mkdir ..\..\..\output30\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\devel30\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\output30\share\CodeBlocks\lib_finder > nul 2>&1
zip -j9 ..\..\..\devel30\share\CodeBlocks\lib_finder.zip manifest.xml
