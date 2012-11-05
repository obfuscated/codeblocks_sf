@echo off
md ..\..\..\devel29\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output29\share\CodeBlocks\images\settings > nul 2>&1
copy .\Resources\*.png ..\..\..\devel29\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\Resources\*.png ..\..\..\output29\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0
