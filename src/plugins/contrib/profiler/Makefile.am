SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libProfiler.la

libProfiler_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libProfiler_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libProfiler_la_SOURCES = cbprofilerconfig.cpp \
			cbprofiler.cpp \
			cbprofilerexec.cpp

noinst_HEADERS = cbprofilerconfig.h \
			cbprofilerexec.h \
			cbprofiler.h

EXTRA_DIST = cbprofiler.cbp \
		cbprofiler.unix.cbp
