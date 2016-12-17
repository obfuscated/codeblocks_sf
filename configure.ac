AC_PREREQ(2.50)

m4_include([revision.m4])

AC_INIT([codeblocks], SVN_REVISION)

AC_CANONICAL_TARGET

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign dist-bzip2 no-dist-gzip subdir-objects])

AC_DISABLE_STATIC

LT_INIT

AC_CONFIG_HEADER([src/include/config.h])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

AM_CONDITIONAL([LINUX], [test `uname` = "Linux"])
AM_CONDITIONAL([DARWIN], [test `uname` = "Darwin"])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_AWK

CODEBLOCKS_CHECK_LINK_FLAG([-Wl,--no-undefined],[LDFLAGS="-Wl,--no-undefined $LDFLAGS"])

CODEBLOCKS_CHECK_DEBUG

CODEBLOCKS_SETUP_FOR_TARGET

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/param.h unistd.h malloc.h sys/malloc.h malloc/malloc.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

dnl Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit getcwd isascii memchr memmove memset strcasecmp strchr strcspn strdup strrchr strstr])
AC_CHECK_LIB(dl, dlopen)
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_FUNC(snprintf, AC_DEFINE(HAVE_SNPRINTF,,snprintf))
AC_CHECK_FUNC(vsnprintf, AC_DEFINE(HAVE_VSNPRINTF,,vsnprintf))

PKG_CHECK_MODULES([zlib], [zlib], [HAVE_ZLIB=yes], [HAVE_ZLIB=no])
save_LIBS=$LIBS
AC_SEARCH_LIBS(BZ2_bzopen, bz2, [HAVE_BZIP2=yes], [HAVE_BZIP2=no])
LIBS=$save_LIBS

save_CC=$CC
save_CPP=$CPP
CC=$CXX
CPP=$CXXCPP
AC_CHECK_HEADER(astyle.h, [HAVE_ASTYLE=yes], [HAVE_ASTYLE=no])
CC=$save_CC
CPP=$save_CPP

AM_CONDITIONAL([HAVE_ZLIB], [test "$HAVE_ZLIB" = yes])
AM_CONDITIONAL([HAVE_BZIP2], [test "$HAVE_BZIP2" = yes])
AM_CONDITIONAL([HAVE_ASTYLE], [test "$HAVE_ASTYLE" = yes])

AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, yes, no)

ifdef([PKG_CHECK_MODULES],[],[
	define([PKG_CHECK_MODULES],
	[ echo "You don't have pkg.m4 properly installed" >&2
		exit 1
	])
])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([SQUIRREL], [squirrel], [HAVE_SQUIRREL=yes], [HAVE_SQUIRREL=no])
AM_CONDITIONAL([HAVE_SQUIRREL], [test "$HAVE_SQUIRREL" = yes])

PKG_CHECK_MODULES([TINYXML], [tinyxml], [HAVE_TINYXML=yes], [HAVE_TINYXML=no])
AM_CONDITIONAL([HAVE_TINYXML], [test "$HAVE_TINYXML" = yes])

dnl Darwin -> .dylib  Linux -> .so
if test "x$DARWIN_TRUE" = "x" ; then
    MODULE_SHARED_LDFLAGS="-dynamiclib"
else
    MODULE_SHARED_LDFLAGS="-module -shared"
fi
AC_SUBST(MODULE_SHARED_LDFLAGS)

dnl versioning info for libtool
dnl Note this is the ABI version which is not the same as our actual library version
CODEBLOCKS_CURRENT=0
CODEBLOCKS_REVISION=1
CODEBLOCKS_AGE=0
CODEBLOCKS_VERSION_INFO=$CODEBLOCKS_CURRENT:$CODEBLOCKS_REVISION:$CODEBLOCKS_AGE
AC_SUBST(CODEBLOCKS_VERSION_INFO)

AM_MISSING_PROG(AMTAR, tar)
_AM_IF_OPTION([tar-ustar], [_AM_PROG_TAR([ustar])],
              [_AM_IF_OPTION([tar-pax], [_AM_PROG_TAR([pax])],
                             [_AM_PROG_TAR([v7])])])

CODEBLOCKS_GET_PLATFORM
CODEBLOCKS_ENABLE_SETTINGS

AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.8.12, wxWin=1)
    if test "$wxWin" != 1; then
        AC_MSG_ERROR([
                wxWidgets must be installed on your system.

                Please check that wx-config is in path, the directory
                where wxWidgets libraries are installed (returned by
                'wx-config --libs' or 'wx-config --static --libs' command)
                is in LD_LIBRARY_PATH or equivalent variable and
                wxWindows version is 2.8.0 or above.
                ])
	else
		# check for wx2.8.3 which is known to behave badly for us
		if test "$WX_VERSION" = "2.8.3"; then
			AC_MSG_ERROR([
		wxWidgets version 2.8.3 is known to behave badly with
		Code::Blocks. Please upgrade to a newer (or downgrade to an
		earlier) wxWidgets version...
			])
		fi
    fi

dnl		CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
dnl		CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
dnl		CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
dnl		LIBS="$LIBS $WX_LIBS"

AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(LIBS)

WX_GTK_CFLAGS=
WX_GTK_LIBS=
WX_X11_CFLAGS=
WX_X11_LIBS=

dnl default value is to (silently) do nothing in the makefile
POSTLINK_COMMAND="@true"
MACSETFILE="@true"

AC_MSG_CHECKING(for wxWidgets platform)
WX_BASENAME=`$WX_CONFIG_WITH_ARGS --basename`
case $WX_BASENAME in
    *wx_gtk*)
        AC_MSG_RESULT(wxGTK)
		dnl
		dnl pkg-config no longer needed by debugger plugin
		dnl but is needed by others
		dnl
		dnl only do this if it is really needed
		dnl AM_CONDITIONAL adds _TRUE and _FALSE to the appropriate variables
		dnl if the condition is true, the *_TRUE variable is empty (!)
		if test "x$BUILD_FILEMANAGER_TRUE" = "x" -o \
				"x$BUILD_SPELLCHECKER_TRUE" = "x" -o \
				"x$BUILD_MOUSESAP_TRUE" = "x" -o \
				"x$BUILD_EXPORTER_TRUE" = "x" -o \
				"x$GTK_NOTEBOOK_TRUE" = "x" -o \
				"x$LINUX_TRUE" = "x" ; then

			if test "x$LINUX_TRUE" = "x" ; then
				if test "x$HAVE_PKG_CONFIG" = "xyes"; then
					PKG_CHECK_MODULES([GLIB2], [glib-2.0])
				fi
			fi

			if test "x$BUILD_SPELLCHECKER_TRUE" = "x" ; then
				if test "x$HAVE_PKG_CONFIG" = "xyes"; then
					PKG_CHECK_MODULES([HUNSPELL], [hunspell])
				fi
			fi

			if test "x$BUILD_MOUSESAP_TRUE" = "x"  -o \
					"x$GTK_NOTEBOOK_TRUE" = "x" ; then
				if test "x$HAVE_PKG_CONFIG" = "xyes"; then
					case $WX_BASENAME in
						*wx_gtk3*)
							AC_MSG_NOTICE([gtk3 used])
							PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0.0])
						;;
						*wx_gtk2*)
							AC_MSG_NOTICE([gtk2 used])
							PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.0.0])
						;;
					esac
				fi
			fi

			if test "x$BUILD_FILEMANAGER_TRUE" = "x" ; then
				if test "x$HAVE_PKG_CONFIG" = "xyes"; then
					PKG_CHECK_MODULES([GAMIN], [gamin])
				fi
			fi

			if test "x$BUILD_EXPORTER_TRUE" = "x" ; then
				if test "x$HAVE_PKG_CONFIG" = "xyes"; then
					PKG_CHECK_MODULES([FONTCONFIG], [fontconfig])
				fi
			fi

    	fi

        dnl Check whether boost is installed
        AX_BOOST_BASE

		CB_GLIB2_CFLAGS="$GLIB2_CFLAGS"
		CB_GLIB2_LIBS="$GLIB2_LIBS"
		CB_GAMIN_CFLAGS="$GAMIN_CFLAGS"
		CB_GAMIN_LIBS="$GAMIN_LIBS"
		CB_HUNSPELL_CFLAGS="$HUNSPELL_CFLAGS"
		CB_HUNSPELL_LIBS="$HUNSPELL_LIBS"
		CB_FONTCONFIG_CFLAGS="$FONTCONFIG_CFLAGS"
		CB_FONTCONFIG_LIBS="$FONTCONFIG_LIBS"
		WX_GTK_CFLAGS="$GTK_CFLAGS"
		WX_GTK_LIBS="$GTK_LIBS"
		AC_PATH_XTRA
		WX_X11_CFLAGS="`echo $X_CFLAGS | sed 's/ -INONE//'`"
		WX_X11_LIBS="`echo $X_LIBS | sed 's/ -LNONE//' | sed 's/ -RNONE//'`"
		if test "$no_x" != yes; then
			WX_X11_LIBS="$WX_X11_LIBS -lX11 $X_EXTRA_LIBS"
		fi
    ;;
    *wx_msw*)
        AC_MSG_RESULT(wxMSW)
    ;;
    *wx_mac*)
        AC_MSG_RESULT(wxMac)
        AC_CHECK_PROG(REZ, Rez, Rez, /Developer/Tools/Rez)
        POSTLINK_COMMAND="\$(REZ) -d __DARWIN__ -t APPL Carbon.r -o"
        AC_CHECK_PROG(SETFILE, SetFile, SetFile, /Developer/Tools/SetFile)
        MACSETFILE="\$(SETFILE)"
    ;;
    *wx_osx*)
        AC_MSG_RESULT(wxOSX)

        if test "x$BUILD_SPELLCHECKER_TRUE" = "x" ; then

            AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, yes, no)
            ifdef([PKG_CHECK_MODULES],[],[
                define([PKG_CHECK_MODULES],
                [ echo "You don't have pkg.m4 properly installed" >&2
                    exit 1
                ])
            ])
            PKG_PROG_PKG_CONFIG

            if test "x$HAVE_PKG_CONFIG" = "xyes"; then
                PKG_CHECK_MODULES([HUNSPELL], [hunspell])
            fi
        fi
        CB_HUNSPELL_CFLAGS="$HUNSPELL_CFLAGS"
        CB_HUNSPELL_LIBS="$HUNSPELL_LIBS"
    ;;
    *)
        AC_MSG_RESULT(other)
    ;;
esac

CB_GCC_VERSION
AM_CONDITIONAL([HAVE_GCC48], [test $GCC_MAJOR_VERSION -gt 4 -o $GCC_MAJOR_VERSION -eq 4 -a $GCC_MINOR_VERSION -ge 8])
AM_CONDITIONAL([HAVE_GCC6], [test $GCC_MAJOR_VERSION -ge 6])

AC_MSG_CHECKING([for wxWidgets >= 2.9.0])
AM_CONDITIONAL([HAVE_WX29], [test $wx_config_major_version -gt 2 -o $wx_config_major_version -eq 2 -a $wx_config_minor_version -ge 9])

if test "x$HAVE_WX29_TRUE" = "x" ; then
    AC_MSG_RESULT(yes (version $WX_VERSION))
    AC_MSG_NOTICE([(Re)setting libs for wxWidgets $WX_VERSION])
    WX_LIBS=`$WX_CONFIG_WITH_ARGS --libs std,aui,propgrid,richtext`
    WXPROPGRID_DIR=
    WXPROPGRID_LIB=
    WXPROPGRID_INCLUDE=
else
    AC_MSG_RESULT(no)
    WXPROPGRID_DIR=wxpropgrid
    WXPROPGRID_LIB=wxpropgrid/libwxpropgrid.la
    WXPROPGRID_INCLUDE=-I\$\(top_srcdir\)/src/sdk/wxpropgrid/include

    if test "x$HAVE_GCC48_TRUE" = "x" ; then
	gcc48_flags="-Wno-unused-local-typedefs"
        AC_MSG_NOTICE([setting additional flags for gcc $GCC_FULL_VERSION]: $gcc48_flags)
        CXXFLAGS="$gcc48_flags $CXXFLAGS"
    fi
fi

if test "x$HAVE_SQUIRREL_TRUE" = "x" ; then
	CB_SQUIRREL_CFLAGS="$SQUIRREL_CFLAGS"
	CB_SQUIRREL_LIBS="$SQUIRREL_LIBS"
else
	CB_SQUIRREL_CFLAGS="-I\$(top_srcdir)/src/include/scripting/include"
	CB_SQUIRREL_LIBS="\$(top_builddir)/src/sdk/scripting/squirrel/libsquirrel.la \$(top_builddir)/src/sdk/scripting/sqstdlib/libsqstdlib.la"
fi

if test "x$HAVE_TINYXML_TRUE" = "x" ; then
	CB_TINYXML_CFLAGS="$TINYXML_CFLAGS"
	CB_TINYXML_LIBS="$TINYXML_LIBS"
else
	CB_TINYXML_CFLAGS="-I\$(top_srcdir)/src/include/tinyxml -DTIXML_USE_STL=YES"
	CB_TINYXML_LIBS="\$(top_builddir)/src/base/tinyxml/libtinyxml.la"
fi

AC_SUBST(CB_SQUIRREL_CFLAGS)
AC_SUBST(CB_SQUIRREL_LIBS)
AC_SUBST(CB_TINYXML_CFLAGS)
AC_SUBST(CB_TINYXML_LIBS)
AC_SUBST(WX_GTK_CFLAGS)
AC_SUBST(WX_GTK_LIBS)
AC_SUBST(CB_GLIB2_CFLAGS)
AC_SUBST(CB_GLIB2_LIBS)
AC_SUBST(CB_GAMIN_CFLAGS)
AC_SUBST(CB_GAMIN_LIBS)
AC_SUBST(CB_HUNSPELL_CFLAGS)
AC_SUBST(CB_HUNSPELL_LIBS)
AC_SUBST(CB_FONTCONFIG_CFLAGS)
AC_SUBST(CB_FONTCONFIG_LIBS)
AC_SUBST(WX_X11_CFLAGS)
AC_SUBST(WX_X11_LIBS)
AC_SUBST(WXPROPGRID_DIR)
AC_SUBST(WXPROPGRID_LIB)
AC_SUBST(WXPROPGRID_INCLUDE)
AC_SUBST(WXPROPGRID_HEADERS)

AC_SUBST(POSTLINK_COMMAND)
AC_SUBST(MACSETFILE)

PIC_FLAGS=

AC_MSG_CHECKING(for PIC flags)
case `uname` in
    Solaris)
        if test "$GCC" = yes; then
            PIC_FLAGS="-fPIC -DPIC"
        else
            PIC_FLAGS="-KPIC -DPIC"
        fi
    ;;
    *)
        PIC_FLAGS="-fPIC -DPIC"
    ;;
esac
AC_MSG_RESULT($PIC_FLAGS)

CFLAGS="$CFLAGS -std=c90"
CPPFLAGS="$CPPFLAGS $CB_TINYXML_CFLAGS"
CXXFLAGS="$CXXFLAGS $PCH_FLAGS $PIC_FLAGS -fexceptions"
AC_SUBST(codeblocks_PCH_FLAGS, "$PCH_FLAGS")

AX_CXX_COMPILE_STDCXX(11, noext)

dnl AM_PATH_GTK(1.2.7, ,
dnl             AC_MSG_ERROR(Cannot find GTK: Is gtk-config in path?),
dnl             gthread)

AC_MSG_NOTICE([Configuring Code::Blocks...])
AC_MSG_NOTICE(SVN revision SVN_REVISION (SVN_DATE))
AC_SUBST(REVISION, SVN_REV)
AC_SUBST(DATE, "SVN_DATE")

AC_CONFIG_FILES([Makefile
	src/Makefile
	src/base/Makefile
	src/base/tinyxml/Makefile
	src/build_tools/Makefile
	src/build_tools/autorevision/Makefile
	src/include/autorevision.h
	src/include/Makefile
	src/include/scripting/Makefile
	src/include/scripting/bindings/Makefile
	src/include/scripting/include/Makefile
	src/include/scripting/sqplus/Makefile
	src/include/scripting/sqstdlib/Makefile
	src/include/scripting/squirrel/Makefile
	src/include/tinyxml/Makefile
	src/sdk/Makefile
	src/sdk/wxpropgrid/Makefile
	src/sdk/scripting/Makefile
	src/sdk/scripting/bindings/Makefile
	src/sdk/scripting/squirrel/Makefile
	src/sdk/scripting/sqstdlib/Makefile
	src/sdk/scripting/sqplus/Makefile
	src/sdk/wxscintilla/Makefile
	src/sdk/resources/Makefile
	src/sdk/resources/lexers/Makefile
	src/src/resources/Makefile
	src/src/resources/icons/Makefile
	src/src/resources/images/Makefile
	src/src/resources/images/16x16/Makefile
	src/src/resources/images/settings/Makefile
	src/src/Makefile
	src/plugins/Makefile
	src/plugins/astyle/Makefile
	src/plugins/astyle/resources/Makefile
	src/plugins/autosave/Makefile
	src/plugins/classwizard/Makefile
	src/plugins/classwizard/resources/Makefile
	src/plugins/codecompletion/Makefile
	src/plugins/codecompletion/resources/Makefile
	src/plugins/compilergcc/Makefile
	src/plugins/compilergcc/depslib/Makefile
	src/plugins/compilergcc/depslib/src/Makefile
	src/plugins/compilergcc/resources/Makefile
	src/plugins/debuggergdb/Makefile
	src/plugins/debuggergdb/resources/Makefile
	src/plugins/defaultmimehandler/Makefile
	src/plugins/defaultmimehandler/resources/Makefile
	src/plugins/openfileslist/Makefile
	src/plugins/occurrenceshighlighting/Makefile
	src/plugins/occurrenceshighlighting/resources/Makefile
	src/plugins/projectsimporter/Makefile
	src/plugins/projectsimporter/resources/Makefile
	src/plugins/scriptedwizard/Makefile
	src/plugins/scriptedwizard/resources/Makefile
	src/plugins/scriptedwizard/resources/arduino/Makefile
	src/plugins/scriptedwizard/resources/arm/Makefile
	src/plugins/scriptedwizard/resources/avr/Makefile
	src/plugins/scriptedwizard/resources/c_file/Makefile
	src/plugins/scriptedwizard/resources/console/Makefile
	src/plugins/scriptedwizard/resources/d/Makefile
	src/plugins/scriptedwizard/resources/directx/Makefile
	src/plugins/scriptedwizard/resources/dll/Makefile
	src/plugins/scriptedwizard/resources/d_source/Makefile
	src/plugins/scriptedwizard/resources/empty/Makefile
	src/plugins/scriptedwizard/resources/empty_file/Makefile
	src/plugins/scriptedwizard/resources/fltk/Makefile
	src/plugins/scriptedwizard/resources/fortran/Makefile
	src/plugins/scriptedwizard/resources/glfw/Makefile
	src/plugins/scriptedwizard/resources/glut/Makefile
	src/plugins/scriptedwizard/resources/gtk/Makefile
	src/plugins/scriptedwizard/resources/h_file/Makefile
	src/plugins/scriptedwizard/resources/irrlicht/Makefile
	src/plugins/scriptedwizard/resources/java/Makefile
	src/plugins/scriptedwizard/resources/lf/Makefile
	src/plugins/scriptedwizard/resources/matlab_csf/Makefile
	src/plugins/scriptedwizard/resources/mcs51/Makefile
	src/plugins/scriptedwizard/resources/msp430/Makefile
	src/plugins/scriptedwizard/resources/ogre/Makefile
	src/plugins/scriptedwizard/resources/opencv/Makefile
	src/plugins/scriptedwizard/resources/opengl/Makefile
	src/plugins/scriptedwizard/resources/plugins/Makefile
	src/plugins/scriptedwizard/resources/ppc/Makefile
	src/plugins/scriptedwizard/resources/qt4/Makefile
	src/plugins/scriptedwizard/resources/qt4dll/Makefile
	src/plugins/scriptedwizard/resources/qt5/Makefile
	src/plugins/scriptedwizard/resources/sdl/Makefile
	src/plugins/scriptedwizard/resources/sdl2/Makefile
	src/plugins/scriptedwizard/resources/sfml/Makefile
	src/plugins/scriptedwizard/resources/sharedlib/Makefile
	src/plugins/scriptedwizard/resources/smartwin/Makefile
	src/plugins/scriptedwizard/resources/staticlib/Makefile
	src/plugins/scriptedwizard/resources/stlport/Makefile
	src/plugins/scriptedwizard/resources/sys/Makefile
	src/plugins/scriptedwizard/resources/tricore/Makefile
	src/plugins/scriptedwizard/resources/win32gui/Makefile
	src/plugins/scriptedwizard/resources/wxwidgets/Makefile
	src/plugins/todo/Makefile
	src/plugins/todo/resources/Makefile
	src/plugins/abbreviations/Makefile
	src/plugins/abbreviations/resources/Makefile
	src/plugins/xpmanifest/Makefile
	src/plugins/contrib/Makefile
	src/plugins/contrib/appdata/Makefile
	src/plugins/contrib/AutoVersioning/Makefile
	src/plugins/contrib/BrowseTracker/Makefile
	src/plugins/contrib/byogames/Makefile
	src/plugins/contrib/Cccc/Makefile
	src/plugins/contrib/Cccc/resources/Makefile
	src/plugins/contrib/CppCheck/Makefile
	src/plugins/contrib/CppCheck/resources/Makefile
	src/plugins/contrib/cb_koders/Makefile
	src/plugins/contrib/codesnippets/Makefile
	src/plugins/contrib/codesnippets/resources/Makefile
	src/plugins/contrib/codestat/Makefile
	src/plugins/contrib/codestat/resources/Makefile
	src/plugins/contrib/copystrings/Makefile
	src/plugins/contrib/Cscope/Makefile
	src/plugins/contrib/devpak_plugin/Makefile
	src/plugins/contrib/devpak_plugin/bzip2/Makefile
	src/plugins/contrib/DoxyBlocks/Makefile
	src/plugins/contrib/DoxyBlocks/images/Makefile
	src/plugins/contrib/DoxyBlocks/images/16x16/Makefile
	src/plugins/contrib/dragscroll/Makefile
	src/plugins/contrib/EditorConfig/Makefile
	src/plugins/contrib/EditorTweaks/Makefile
	src/plugins/contrib/envvars/Makefile
	src/plugins/contrib/FileManager/Makefile
	src/plugins/contrib/headerfixup/Makefile
	src/plugins/contrib/help_plugin/Makefile
	src/plugins/contrib/help_plugin/bzip2/Makefile
	src/plugins/contrib/help_plugin/zlib/Makefile
	src/plugins/contrib/keybinder/Makefile
	src/plugins/contrib/NassiShneiderman/Makefile
	src/plugins/contrib/profiler/Makefile
	src/plugins/contrib/profiler/resources/Makefile
	src/plugins/contrib/ProjectOptionsManipulator/Makefile
	src/plugins/contrib/source_exporter/Makefile
	src/plugins/contrib/source_exporter/wxPdfDocument/Makefile
	src/plugins/contrib/SmartIndent/Makefile
	src/plugins/contrib/SpellChecker/Makefile
	src/plugins/contrib/SpellChecker/wxspellchecker/Makefile
	src/plugins/contrib/symtab/Makefile
	src/plugins/contrib/symtab/resources/Makefile
	src/plugins/contrib/regex_testbed/Makefile
	src/plugins/contrib/ReopenEditor/Makefile
	src/plugins/contrib/ThreadSearch/Makefile
	src/plugins/contrib/ThreadSearch/resources/Makefile
	src/plugins/contrib/ThreadSearch/resources/images/Makefile
	src/plugins/contrib/ThreadSearch/resources/images/16x16/Makefile
	src/plugins/contrib/ThreadSearch/resources/images/22x22/Makefile
	src/plugins/contrib/ToolsPlus/Makefile
	src/plugins/contrib/ToolsPlus/Resources/Makefile
	src/plugins/contrib/Valgrind/Makefile
	src/plugins/contrib/Valgrind/resources/Makefile
	src/plugins/contrib/wxContribItems/Makefile
	src/plugins/contrib/wxContribItems/wxsmith-contrib.pc
	src/plugins/contrib/wxContribItems/cb_wxcontrib.pc
	src/plugins/contrib/wxContribItems/cb_wxchartctrl.pc
	src/plugins/contrib/wxContribItems/cb_wxcustombutton.pc
	src/plugins/contrib/wxContribItems/cb_wximagepanel.pc
	src/plugins/contrib/wxContribItems/cb_wxflatnotebook.pc
	src/plugins/contrib/wxContribItems/cb_wxspeedbutton.pc
	src/plugins/contrib/wxContribItems/cb_wxKWIC.pc
	src/plugins/contrib/wxContribItems/cb_wxtreelist.pc
	src/plugins/contrib/wxContribItems/cb_wxled.pc
	src/plugins/contrib/wxContribItems/cb_wxmathplot.pc
	src/plugins/contrib/wxContribItems/wxchart/Makefile
	src/plugins/contrib/wxContribItems/wxFlatNotebook/Makefile
	src/plugins/contrib/wxContribItems/wxImagePanel/Makefile
	src/plugins/contrib/wxContribItems/wxthings/Makefile
	src/plugins/contrib/wxContribItems/wxSpeedButton/Makefile
	src/plugins/contrib/wxContribItems/KWIC/Makefile
	src/plugins/contrib/wxContribItems/KWIC/include/wx/KWIC/Makefile
	src/plugins/contrib/wxContribItems/KWIC/include/wx/xrc/Makefile
	src/plugins/contrib/wxContribItems/wxTreeList/Makefile
	src/plugins/contrib/wxContribItems/wxled/Makefile
	src/plugins/contrib/wxContribItems/wxled/gizmosled/include/wx/gizmos/Makefile
	src/plugins/contrib/wxContribItems/wxled/lcdWindow/include/wx/Makefile
	src/plugins/contrib/wxContribItems/wxled/led/include/wx/Makefile
	src/plugins/contrib/wxContribItems/wxled/led/include/xrc/Makefile
	src/plugins/contrib/wxContribItems/wxled/ledpanel/include/wx/Makefile
	src/plugins/contrib/wxContribItems/wxmathplot/Makefile
	src/plugins/contrib/wxSmith/Makefile
	src/plugins/contrib/wxSmith/wxsmith.pc
	src/plugins/contrib/wxSmith/plugin/Makefile
	src/plugins/contrib/wxSmith/properties/Makefile
	src/plugins/contrib/wxSmith/wxwidgets/Makefile
	src/plugins/contrib/wxSmith/wxwidgets/properties/Makefile
	src/plugins/contrib/wxSmith/wxwidgets/defitems/Makefile
	src/plugins/contrib/wxSmith/wxwidgets/icons/Makefile
	src/plugins/contrib/wxSmithContribItems/Makefile
	src/plugins/contrib/wxSmithAui/Makefile
	src/plugins/contrib/wxSmithAui/wxsmithaui.pc
	src/plugins/contrib/wxSmithDemo/Makefile
	src/plugins/contrib/wxSmithIExplore/Makefile
	src/plugins/contrib/wxSmithSTC/Makefile
	src/plugins/contrib/lib_finder/Makefile
	src/plugins/contrib/lib_finder/lib_finder/Makefile
	src/plugins/contrib/HexEditor/Makefile
	src/plugins/contrib/IncrementalSearch/Makefile
	src/plugins/contrib/MouseSap/Makefile
	src/mime/Makefile
	src/scripts/Makefile
	src/tools/Makefile
	src/tools/cb_share_config/Makefile
	src/tools/ConsoleRunner/Makefile
	src/tools/CBLauncher/Makefile
	src/templates/Makefile
	src/templates/common/Makefile
	src/templates/unix/Makefile
	src/templates/win32/Makefile
	src/wxsmith/Makefile
	codeblocks.pc
	codeblocks.spec.fedora
	codeblocks.spec
	codeblocks.plist
])

AC_OUTPUT

echo
echo
echo "*************************************************"
echo "* Code::Blocks source tree has been configured. *"
echo "*************************************************"
echo
echo "You can now build Code::Blocks by issuing 'make'".
echo "When the build is complete, become root and install"
echo "it by issuing 'make install'".
echo
