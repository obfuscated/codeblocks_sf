@echo off

set CB_DEVEL_DIR=devel%1
set CB_OUTPUT_DIR=output%1
set CB_DEVEL_RESDIR=..\..\..\%CB_DEVEL_DIR%\share\CodeBlocks\images
set CB_OUTPUT_RESDIR=..\..\..\%CB_OUTPUT_DIR%\share\CodeBlocks\images
set CB_DEVEL_ZIPDIR=..\..\..\%CB_DEVEL_DIR%\share\CodeBlocks

md %CB_DEVEL_RESDIR%\settings > nul 2>&1
md %CB_OUTPUT_RESDIR%\settings > nul 2>&1
copy .\resources\*.png %CB_DEVEL_RESDIR%\settings\ > nul 2>&1
copy .\resources\*.png %CB_OUTPUT_RESDIR%\settings\ > nul 2>&1

zip -j9 %CB_DEVEL_ZIPDIR%\ThreadSearch.zip manifest.xml
cd resources/
zip -Rq9 ..\%CB_DEVEL_ZIPDIR%\ThreadSearch.zip ^
    images/16x16/*.png ^
    images/20x20/*.png ^
    images/24x24/*.png ^
    images/28x28/*.png ^
    images/32x32/*.png ^
    images/40x40/*.png ^
    images/48x48/*.png ^
    images/56x56/*.png ^
    images/64x64/*.png
cd ..

exit 0
