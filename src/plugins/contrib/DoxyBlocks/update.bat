@echo off
md   ..\..\..\devel                                           > nul 2>&1
md   ..\..\..\output                                          > nul 2>&1
md   ..\..\..\devel\share                                     > nul 2>&1
md   ..\..\..\output\share                                    > nul 2>&1
md   ..\..\..\devel\share\CodeBlocks                          > nul 2>&1
md   ..\..\..\output\share\CodeBlocks                         > nul 2>&1
md   ..\..\..\devel\share\CodeBlocks\images                   > nul 2>&1
md   ..\..\..\output\share\CodeBlocks\images                  > nul 2>&1
md   ..\..\..\devel\share\CodeBlocks\images\settings          > nul 2>&1
md   ..\..\..\output\share\CodeBlocks\images\settings         > nul 2>&1
md   ..\..\..\devel\share\CodeBlocks\images\DoxyBlocks        > nul 2>&1
md   ..\..\..\output\share\CodeBlocks\images\DoxyBlocks       > nul 2>&1
md   ..\..\..\devel\share\CodeBlocks\images\DoxyBlocks\16x16  > nul 2>&1
md   ..\..\..\output\share\CodeBlocks\images\DoxyBlocks\16x16 > nul 2>&1

copy images\*.png       ..\..\..\devel\share\CodeBlocks\images\DoxyBlocks\        > nul 2>&1
copy images\16x16\*.png ..\..\..\devel\share\CodeBlocks\images\DoxyBlocks\16x16\  > nul 2>&1
copy *.png              ..\..\..\devel\share\CodeBlocks\images\settings\          > nul 2>&1

copy images\*.png       ..\..\..\output\share\CodeBlocks\images\DoxyBlocks\       > nul 2>&1
copy images\16x16\*.png ..\..\..\output\share\CodeBlocks\images\DoxyBlocks\16x16\ > nul 2>&1
copy *.png              ..\..\..\output\share\CodeBlocks\images\settings\         > nul 2>&1

exit 0
