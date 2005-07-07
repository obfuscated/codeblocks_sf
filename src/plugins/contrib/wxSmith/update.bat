@echo off
zip ..\..\..\devel\share\CodeBlocks\wxsmith.zip *.xrc
md ..\..\..\devel\share\CodeBlocks\images\wxsmith > nul 2>&1
copy smithicons\*.* ..\..\..\devel\share\CodeBlocks\images\wxsmith\ > nul 2>&1
