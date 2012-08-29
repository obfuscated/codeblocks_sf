@echo off
cls
md ..\..\..\devel29\share\CodeBlocks\images\wxsmith > nul 2>&1
md ..\..\..\output29\share\CodeBlocks\images\wxsmith > nul 2>&1
zip ..\..\..\devel29\share\CodeBlocks\wxsmith.zip manifest.xml
zip ..\..\..\output29\share\CodeBlocks\wxsmith.zip manifest.xml
copy wxwidgets\icons\*.png ..\..\..\devel29\share\CodeBlocks\images\wxsmith\ > nul 2>&1
copy wxwidgets\icons\*.png ..\..\..\output29\share\CodeBlocks\images\wxsmith\ > nul 2>&1
exit 0
