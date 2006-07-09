SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libprofiler.la

libprofiler_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libprofiler_la_LIBADD = ../../../sdk/libcodeblocks.la

libprofiler_la_SOURCES = cbprofilerconfig.cpp \
			cbprofiler.cpp \
			cbprofilerexec.cpp

noinst_HEADERS = cbprofilerconfig.h \
			cbprofilerexec.h \
			cbprofiler.h

EXTRA_DIST = InstallationInstructions.txt \
			BuildInstructions.txt \
			cbprofiler.cbp \
			cbprofiler.unix.cbp
