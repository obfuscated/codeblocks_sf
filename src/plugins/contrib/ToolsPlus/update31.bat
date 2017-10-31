@echo off
md ..\..\..\devel31\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output31\share\CodeBlocks\images\settings > nul 2>&1
copy .\Resources\*.png ..\..\..\devel31\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\Resources\*.png ..\..\..\output31\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0
