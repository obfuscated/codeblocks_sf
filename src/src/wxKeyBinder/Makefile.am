INCLUDES = $(WX_CXXFLAGS) -I$(top_srcdir)/src/src/wxKeyBinder/include

noinst_LTLIBRARIES = libwxkeybinder.la

libwxkeybinder_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libwxkeybinder_la_SOURCES = src/keybinder.cpp \
				src/menuutils.cpp

noinst_HEADERS = include/wx/keybinder.h \
				include/wx/keybinderdef.h \
				include/wx/menuutils.h

EXTRA_DIST = $(srcdir)/Readme.txt
