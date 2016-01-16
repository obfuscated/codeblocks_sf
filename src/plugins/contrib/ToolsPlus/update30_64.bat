@echo off
md ..\..\..\devel30_64\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output30_64\share\CodeBlocks\images\settings > nul 2>&1
copy .\Resources\*.png ..\..\..\devel30_64\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\Resources\*.png ..\..\..\output30_64\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0
