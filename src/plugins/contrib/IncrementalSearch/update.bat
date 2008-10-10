@echo off
md ..\..\..\devel\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output\share\CodeBlocks\images\settings > nul 2>&1
copy .\*.png ..\..\..\devel\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\*.png ..\..\..\output\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0

