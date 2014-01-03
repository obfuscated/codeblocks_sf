dnl setup codeblocks for current target
dnl copied and adapted from Ogre3D (www.ogre3d.org)

AC_DEFUN([CODEBLOCKS_GET_PLATFORM],
[CODEBLOCKS_PLATFORM=gtk
 AC_ARG_WITH(platform,
             AC_HELP_STRING([--with-platform=PLATFORM],
                            [the platform to build, win32, macosx or gtk(default)]),
             CODEBLOCKS_PLATFORM=$withval,
             CODEBLOCKS_PLATFORM=gtk)


  PLATFORM_CFLAGS=""
  PLATFORM_LIBS=""

  dnl Do the extra checks per type here
  case $CODEBLOCKS_PLATFORM in
    gtk)
      PLATFORM_CFLAGS="-I/usr/X11R6/include"
      PLATFORM_LIBS="-L/usr/X11R6/lib -lX11"
    ;;
    win32)
      PLATFORM_CFLAGS=""
      PLATFORM_LIBS="-lgdi32"
    ;;
    macosx)
      PLATFORM_CFLAGS=""
      PLATFORM_LIBS=""
    ;;
  esac

  AC_SUBST(PLATFORM_CFLAGS)
  AC_SUBST(PLATFORM_LIBS)
  AC_SUBST(CODEBLOCKS_PLATFORM)
])

AC_DEFUN([CODEBLOCKS_SETUP_FOR_TARGET],
[case $host in
*-*-cygwin* | *-*-mingw*)
	AC_SUBST(SHARED_FLAGS, "-shared -no-undefined -Xlinker --export-all-symbols")
	AC_SUBST(PLUGIN_FLAGS, "-shared -no-undefined -avoid-version")
	AC_CHECK_TOOL(RC, windres)
	nt=true
;;
*-*-darwin*) dnl including macosx
    AC_SUBST(SHARED_FLAGS, "-dynamic")
    AC_SUBST(PLUGIN_FLAGS, "-bundle -avoid-version")
    darwin=true
;;
 *) dnl default to standard linux
    AC_SUBST(SHARED_FLAGS, "-shared")
    AC_SUBST(PLUGIN_FLAGS, "-shared -avoid-version")
    linux=true
;;
esac
dnl you must arrange for every AM_conditional to run every time configure runs
AM_CONDITIONAL(CODEBLOCKS_NT, test x$nt = xtrue)
AM_CONDITIONAL(CODEBLOCKS_LINUX, test x$linux = xtrue)
AM_CONDITIONAL(CODEBLOCKS_DARWIN, test x$darwin = xtrue )
])

dnl check whether to enable debugging
AC_DEFUN([CODEBLOCKS_CHECK_DEBUG],
[
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug], [turn on debugging (default is OFF)])],,
                       enable_debug=$debug_default)
    if test "x$enable_debug" = "xyes"; then
        CFLAGS="-g -DDEBUG -DCB_AUTOCONF $CFLAGS"
        CXXFLAGS="-g -DDEBUG -DCB_AUTOCONF $CXXFLAGS"
        LDFLAGS="-Wl,--no-undefined"
        AC_MSG_RESULT(yes)
    else
        CFLAGS="-O2 -ffast-math -DCB_AUTOCONF $CFLAGS"
        CXXFLAGS="-O2 -ffast-math -DCB_AUTOCONF $CXXFLAGS"
        LDFLAGS="-Wl,--no-undefined"
        AC_MSG_RESULT(no)
    fi
])

AC_DEFUN([CB_GCC_VERSION], [
    GCC_FULL_VERSION=""
    GCC_MAJOR_VERSION=""
    GCC_MINOR_VERSION=""
    GCC_PATCH_VERSION=""
    if test "x$GCC" = "xyes" ; then
        AC_CACHE_CHECK([gcc version],[cb_cv_gcc_version],[
            cb_cv_gcc_version="`$CC -dumpversion`"
            if test "x$cb_cv_gcc_version" = "x"; then
                cb_cv_gcc_version=""
            fi
        ])
        GCC_FULL_VERSION=$cb_cv_gcc_version
        GCC_MAJOR_VERSION=$(echo $GCC_FULL_VERSION | cut -d'.' -f1)
        GCC_MINOR_VERSION=$(echo $GCC_FULL_VERSION | cut -d'.' -f2)
        GCC_PATCH_VERSION=$(echo $GCC_FULL_VERSION | cut -d'.' -f3)
    fi
    AC_SUBST([GCC_VERSION])
    AC_SUBST([GCC_MAJOR_VERSION])
    AC_SUBST([GCC_MINOR_VERSION])
    AC_SUBST([GCC_PATCH_VERSION])
])

dnl check what settings to enable
AC_DEFUN([CODEBLOCKS_ENABLE_SETTINGS],
[
AC_MSG_CHECKING(whether to build the source formatter plugin)
astyle_default="yes"
AC_ARG_ENABLE(source-formatter, [AC_HELP_STRING([--enable-source-formatter], [build the source formatter plugin (default YES)])],,
                       enable_astyle=$astyle_default)
AM_CONDITIONAL([BUILD_ASTYLE], [test "x$enable_astyle" = "xyes"])
if test "x$enable_astyle" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the autosave plugin)
autosave_default="yes"
AC_ARG_ENABLE(autosave, [AC_HELP_STRING([--enable-autosave], [build the autosave plugin (default YES)])],,
                       enable_autosave=$autosave_default)
AM_CONDITIONAL([BUILD_AUTOSAVE], [test "x$enable_autosave" = "xyes"])
if test "x$enable_autosave" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the class wizard plugin)
cw_default="yes"
AC_ARG_ENABLE(class-wizard, [AC_HELP_STRING([--enable-class-wizard], [build the class wizard plugin (default YES)])],,
                       enable_cw=$cw_default)
AM_CONDITIONAL([BUILD_CLASSWIZARD], [test "x$enable_cw" = "xyes"])
if test "x$enable_cw" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the code completion plugin)
cc_default="yes"
AC_ARG_ENABLE(code-completion, [AC_HELP_STRING([--enable-code-completion], [build the code completion plugin (default YES)])],,
                       enable_cc=$cc_default)
AM_CONDITIONAL([BUILD_CODECOMPLETION], [test "x$enable_cc" = "xyes"])
if test "x$enable_cc" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the compiler plugin)
gcc_default="yes"
AC_ARG_ENABLE(compiler, [AC_HELP_STRING([--enable-compiler], [build the compiler plugin (default YES)])],,
                       enable_gcc=$gcc_default)
AM_CONDITIONAL([BUILD_COMPILER], [test "x$enable_gcc" = "xyes"])
if test "x$enable_gcc" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the debugger plugin)
dbg_default="yes"
AC_ARG_ENABLE(debugger, [AC_HELP_STRING([--enable-debugger], [build the debugger plugin (default YES)])],,
                       enable_dbg=$dbg_default)
AM_CONDITIONAL([BUILD_DEBUGGER], [test "x$enable_dbg" = "xyes"])
if test "x$enable_dbg" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the default MIME handler plugin)
mime_default="yes"
AC_ARG_ENABLE(mime-handler, [AC_HELP_STRING([--enable-mime-handler], [build the default MIME handler plugin (default YES)])],,
                       enable_mime=$mime_default)
AM_CONDITIONAL([BUILD_MIMEHANDLER], [test "x$enable_mime" = "xyes"])
if test "x$enable_mime" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the open files list plugin)
openfiles_default="yes"
AC_ARG_ENABLE(open-files-list, [AC_HELP_STRING([--enable-open-files-list], [build the open files list plugin (default YES)])],,
                       enable_openfiles=$openfiles_default)
AM_CONDITIONAL([BUILD_OPENFILESLIST], [test "x$enable_openfiles" = "xyes"])
if test "x$enable_openfiles" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the occurrences highlighting plugin)
occurrenceshighlighting_default="yes"
AC_ARG_ENABLE(occurrences-highlighting, [AC_HELP_STRING([--enable-occurrences-highlighting], [build the occurrences highlighting plugin (default YES)])],,
                       enable_occurrenceshighlighting=$occurrenceshighlighting_default)
AM_CONDITIONAL([BUILD_OCCURRENCESHIGHLIGHTING], [test "x$enable_occurrenceshighlighting" = "xyes"])
if test "x$enable_occurrenceshighlighting" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the foreign projects importer plugin)
pimport_default="yes"
AC_ARG_ENABLE(projects-importer, [AC_HELP_STRING([--enable-projects-importer], [build the foreign projects importer plugin (default YES)])],,
                       enable_pimport=$pimport_default)
AM_CONDITIONAL([BUILD_PROJECTSIMPORTER], [test "x$enable_pimport" = "xyes"])
if test "x$enable_pimport" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the scripted wizard plugin)
prw_default="yes"
AC_ARG_ENABLE(scripted-wizard, [AC_HELP_STRING([--enable-scripted-wizard], [build the scripted wizard plugin (default YES)])],,
                       enable_prw=$prw_default)
AM_CONDITIONAL([BUILD_SCRIPTEDWIZARD], [test "x$enable_prw" = "xyes"])
if test "x$enable_prw" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the to-do plugin)
todo_default="yes"
AC_ARG_ENABLE(todo, [AC_HELP_STRING([--enable-todo], [build the to-do plugin (default YES)])],,
                       enable_todo=$todo_default)
AM_CONDITIONAL([BUILD_TODO], [test "x$enable_todo" = "xyes"])
if test "x$enable_todo" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to build the abbreviations plugin)
abbreviations_default="yes"
AC_ARG_ENABLE(abbreviations, [AC_HELP_STRING([--enable-abbreviations], [build the abbreviations plugin (default YES)])],,
                       enable_abbreviations=$abbreviations_default)
AM_CONDITIONAL([BUILD_ABBREVIATIONS], [test "x$enable_abbreviations" = "xyes"])
if test "x$enable_abbreviations" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to keep prebuild windows dll's in dist-tarball)
keep_dlls_default="yes"
AC_ARG_ENABLE(keep-dlls, [AC_HELP_STRING([--enable-keep-dlls], [keep prebuild windows dll's in dist-tarball (default YES)])],,
                       enable_keep_dlls=$keep_dlls_default)
AM_CONDITIONAL([KEEP_DLLS], [test "x$enable_keep_dlls" = "xyes"])
if test "x$enable_keep_dlls" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to integrate fortran-plugin in dist-tarball)
enable_fortran_default="yes"
AC_ARG_ENABLE(fortran, [AC_HELP_STRING([--enable-fortran], [integrate (external) fortran plugin in dist-tarball (default YES), NOTE: it will not be build automatically])],,
                       enable_fortran=$enable_fortran_default)
AM_CONDITIONAL([ENABLE_FORTRAN], [test "x$enable_fortran" = "xyes"])
if test "x$enable_fortran" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to use gtk-notebook as default notebook)
gtk_notebook_default="yes"
AC_ARG_ENABLE(gtk-notebook, [AC_HELP_STRING([--enable-gtk-notebook], [use gtk-notebook as default notebook (default YES)])],,
                       enable_gtk_notebook=$gtk_notebook_default)
AM_CONDITIONAL([GTK_NOTEBOOK], [test "x$enable_gtk_notebook" = "xyes"])
if test "x$enable_gtk_notebook" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi


case $host in
	*-*-cygwin* | *-*-mingw*)
		AC_MSG_CHECKING(whether to build the xp-manifest plugin)
		xpmanifest_default="yes"
		AC_ARG_ENABLE(xpmanifest, [AC_HELP_STRING([--enable-xpmanifest], [build the xp-manifest plugin (default YES)])],,
                       enable_xpmanifest=$xpmanifest_default)
		AM_CONDITIONAL([BUILD_MANIFEST], [test "x$enable_xpmanifest" = "xyes"])
		if test "x$enable_xpmanifest" = "xyes"; then
			AC_MSG_RESULT(yes)
		else
			AC_MSG_RESULT(no)
		fi
		;;
	*)
		AM_CONDITIONAL([BUILD_MANIFEST], [false])
		;;
esac

AC_DEFUN([BUILD_CONTRIB_NONE], [
	AM_CONDITIONAL([BUILD_AUTOVERSIONING], [false])
	AM_CONDITIONAL([BUILD_BROWSETRACKER], [false])
	AM_CONDITIONAL([BUILD_BYOGAMES], [false])
	AM_CONDITIONAL([BUILD_CBKODERS], [false])
	AM_CONDITIONAL([BUILD_CODESNIPPETS], [false])
	AM_CONDITIONAL([BUILD_CODESTAT], [false])
	AM_CONDITIONAL([BUILD_COPYSTRINGS], [false])
	AM_CONDITIONAL([BUILD_CSCOPE], [false])
	AM_CONDITIONAL([BUILD_DOXYBLOCKS], [false])
	AM_CONDITIONAL([BUILD_DRAGSCROLL], [false])
	AM_CONDITIONAL([BUILD_EDITORCONFIG], [false])
	AM_CONDITIONAL([BUILD_EDITORTWEAKS], [false])
	AM_CONDITIONAL([BUILD_ENVVARS], [false])
	AM_CONDITIONAL([BUILD_FILEMANAGER], [false])
	AM_CONDITIONAL([BUILD_HEADERFIXUP], [false])
	AM_CONDITIONAL([BUILD_HELP], [false])
	AM_CONDITIONAL([BUILD_KEYBINDER], [false])
	AM_CONDITIONAL([BUILD_LIBFINDER], [false])
	AM_CONDITIONAL([BUILD_NASSISHNEIDERMAN], [false])
	AM_CONDITIONAL([BUILD_PROFILER], [false])
	AM_CONDITIONAL([BUILD_PROJECTOPTIONSMANIPULATOR], [false])
	AM_CONDITIONAL([BUILD_REGEX], [false])
	AM_CONDITIONAL([BUILD_REOPENEDITOR], [false])
	AM_CONDITIONAL([BUILD_EXPORTER], [false])
	AM_CONDITIONAL([BUILD_SMARTINDENT], [false])
	AM_CONDITIONAL([BUILD_SPELLCHECKER], [false])
	AM_CONDITIONAL([BUILD_SYMTAB], [false])
	AM_CONDITIONAL([BUILD_THREADSEARCH], [false])
	AM_CONDITIONAL([BUILD_TOOLSPLUS], [false])
	AM_CONDITIONAL([BUILD_VALGRIND], [false])
	AM_CONDITIONAL([BUILD_WXCONTRIB], [false])
	AM_CONDITIONAL([BUILD_WXSMITH], [false])
	AM_CONDITIONAL([BUILD_WXSMITHCONTRIB], [false])
	AM_CONDITIONAL([BUILD_WXSMITHAUI], [false])
	AM_CONDITIONAL([BUILD_HEXEDITOR], [false])
	AM_CONDITIONAL([BUILD_INCSEARCH], [false])
	AM_CONDITIONAL([BUILD_MOUSESAP], [false])
	AM_CONDITIONAL([BUILD_CCCC], [false])
	AM_CONDITIONAL([BUILD_CPPCHECK], [false])
])

AC_DEFUN([BUILD_CONTRIB_ALL], [
	AM_CONDITIONAL([BUILD_AUTOVERSIONING], [true])
	AM_CONDITIONAL([BUILD_BROWSETRACKER], [true])
	AM_CONDITIONAL([BUILD_BYOGAMES], [true])
	AM_CONDITIONAL([BUILD_CBKODERS], [true])
	AM_CONDITIONAL([BUILD_CODESNIPPETS], [true])
	AM_CONDITIONAL([BUILD_CODESTAT], [true])
	AM_CONDITIONAL([BUILD_COPYSTRINGS], [true])
	AM_CONDITIONAL([BUILD_CSCOPE], [true])
	AM_CONDITIONAL([BUILD_DOXYBLOCKS], [true])
	AM_CONDITIONAL([BUILD_DRAGSCROLL], [true])
	AM_CONDITIONAL([BUILD_EDITORCONFIG], [true])
	AM_CONDITIONAL([BUILD_EDITORTWEAKS], [true])
	AM_CONDITIONAL([BUILD_ENVVARS], [true])
	AM_CONDITIONAL([BUILD_FILEMANAGER], [true])
	AM_CONDITIONAL([BUILD_HEADERFIXUP], [true])
	AM_CONDITIONAL([BUILD_HELP], [true])
	AM_CONDITIONAL([BUILD_KEYBINDER], [true])
	AM_CONDITIONAL([BUILD_LIBFINDER], [true])
	AM_CONDITIONAL([BUILD_NASSISHNEIDERMAN], [true])
	AM_CONDITIONAL([BUILD_PROJECTOPTIONSMANIPULATOR], [true])
	AM_CONDITIONAL([BUILD_PROFILER], [true])
	AM_CONDITIONAL([BUILD_REGEX], [true])
	AM_CONDITIONAL([BUILD_REOPENEDITOR], [true])
	AM_CONDITIONAL([BUILD_EXPORTER], [true])
	AM_CONDITIONAL([BUILD_SMARTINDENT], [true])
	AM_CONDITIONAL([BUILD_SPELLCHECKER], [true])
	AM_CONDITIONAL([BUILD_SYMTAB], [true])
	AM_CONDITIONAL([BUILD_THREADSEARCH], [true])
	AM_CONDITIONAL([BUILD_TOOLSPLUS], [true])
	AM_CONDITIONAL([BUILD_VALGRIND], [true])
	AM_CONDITIONAL([BUILD_WXCONTRIB], [true])
	AM_CONDITIONAL([BUILD_WXSMITH], [true])
	AM_CONDITIONAL([BUILD_WXSMITHCONTRIB], [true])
	AM_CONDITIONAL([BUILD_WXSMITHAUI], [true])
	AM_CONDITIONAL([BUILD_HEXEDITOR], [true])
	AM_CONDITIONAL([BUILD_INCSEARCH], [true])
	AM_CONDITIONAL([BUILD_MOUSESAP], [true])
	AM_CONDITIONAL([BUILD_CCCC], [true])
	AM_CONDITIONAL([BUILD_CPPCHECK], [true])
])

# default to 'none'
BUILD_CONTRIB_NONE

AC_MSG_CHECKING(which (if any) contrib plugins to build)
AC_ARG_WITH(contrib-plugins,
  [  --with-contrib-plugins=<list>     compile contrib plugins in <list>. ]
  [                        plugins may be separated with commas. ]
  [                        "all", "yes" or just "--with-contrib-plugins" compiles all contrib plugins ]
  [                        "all,-help" or "yes,-help" compiles all contrib plugins except the help plugin ]
  [                        "none", "no", "--without-contrib-plugins" or skipping the parameter at all, ]
  [                        compiles none of the contrib-plugins ]
  [                        Plugin names are: AutoVersioning, BrowseTracker, byogames, Cccc, CppCheck, cbkoders, codesnippets, ]
  [                        		     codestat, copystrings, Cscope, DoxyBlocks, dragscroll, EditorConfig, EditorTweaks, envvars, ]
  [                        		     FileManager, headerfixup, help, hexeditor, incsearch, keybinder, libfinder, MouseSap, ]
  [                        		     NassiShneiderman, ProjectOptionsManipulator, profiler, regex, ReopenEditor, exporter, smartindent, spellchecker, ]
  [                        		     symtab, ThreadSearch, ToolsPlus, Valgrind, wxcontrib, wxsmith, wxsmithcontrib, wxsmithaui ],
  plugins="$withval", plugins="none")

plugins=`echo $plugins | sed 's/,/ /g'`
for plugin in $plugins
do
    case "$plugin" in
	all|yes)
		BUILD_CONTRIB_ALL
		;;
	AutoVersioning)
		AM_CONDITIONAL([BUILD_AUTOVERSIONING], [true])
		;;
	BrowseTracker)
		AM_CONDITIONAL([BUILD_BROWSETRACKER], [true])
		;;
	byogames)
		AM_CONDITIONAL([BUILD_BYOGAMES], [true])
		;;
	cbkoders)
		AM_CONDITIONAL([BUILD_CBKODERS], [true])
		;;
	codesnippets)
		AM_CONDITIONAL([BUILD_CODESNIPPETS], [true])
		;;
	codestat)
		AM_CONDITIONAL([BUILD_CODESTAT], [true])
		;;
	copystrings)
		AM_CONDITIONAL([BUILD_COPYSTRINGS], [true])
		;;
	Cscope)
		AM_CONDITIONAL([BUILD_CSCOPE], [true])
		;;
	DoxyBlocks)
		AM_CONDITIONAL([BUILD_DOXYBLOCKS], [true])
		;;
	dragscroll)
		AM_CONDITIONAL([BUILD_DRAGSCROLL], [true])
		;;
	EditorConfig)
		AM_CONDITIONAL([BUILD_EDITORCONFIG], [true])
		;;
	EditorTweaks)
		AM_CONDITIONAL([BUILD_EDITORTWEAKS], [true])
		;;
	envvars)
		AM_CONDITIONAL([BUILD_ENVVARS], [true])
		;;
	FileManager)
		AM_CONDITIONAL([BUILD_FILEMANAGER], [true])
		;;
	headerfixup)
		AM_CONDITIONAL([BUILD_HEADERFIXUP], [true])
		;;
	help)
		AM_CONDITIONAL([BUILD_HELP], [true])
		;;
	keybinder)
		AM_CONDITIONAL([BUILD_KEYBINDER], [true])
		;;
	libfinder)
		AM_CONDITIONAL([BUILD_LIBFINDER], [true])
		;;
	NassiShneiderman)
		AM_CONDITIONAL([BUILD_NASSISHNEIDERMAN], [true])
		;;
	ProjectOptionsManipulator)
		AM_CONDITIONAL([BUILD_PROJECTOPTIONSMANIPULATOR], [true])
		;;
	profiler)
		AM_CONDITIONAL([BUILD_PROFILER], [true])
		;;
	regex)
		AM_CONDITIONAL([BUILD_REGEX], [true])
		;;
	ReopenEditor)
		AM_CONDITIONAL([BUILD_REOPENEDITOR], [true])
		;;
	exporter)
		AM_CONDITIONAL([BUILD_EXPORTER], [true])
		;;
	smartindent)
		AM_CONDITIONAL([BUILD_SMARTINDENT], [true])
		;;
	spellchecker)
		AM_CONDITIONAL([BUILD_SPELLCHECKER], [true])
		;;
	symtab)
		AM_CONDITIONAL([BUILD_SYMTAB], [true])
		;;
	ThreadSearch)
		AM_CONDITIONAL([BUILD_THREADSEARCH], [true])
		;;
	ToolsPlus)
		AM_CONDITIONAL([BUILD_TOOLSPLUS], [true])
		;;
	Valgrind)
		AM_CONDITIONAL([BUILD_VALGRIND], [true])
		;;
	wxcontrib)
		AM_CONDITIONAL([BUILD_WXCONTRIB], [true])
		;;
	wxsmith)
		AM_CONDITIONAL([BUILD_WXSMITH], [true])
		;;
	wxsmithcontrib)
		AM_CONDITIONAL([BUILD_WXSMITHCONTRIB], [true])
		;;
	wxsmithaui)
		AM_CONDITIONAL([BUILD_WXSMITHAUI], [true])
		;;
	hexeditor)
		AM_CONDITIONAL([BUILD_HEXEDITOR], [true])
		;;
	incsearch)
		AM_CONDITIONAL([BUILD_INCSEARCH], [true])
		;;
	MouseSap)
		AM_CONDITIONAL([BUILD_MOUSESAP], [true])
		;;
	Cccc)
		AM_CONDITIONAL([BUILD_CCCC], [true])
		;;
	CppCheck)
		AM_CONDITIONAL([BUILD_CPPCHECK], [true])
		;;
	-AutoVersioning)
		AM_CONDITIONAL([BUILD_AUTOVERSIONING], [false])
		;;
	-BrowseTracker)
		AM_CONDITIONAL([BUILD_BROWSETRACKER], [false])
		;;
	-byogames)
		AM_CONDITIONAL([BUILD_BYOGAMES], [false])
		;;
	-cbkoders)
		AM_CONDITIONAL([BUILD_CBKODERS], [false])
		;;
	-codesnippets)
		AM_CONDITIONAL([BUILD_CODESNIPPETS], [false])
		;;
	-codestat)
		AM_CONDITIONAL([BUILD_CODESTAT], [false])
		;;
	-copystrings)
		AM_CONDITIONAL([BUILD_COPYSTRINGS], [false])
		;;
	-Cscope)
		AM_CONDITIONAL([BUILD_CSCOPE], [false])
		;;
	-DoxyBlocks)
		AM_CONDITIONAL([BUILD_DOXYBLOCKS], [false])
		;;
	-dragscroll)
		AM_CONDITIONAL([BUILD_DRAGSCROLL], [false])
		;;
	-EditorConfig)
		AM_CONDITIONAL([BUILD_EDITORCONFIG], [false])
		;;
	-EditorTweaks)
		AM_CONDITIONAL([BUILD_EDITORTWEAKS], [false])
		;;
	-envvars)
		AM_CONDITIONAL([BUILD_ENVVARS], [false])
		;;
	-FileManager)
		AM_CONDITIONAL([BUILD_FILEMANAGER], [false])
		;;
	-headerfixup)
		AM_CONDITIONAL([BUILD_HEADERFIXUP], [false])
		;;
	-help)
		AM_CONDITIONAL([BUILD_HELP], [false])
		;;
	-keybinder)
		AM_CONDITIONAL([BUILD_KEYBINDER], [false])
		;;
	-libfinder)
		AM_CONDITIONAL([BUILD_LIBFINDER], [false])
		;;
	-NassiShneiderman)
		AM_CONDITIONAL([BUILD_NASSISHNEIDERMAN], [false])
		;;
	-profiler)
		AM_CONDITIONAL([BUILD_PROFILER], [false])
		;;
	-regex)
		AM_CONDITIONAL([BUILD_REGEX], [false])
		;;
	-ReopenEditor)
		AM_CONDITIONAL([BUILD_REOPENEDITOR], [false])
		;;
	-exporter)
		AM_CONDITIONAL([BUILD_EXPORTER], [false])
		;;
	-smartindent)
		AM_CONDITIONAL([BUILD_SMARTINDENT], [false])
		;;
	-spellchecker)
		AM_CONDITIONAL([BUILD_SPELLCHECKER], [false])
		;;
	-symtab)
		AM_CONDITIONAL([BUILD_SYMTAB], [false])
		;;
	-ThreadSearch)
		AM_CONDITIONAL([BUILD_THREADSEARCH], [false])
		;;
	-ToolsPlus)
		AM_CONDITIONAL([BUILD_TOOLSPLUS], [false])
		;;
	-Valgrind)
		AM_CONDITIONAL([BUILD_VALGRIND], [false])
		;;
	-wxcontrib)
		AM_CONDITIONAL([BUILD_WXCONTRIB], [false])
		;;
	-wxsmith)
		AM_CONDITIONAL([BUILD_WXSMITH], [false])
		;;
	-wxsmithcontrib)
		AM_CONDITIONAL([BUILD_WXSMITHCONTRIB], [false])
		;;
	-wxsmithaui)
		AM_CONDITIONAL([BUILD_WXSMITHAUI], [false])
		;;
	-hexeditor)
		AM_CONDITIONAL([BUILD_HEXEDITOR], [false])
		;;
	-incsearch)
		AM_CONDITIONAL([BUILD_INCSEARCH], [false])
		;;
	-MouseSap)
		AM_CONDITIONAL([BUILD_MOUSESAP], [false])
		;;
	-Cccc)
		AM_CONDITIONAL([BUILD_CCCC], [false])
		;;
	-CppCheck)
		AM_CONDITIONAL([BUILD_CPPCHECK], [false])
		;;
	none|no)
		;;
	*)
		echo
		echo "Error: Unknown contrib plugin $plugin." >&2
		echo "       Note: the names are case-sensitive!" >&2
		echo "       Try $[0] --help for exact spelling." >&2
		exit 1
		;;
    esac
done
AC_MSG_RESULT($plugins)


AC_SUBST(BUILD_AUTOVERSIONING)
AC_SUBST(BUILD_BROWSETRACKER)
AC_SUBST(BUILD_BYOGAMES)
AC_SUBST(BUILD_CBKODERS)
AC_SUBST(BUILD_CCCC)
AC_SUBST(BUILD_CODESNIPPETS)
AC_SUBST(BUILD_CODESTAT)
AC_SUBST(BUILD_COPYSTRINGS)
AC_SUBST(BUILD_CPPCHECK)
AC_SUBST(BUILD_CSCOPE)
AC_SUBST(BUILD_DOXYBLOCKS)
AC_SUBST(BUILD_DRAGSCROLL)
AC_SUBST(BUILD_EDITORCONFIG)
AC_SUBST(BUILD_EDITORTWEAKS)
AC_SUBST(BUILD_ENVVARS)
AC_SUBST(BUILD_FILEMANAGER)
AC_SUBST(BUILD_HEADERFIXUP)
AC_SUBST(BUILD_HELP)
AC_SUBST(BUILD_HEXEDITOR)
AC_SUBST(BUILD_INCSEARCH)
AC_SUBST(BUILD_KEYBINDER)
AC_SUBST(BUILD_LIBFINDER)
AC_SUBST(BUILD_MOUSESAP)
AC_SUBST(BUILD_NASSISHNEIDERMAN)
AC_SUBST(BUILD_PROFILER)
AC_SUBST(BUILD_REGEX)
AC_SUBST(BUILD_REOPENEDITOR)
AC_SUBST(BUILD_EXPORTER)
AC_SUBST(BUILD_SYMTAB)
AC_SUBST(BUILD_SMARTINDENT)
AC_SUBST(BUILD_SPELLCHECKER)
AC_SUBST(BUILD_THREADSEARCH)
AC_SUBST(BUILD_TOOLSPLUS)
AC_SUBST(BUILD_VALGRIND)
AC_SUBST(BUILD_WXCONTRIB)
AC_SUBST(BUILD_WXSMITH)
AC_SUBST(BUILD_WXSMITHCONTRIB)
AC_SUBST(BUILD_WXSMITHAUI)

GCC_PCH=0
PCH_FLAGS=
pch_default="yes"
AM_CONDITIONAL([PRECOMPILE_HEADERS], [false])
AC_ARG_ENABLE(pch, [AC_HELP_STRING([--enable-pch], [use precompiled headers if available (default YES)])],,
                       enable_pch=$pch_default)
if test "x$enable_pch" = "x" -o "x$enable_pch" = "xyes" ; then
    if test "x$GCC" = "xyes"; then
        dnl test if we have gcc-3.4:
        AC_MSG_CHECKING([if the compiler supports precompiled headers])
        AC_TRY_COMPILE([],
            [
                #if !defined(__GNUC__) || !defined(__GNUC_MINOR__)
                    #error "no pch support"
                #endif
                #if (__GNUC__ < 3)
                    #error "no pch support"
                #endif
                #if (__GNUC__ == 3) && \
                   ((!defined(__APPLE_CC__) && (__GNUC_MINOR__ < 4)) || \
                   ( defined(__APPLE_CC__) && (__GNUC_MINOR__ < 3)))
                    #error "no pch support"
                #endif
            ],
            [
                AC_MSG_RESULT([yes])
                GCC_PCH=1
                PCH_FLAGS="-DCB_PRECOMP -Winvalid-pch"
            ],
            [
                AC_MSG_RESULT([no])
            ])
        AM_CONDITIONAL([PRECOMPILE_HEADERS], [test $GCC_PCH = 1])
    fi
fi

])

# _AM_PROG_TAR(FORMAT)
# --------------------
# Check how to create a tarball in format FORMAT.
# FORMAT should be one of `v7', `ustar', or `pax'.
#
# Substitute a variable $(am__tar) that is a command
# writing to stdout a FORMAT-tarball containing the directory
# $tardir.
#     tardir=directory && $(am__tar) > result.tar
#
# Substitute a variable $(am__untar) that extract such
# a tarball read from stdin.
#     $(am__untar) < result.tar
AC_DEFUN([_AM_PROG_TAR],
[# Always define AMTAR for backward compatibility.
AM_MISSING_PROG([AMTAR], [tar])
m4_if([$1], [v7],
     [am__tar='${AMTAR} chf - "$$tardir"'; am__untar='${AMTAR} xf -'],
     [m4_case([$1], [ustar],, [pax],,
              [m4_fatal([Unknown tar format])])
AC_MSG_CHECKING([how to create a $1 tar archive])
# Loop over all known methods to create a tar archive until one works.
_am_tools='gnutar m4_if([$1], [ustar], [plaintar]) pax cpio none'
_am_tools=${am_cv_prog_tar_$1-$_am_tools}
# Do not fold the above two line into one, because Tru64 sh and
# Solaris sh will not grok spaces in the rhs of `-'.
for _am_tool in $_am_tools
do
  case $_am_tool in
  gnutar)
    for _am_tar in tar gnutar gtar;
    do
      AM_RUN_LOG([$_am_tar --version]) && break
    done
    am__tar="$_am_tar --format=m4_if([$1], [pax], [posix], [$1]) -chf - "'"$$tardir"'
    am__tar_="$_am_tar --format=m4_if([$1], [pax], [posix], [$1]) -chf - "'"$tardir"'
    am__untar="$_am_tar -xf -"
    ;;
  plaintar)
    # Must skip GNU tar: if it does not support --format= it doesn't create
    # ustar tarball either.
    (tar --version) >/dev/null 2>&1 && continue
    am__tar='tar chf - "$$tardir"'
    am__tar_='tar chf - "$tardir"'
    am__untar='tar xf -'
    ;;
  pax)
    am__tar='pax -L -x $1 -w "$$tardir"'
    am__tar_='pax -L -x $1 -w "$tardir"'
    am__untar='pax -r'
    ;;
  cpio)
    am__tar='find "$$tardir" -print | cpio -o -H $1 -L'
    am__tar_='find "$tardir" -print | cpio -o -H $1 -L'
    am__untar='cpio -i -H $1 -d'
    ;;
  none)
    am__tar=false
    am__tar_=false
    am__untar=false
    ;;
  esac

  # If the value was cached, stop now.  We just wanted to have am__tar
  # and am__untar set.
  test -n "${am_cv_prog_tar_$1}" && break

  # tar/untar a dummy directory, and stop if the command works
  rm -rf conftest.dir
  mkdir conftest.dir
  echo GrepMe > conftest.dir/file
  AM_RUN_LOG([tardir=conftest.dir && eval $am__tar_ >conftest.tar])
  rm -rf conftest.dir
  if test -s conftest.tar; then
    AM_RUN_LOG([$am__untar <conftest.tar])
    grep GrepMe conftest.dir/file >/dev/null 2>&1 && break
  fi
done
rm -rf conftest.dir

AC_CACHE_VAL([am_cv_prog_tar_$1], [am_cv_prog_tar_$1=$_am_tool])
AC_MSG_RESULT([$am_cv_prog_tar_$1])])
AC_SUBST([am__tar])
AC_SUBST([am__untar])
]) # _AM_PROG_TAR
