@echo off
md ..\..\..\devel30\share\CodeBlocks\images\ThreadSearch > nul 2>&1
md ..\..\..\output30\share\CodeBlocks\images\ThreadSearch > nul 2>&1
md ..\..\..\devel30\share\CodeBlocks\images\ThreadSearch\16x16 > nul 2>&1
md ..\..\..\output30\share\CodeBlocks\images\ThreadSearch\16x16 > nul 2>&1
md ..\..\..\devel30\share\CodeBlocks\images\ThreadSearch\22x22 > nul 2>&1
md ..\..\..\output30\share\CodeBlocks\images\ThreadSearch\22x22 > nul 2>&1
md ..\..\..\devel30\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output30\share\CodeBlocks\images\settings > nul 2>&1
copy .\resources\images\*.png ..\..\..\devel30\share\CodeBlocks\images\ThreadSearch\ > nul 2>&1
copy .\resources\images\16x16\*.png ..\..\..\devel30\share\CodeBlocks\images\ThreadSearch\16x16\ > nul 2>&1
copy .\resources\images\22x22\*.png ..\..\..\devel30\share\CodeBlocks\images\ThreadSearch\22x22\ > nul 2>&1
copy .\resources\images\*.png ..\..\..\output30\share\CodeBlocks\images\ThreadSearch\ > nul 2>&1
copy .\resources\images\16x16\*.png ..\..\..\output30\share\CodeBlocks\images\ThreadSearch\16x16\ > nul 2>&1
copy .\resources\images\22x22\*.png ..\..\..\output30\share\CodeBlocks\images\ThreadSearch\22x22\ > nul 2>&1
copy .\resources\*.png ..\..\..\devel30\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\resources\*.png ..\..\..\output30\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0

