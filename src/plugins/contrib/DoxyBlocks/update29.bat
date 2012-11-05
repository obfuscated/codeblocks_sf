@echo off
md   ..\..\..\devel29                                           > nul 2>&1
md   ..\..\..\output29                                          > nul 2>&1
md   ..\..\..\devel29\share                                     > nul 2>&1
md   ..\..\..\output29\share                                    > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks                          > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks                         > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks\images                   > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks\images                  > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks\images\settings          > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks\images\settings         > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks\images\DoxyBlocks        > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks\images\DoxyBlocks       > nul 2>&1
md   ..\..\..\devel29\share\CodeBlocks\images\DoxyBlocks\16x16  > nul 2>&1
md   ..\..\..\output29\share\CodeBlocks\images\DoxyBlocks\16x16 > nul 2>&1

copy images\*.png       ..\..\..\devel29\share\CodeBlocks\images\DoxyBlocks\        > nul 2>&1
copy images\16x16\*.png ..\..\..\devel29\share\CodeBlocks\images\DoxyBlocks\16x16\  > nul 2>&1
copy *.png              ..\..\..\devel29\share\CodeBlocks\images\settings\          > nul 2>&1

copy images\*.png       ..\..\..\output29\share\CodeBlocks\images\DoxyBlocks\       > nul 2>&1
copy images\16x16\*.png ..\..\..\output29\share\CodeBlocks\images\DoxyBlocks\16x16\ > nul 2>&1
copy *.png              ..\..\..\output29\share\CodeBlocks\images\settings\         > nul 2>&1

exit 0
