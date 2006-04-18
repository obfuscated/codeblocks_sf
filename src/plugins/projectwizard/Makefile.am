SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/as/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libprojectwizard.la

libprojectwizard_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libprojectwizard_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libprojectwizard_la_SOURCES = compilerpanel.cpp \
								intropanel.cpp \
								languagepanel.cpp \
								projectpathpanel.cpp \
								wiz.cpp \
								wizpage.cpp

noinst_HEADERS = compilerpanel.h \
					intropanel.h \
					languagepanel.h \
					projectpathpanel.h \
					wiz.h \
					wizpage.h

EXTRA_DIST = wxsmith/*
