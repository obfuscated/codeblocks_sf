INCLUDES = $(WX_CXXFLAGS) \
           -I$(top_srcdir)/src/include \
           -I$(top_srcdir)/src/sdk/wxscintilla/include

CXXFLAGS = @CXXFLAGS@  -O0

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libheaderfixup.la

libheaderfixup_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libheaderfixup_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libheaderfixup_la_SOURCES = bindings.cpp \
                            configuration.cpp \
                            defaults.cpp \
                            execution.cpp \
                            fileanalysis.cpp \
                            headerfixup.cpp \
                            helper.cpp \
                            protocol.cpp

noinst_HEADERS = bindings.h \
                 configuration.h \
                 execution.h \
                 fileanalysis.h \
                 headerfixup.h \
                 helper.h \
                 protocol.h

EXTRA_DIST = $(srcdir)/*.cbp \
             wxsmith/configuration.wxs \
             wxsmith/execution.wxs \
             wxsmith/protocol.wxs \
             todo.txt \
             manifest.xml

pkgdata_DATA = headerfixup.zip
CLEANFILES = $(pkgdata_DATA)

headerfixup.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/headerfixup.zip manifest.xml > /dev/null
