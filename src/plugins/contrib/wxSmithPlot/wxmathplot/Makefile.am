AM_CPPFLAGS = $(WX_CXXFLAGS)

noinst_LTLIBRARIES = libwxsmithplot_wxmathplot.la

libwxsmithplot_wxmathplot_la_LDFLAGS = -pthread -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmithplot_wxmathplot_la_LIBADD = $(WX_LIBS)

EXTRA_DIST = 	$(srcdir)/*.cpp \
		$(srcdir)/*.h


libwxsmithplot_wxmathplot_la_SOURCES = \
./mathplot.cpp

noinst_HEADERS = \
./mathplot.h
