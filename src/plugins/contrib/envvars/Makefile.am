INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/scripting/include \
		-I$(top_srcdir)/src/include/scripting/sqplus \
		-I$(top_srcdir)/src/include/scripting/bindings \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins
pkgdata_DATA = envvars.zip

pluginlib_LTLIBRARIES = libenvvars.la

libenvvars_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libenvvars_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libenvvars_la_SOURCES = envvars.cpp \
                        envvars_cfgdlg.cpp \
                        envvars_common.cpp \
                        envvars_prjoptdlg.cpp

noinst_HEADERS = envvars.h \
                 envvars_cfgdlg.h \
                 envvars_common.h \
                 envvars_prjoptdlg.h

EXTRA_DIST = envvars.cbp \
             envvars-unix.cbp \
             envvars.xrc \
             project_settings.xrc \
             manifest.xml

CLEANFILES = $(pkgdata_DATA)

envvars.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/envvars.zip manifest.xml *.xrc > /dev/null
