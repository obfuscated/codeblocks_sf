AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libProjectOptionsManipulator.la

libProjectOptionsManipulator_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libProjectOptionsManipulator_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libProjectOptionsManipulator_la_SOURCES = ProjectOptionsManipulator.cpp \
		ProjectOptionsManipulatorDlg.cpp \
		ProjectOptionsManipulatorResultDlg.cpp

noinst_HEADERS = ProjectOptionsManipulator.h \
		ProjectOptionsManipulatorDlg.h \
		ProjectOptionsManipulatorResultDlg.h

EXTRA_DIST =	$(srcdir)/*.cbp \
		manifest.xml \
		$(srcdir)/wxsmith/*.wxs

pkgdata_DATA = ProjectOptionsManipulator.zip

CLEANFILES = $(pkgdata_DATA)

ProjectOptionsManipulator.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/ProjectOptionsManipulator.zip manifest.xml > /dev/null
