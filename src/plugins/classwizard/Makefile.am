SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libclasswizard.la

libclasswizard_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libclasswizard_la_LIBADD = ../../sdk/libcodeblocks.la $(WX_LIBS)

libclasswizard_la_SOURCES = classwizard.cpp classwizarddlg.cpp
noinst_HEADERS = classwizard.h classwizarddlg.h
