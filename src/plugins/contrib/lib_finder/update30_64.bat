@echo off
mkdir ..\..\..\devel30_64\share\CodeBlocks\lib_finder > nul 2>&1
mkdir ..\..\..\output30_64\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\devel30_64\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\output30_64\share\CodeBlocks\lib_finder > nul 2>&1
zip -j9 ..\..\..\devel30_64\share\CodeBlocks\lib_finder.zip manifest.xml
