INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libbyogames.la

libbyogames_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libbyogames_la_LIBADD = $(WX_LIBS) ../../../sdk/libcodeblocks.la

libbyogames_la_SOURCES = byocbtris.cpp \
            byogame.cpp \
            byogameselect.cpp \
            byogames.cpp \
            byogamebase.cpp \
            byoconf.cpp \
		byosnake.cpp


noinst_HEADERS = byogameselect.h \
            byocbtris.h \
            byogame.h \
            byogames.h \
            byogamebase.h \
            byoconf.h \
		byosnake.h

EXTRA_DIST = wxsmith/byoGameSelect.wxs \
            byogames.cbp \
            byogames-unix.cbp \
            manifest.xml

pkgdata_DATA = byogames.zip
CLEANFILES = $(pkgdata_DATA)

byogames.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/byogames.zip manifest.xml > /dev/null
