@echo off

set CB_DEVEL_DIR=devel%1
set CB_DEVEL_RESDIR=..\..\..\%CB_DEVEL_DIR%\share\CodeBlocks

call:mkdirSilent "%CB_DEVEL_RESDIR%\SpellChecker"
call:mkdirSilent "%CB_DEVEL_RESDIR%\images\settings"

zip -j9 %CB_DEVEL_RESDIR%\SpellChecker.zip manifest.xml SpellCheckSettingsPanel.xrc

xcopy /D /Y OnlineSpellChecking.xml  "%CB_DEVEL_RESDIR%\SpellChecker\"
xcopy /D /Y dictionaries\*.aff "%CB_DEVEL_RESDIR%\SpellChecker\"
xcopy /D /Y dictionaries\*.dic "%CB_DEVEL_RESDIR%\SpellChecker\"
xcopy /D /Y dictionaries\th_*.dat "%CB_DEVEL_RESDIR%\SpellChecker\"
xcopy /D /Y dictionaries\th_*.idx "%CB_DEVEL_RESDIR%\SpellChecker\"

xcopy /D /Y *.png "%CB_DEVEL_RESDIR%\images\settings\"
call:copyImageFiles rc "%CB_DEVEL_RESDIR%\SpellChecker"

set CB_OUTPUT_DIR=output%1
set CB_OUTPUT_RESDIR=..\..\..\%CB_OUTPUT_DIR%\share\CodeBlocks

call:mkdirSilent "%CB_OUTPUT_RESDIR%\SpellChecker"
call:mkdirSilent "%CB_OUTPUT_RESDIR%\images\settings"

xcopy /D /Y "%CB_DEVEL_RESDIR%\SpellChecker\*" "%CB_OUTPUT_RESDIR%\SpellChecker\"
xcopy /D /Y *.png "%CB_OUTPUT_RESDIR%\images\settings\"

::--------------------------------------------------------
::-- Function section starts below here
::--------------------------------------------------------
GOTO:EOF

:mkdirSilent - create a directory if it doesn't exists
echo Make dir %~1
if not exist "%~1" mkdir "%~1"
GOTO:EOF

:copyImageFiles - create a directory and copy image files to it
setlocal
echo Copy image files from %~1 to %~2
REM call mkdirSilent %~2
for %%g in (16x16,20x20,24x24,28x28,32x32,40x40,48x48,56x56,64x64) do (
    echo From %~1\%%g to %~2\%%g
    call:mkdirSilent %~2\%%g
    xcopy /D /Y %~1\%%g\*.png %~2\%%g > nul
)
REM     call:mkdirSilent "%CB_OUTPUT_RESDIR%\images\ThreadSearch\32x32"
REM     xcopy /D /y "%CB_DEVEL_RESDIR%\images\ThreadSearch\32x32\*.png" "%CB_OUTPUT_RESDIR%\images\ThreadSearch\32x32" > nul
endlocal
GOTO:EOF
