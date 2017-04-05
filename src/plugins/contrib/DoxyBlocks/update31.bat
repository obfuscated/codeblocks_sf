@echo off
md   ..\..\..\devel31                                           > nul 2>&1
md   ..\..\..\output31                                          > nul 2>&1
md   ..\..\..\devel31\share                                     > nul 2>&1
md   ..\..\..\output31\share                                    > nul 2>&1
md   ..\..\..\devel31\share\CodeBlocks                          > nul 2>&1
md   ..\..\..\output31\share\CodeBlocks                         > nul 2>&1
md   ..\..\..\devel31\share\CodeBlocks\images                   > nul 2>&1
md   ..\..\..\output31\share\CodeBlocks\images                  > nul 2>&1
md   ..\..\..\devel31\share\CodeBlocks\images\settings          > nul 2>&1
md   ..\..\..\output31\share\CodeBlocks\images\settings         > nul 2>&1
md   ..\..\..\devel31\share\CodeBlocks\images\DoxyBlocks        > nul 2>&1
md   ..\..\..\output31\share\CodeBlocks\images\DoxyBlocks       > nul 2>&1
md   ..\..\..\devel31\share\CodeBlocks\images\DoxyBlocks\16x16  > nul 2>&1
md   ..\..\..\output31\share\CodeBlocks\images\DoxyBlocks\16x16 > nul 2>&1

copy images\*.png       ..\..\..\devel31\share\CodeBlocks\images\DoxyBlocks\        > nul 2>&1
copy images\16x16\*.png ..\..\..\devel31\share\CodeBlocks\images\DoxyBlocks\16x16\  > nul 2>&1
copy *.png              ..\..\..\devel31\share\CodeBlocks\images\settings\          > nul 2>&1

copy images\*.png       ..\..\..\output31\share\CodeBlocks\images\DoxyBlocks\       > nul 2>&1
copy images\16x16\*.png ..\..\..\output31\share\CodeBlocks\images\DoxyBlocks\16x16\ > nul 2>&1
copy *.png              ..\..\..\output31\share\CodeBlocks\images\settings\         > nul 2>&1

exit 0
