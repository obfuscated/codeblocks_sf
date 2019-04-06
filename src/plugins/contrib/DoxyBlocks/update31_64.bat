@echo off

set CB_DIR=..\..\..\devel31_64\share\CodeBlocks

md %CB_DIR%\images\DoxyBlocks\16x16 > nul 2>&1
md %CB_DIR%\images\DoxyBlocks\22x22 > nul 2>&1
md %CB_DIR%\images\DoxyBlocks\32x32 > nul 2>&1
md %CB_DIR%\images\settings > nul 2>&1

copy images\32x32\*.png %CB_DIR%\images\DoxyBlocks\32x32\ > nul 2>&1
copy images\22x22\*.png %CB_DIR%\images\DoxyBlocks\22x22\ > nul 2>&1
copy images\16x16\*.png %CB_DIR%\images\DoxyBlocks\16x16\ > nul 2>&1
copy *.png              %CB_DIR%\images\settings\         > nul 2>&1

set CB_DIR=..\..\..\output31_64\share\CodeBlocks

md %CB_DIR%\images\DoxyBlocks\16x16 > nul 2>&1
md %CB_DIR%\images\DoxyBlocks\22x22 > nul 2>&1
md %CB_DIR%\images\DoxyBlocks\32x32 > nul 2>&1
md %CB_DIR%\images\settings > nul 2>&1

copy images\32x32\*.png %CB_DIR%\images\DoxyBlocks\32x32\ > nul 2>&1
copy images\22x22\*.png %CB_DIR%\images\DoxyBlocks\22x22\ > nul 2>&1
copy images\16x16\*.png %CB_DIR%\images\DoxyBlocks\16x16\ > nul 2>&1
copy *.png              %CB_DIR%\images\settings\         > nul 2>&1

exit 0
