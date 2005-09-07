dnl setup codeblocks for current target
dnl copied and adapted from Ogre3D (www.ogre3d.org)

AC_DEFUN([CODEBLOCKS_GET_PLATFORM],
[CODEBLOCKS_PLATFORM=gtk
 AC_ARG_WITH(platform, 
             AC_HELP_STRING([--with-platform=PLATFORM],
                            [the platform to build, win32 or gtk(default)]),
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
*-*-darwin*)
	dnl although OSX isn't supported yet, it doesn't hurt
	dnl to support it in this check
    AC_SUBST(SHARED_FLAGS, "-shared")
    AC_SUBST(PLUGIN_FLAGS, "-shared -avoid-version")
    osx=true
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
AM_CONDITIONAL(CODEBLOCKS_OSX, test x$osx = xtrue )
])

dnl check what settings to enable
AC_DEFUN([CODEBLOCKS_ENABLE_SETTINGS],
[
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug], [turn on debugging (default is OFF)])],,
                       enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
         CFLAGS="$CFLAGS -g -DDEBUG"
         CXXFLAGS="$CXXFLAGS -g -DDEBUG"
	AC_MSG_RESULT(yes)
else
	CFLAGS="$CFLAGS -O2 -ffast-math"
	CXXFLAGS="$CXXFLAGS -O2 -ffast-math"
	AC_MSG_RESULT(no)
fi

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

AC_MSG_CHECKING(whether to build the plugin wizard plugin)
pw_default="yes"
AC_ARG_ENABLE(plugin-wizard, [AC_HELP_STRING([--enable-plugin-wizard], [build the plugin wizard plugin (default YES)])],,
                       enable_pw=$pw_default)
AM_CONDITIONAL([BUILD_PLUGINWIZARD], [test "x$enable_pw" = "xyes"])
if test "x$enable_pw" = "xyes"; then
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


case $host in
	*-*-cygwin* | *-*-mingw*)
		AC_MSG_CHECKING(whether to build the xp-manifest plugin)
		xpmanifest_default="yes"
		AC_ARG_ENABLE(todo, [AC_HELP_STRING([--enable-xpmanifest], [build the xp-manifest plugin (default YES)])],,
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

])
