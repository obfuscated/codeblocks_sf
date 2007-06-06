INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = liblib_finder.la

liblib_finder_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

liblib_finder_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

liblib_finder_la_SOURCES = 	dirlistdlg.cpp \
				lib_finder.cpp \
				libraryconfigmanager.cpp \
				processingdlg.cpp \
				resultmap.cpp

noinst_HEADERS =  	dirlistdlg.h \
			lib_finder.h \
			libraryconfig.h \
			libraryconfigmanager.h \
			libraryresult.h \
			processingdlg.h \
			resultmap.h

EXTRA_DIST = 	lib_finder.cbp \
		update \
		update.bat \
		readme.txt \
		manifest.xml
#		license.txt 

pkgdata_DATA = lib_finder.zip
CLEANFILES = $(pkgdata_DATA)

lib_finder.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/lib_finder.zip manifest.xml *.xrc > /dev/null
