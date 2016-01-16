@echo off
rem Adopt path to required tool here:
set PATH=C:\Programme\Tools\GnuWin32\bin;%PATH%

set grep_found=0
set xargs_found=0
set xgettext_found=0
set msginit_found=0

for %%G in ("%path:;=" "%") do (
  if exist %%G\grep.exe     set grep_found=1
  if exist %%G\xargs.exe    set xargs_found=1
  if exist %%G\xgettext.exe set xgettext_found=1
  if exist %%G\msginit.exe  set msginit_found=1
)

if "%grep_found%"=="0"     goto Error
if "%xargs_found%"=="0"    goto Error
if "%xgettext_found%"=="0" goto Error
if "%msginit_found%"=="0"  goto Error

echo "creating core .pot files"
find ../sdk ../src                | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o codeblocks.pot
find ../plugins | grep -v contrib | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o coreplugins.pot

echo "creating contrib .pot files"
find ../plugins/contrib/AutoVersioning            | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o autoversioning.pot
find ../plugins/contrib/BrowseTracker             | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o browsetracker.pot
find ../plugins/contrib/byogames                  | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o byogames.pot
find ../plugins/contrib/cb_koders                 | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o cb_koders.pot
find ../plugins/contrib/codesnippets              | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o codesnippets.pot
find ../plugins/contrib/codestat                  | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o codestat.pot
find ../plugins/contrib/copystrings               | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o copystrings.pot
find ../plugins/contrib/devpak_plugin             | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o devpak_plugin.pot
find ../plugins/contrib/dragscroll                | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o dragscroll.pot
find ../plugins/contrib/envvars                   | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o envvars.pot
find ../plugins/contrib/headerfixup               | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o headerfixup.pot
find ../plugins/contrib/help_plugin               | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o help_plugin.pot
find ../plugins/contrib/HexEditor                 | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o hexeditor.pot
find ../plugins/contrib/IncrementalSearch         | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o incrementalsearch.pot
find ../plugins/contrib/keybinder                 | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o keybinder.pot
find ../plugins/contrib/lib_finder                | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o lib_finder.pot
find ../plugins/contrib/MouseSap                  | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o mousesap.pot
find ../plugins/contrib/profiler                  | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o profiler.pot
find ../plugins/contrib/ProjectOptionsManipulator | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o ProjectOptionsManipulator.pot
find ../plugins/contrib/regex_testbed             | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o regex_testbed.pot
find ../plugins/contrib/rndgen                    | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o rndgen.pot
find ../plugins/contrib/source_exporter           | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o source_exporter.pot
find ../plugins/contrib/symtab                    | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o symtab.pot
find ../plugins/contrib/ThreadSearch              | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o threadsearch.pot
find ../plugins/contrib/Valgrind                  | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o valgrind.pot
find ../plugins/contrib/wxSmith                   | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o wxsmith.pot
find ../plugins/contrib/wxSmithAui                | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o wxsmithaui.pot
find ../plugins/contrib/wxSmithContribItems       | grep -P "(\.cpp|\.h)$" | xargs xgettext --keyword=_ -o wxsmithcontribitems.pot

echo "running msginit"

for %%i in (de fr it es cs da nl ga hu pl ru sk jp tr) do (
  if not exist %%i cmd /C mkdir %%i
  for %%j in (*.pot) do (
    msginit -i %%j -o %%i/%%j -l %%i
  )
)

goto TheEnd

:Error
echo Error: At least one required tool was not found.
echo Make sure you have the following tools in the PATH:
echo - grep, xargs, xgettext, msginit
echo They are all part of the GnuWin32 tools (package gettext).
echo Make sure you have adopted the path in this batch file.

:TheEnd
