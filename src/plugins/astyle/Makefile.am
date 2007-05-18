SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/plugins/astyle/astyle \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libastyle.la

libastyle_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libastyle_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libastyle_la_SOURCES = asstreamiterator.cpp \
      astyleconfigdlg.cpp \
			astyleplugin.cpp \
			dlgformattersettings.cpp \
			formattersettings.cpp \
			astyle/ASBeautifier.cpp \
			astyle/ASFormatter.cpp \
			astyle/ASEnhancer.cpp \
			astyle/ASResource.cpp

noinst_HEADERS = asstreamiterator.h \
      astyleconfigdlg.h \
			astyleplugin.h \
			formattersettings.h \
			dlgformattersettings.h \
			astyle/astyle.h

EXTRA_DIST = astyle/astyle.html \
			astyle/astyle_main.cpp \
			astyle/astyle_release_notes.html \
      astyle/astyleX.html \
			astyle/install.html \
			astyle/license.html
