@echo off
cls
md ..\..\..\devel\share\CodeBlocks\images\wxsmith > nul 2>&1
md ..\..\..\output\share\CodeBlocks\images\wxsmith > nul 2>&1
zip ..\..\..\devel\share\CodeBlocks\wxsmith.zip manifest.xml
zip ..\..\..\output\share\CodeBlocks\wxsmith.zip manifest.xml
copy wxwidgets\icons\*.png ..\..\..\devel\share\CodeBlocks\images\wxsmith\ > nul 2>&1
copy wxwidgets\icons\*.png ..\..\..\output\share\CodeBlocks\images\wxsmith\ > nul 2>&1
exit 0
