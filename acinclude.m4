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
         CFLAGS="-g -DDEBUG $CFLAGS"
         CXXFLAGS="-g -DDEBUG $CXXFLAGS"
	AC_MSG_RESULT(yes)
else
	CFLAGS="-O2 -ffast-math $CFLAGS"
	CXXFLAGS="-O2 -ffast-math $CXXFLAGS"
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

AC_MSG_CHECKING(whether to build the scripted wizard plugin)
prw_default="yes"
AC_ARG_ENABLE(project-wizard, [AC_HELP_STRING([--enable-scripted-wizard], [build the scripted wizard plugin (default YES)])],,
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

AC_MSG_CHECKING(whether to build the contrib plugins)
contrib_default="no"
AC_ARG_ENABLE(contrib, [AC_HELP_STRING([--enable-contrib], [build the contrib plugins (default NO)])],,
                       enable_contrib=$contrib_default)
AM_CONDITIONAL([BUILD_CONTRIB], [test "x$enable_contrib" = "xyes"])
if test "x$enable_contrib" = "xyes"; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

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
