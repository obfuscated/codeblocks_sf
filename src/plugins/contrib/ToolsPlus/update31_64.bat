@echo off
md ..\..\..\devel31_64\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output31_64\share\CodeBlocks\images\settings > nul 2>&1
copy .\Resources\*.png ..\..\..\devel31_64\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\Resources\*.png ..\..\..\output31_64\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0
