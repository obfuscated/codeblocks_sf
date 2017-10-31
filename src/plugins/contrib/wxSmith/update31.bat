@echo off
cls
md ..\..\..\devel31\share\CodeBlocks\images\wxsmith > nul 2>&1
md ..\..\..\output31\share\CodeBlocks\images\wxsmith > nul 2>&1
zip ..\..\..\devel31\share\CodeBlocks\wxsmith.zip manifest.xml
zip ..\..\..\output31\share\CodeBlocks\wxsmith.zip manifest.xml
copy wxwidgets\icons\*.png ..\..\..\devel31\share\CodeBlocks\images\wxsmith\ > nul 2>&1
copy wxwidgets\icons\*.png ..\..\..\output31\share\CodeBlocks\images\wxsmith\ > nul 2>&1
exit 0
