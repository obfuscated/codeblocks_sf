SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/plugins/astyle/astyle \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libastyle.la

libastyle_la_LDFLAGS = -module -version-info 0:1:0

libastyle_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libastyle_la_SOURCES = astyleconfigdlg.cpp \
			astyleplugin.cpp \
			formattersettings.cpp \
			astyle/ASBeautifier.cpp \
			astyle/ASFormatter.cpp

noinst_HEADERS = astyleconfigdlg.h \
			astyleplugin.h \
			formattersettings.h \
			astyle/astyle.h \
			astyle/compiler_defines.h
