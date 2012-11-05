@echo off
mkdir ..\..\..\devel29\share\CodeBlocks\lib_finder > nul 2>&1
mkdir ..\..\..\output29\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\devel29\share\CodeBlocks\lib_finder > nul 2>&1
copy /Y lib_finder\*.xml ..\..\..\output29\share\CodeBlocks\lib_finder > nul 2>&1
zip -j9 ..\..\..\devel29\share\CodeBlocks\lib_finder.zip manifest.xml
