SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/scripting/include \
		-I$(top_srcdir)/src/include/scripting/sqplus

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libscriptedwizard.la

libscriptedwizard_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libscriptedwizard_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libscriptedwizard_la_SOURCES = buildtargetpanel.cpp \
                                compilerpanel.cpp \
                                filepathpanel.cpp \
                                genericselectpath.cpp \
								infopanel.cpp \
								genericsinglechoicelist.cpp \
								projectpathpanel.cpp \
								wiz.cpp \
								wizpage.cpp

noinst_HEADERS = buildtargetpanel.h \
                    compilerpanel.h \
                    filepathpanel.h \
                    genericselectpath.h \
					infopanel.h \
					genericsinglechoicelist.h \
					projectpathpanel.h \
					wiz.h \
					wizpage.h

EXTRA_DIST = wxsmith/*
