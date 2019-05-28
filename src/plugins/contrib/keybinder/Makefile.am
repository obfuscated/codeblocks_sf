AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libkeybinder.la

libkeybinder_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libkeybinder_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libkeybinder_la_SOURCES = cbkeybinder.cpp \
			cbkeyConfigPanel.cpp \
			cJSON.cpp \
			clKeyboardBindingConfig.cpp \
			clKeyboardManager.cpp \
			configurationpanel.cpp \
			json_node.cpp \
			keybinder.cpp \
			menuutils.cpp

noinst_HEADERS = cbkeybinder.h \
			cbKeyConfigPanel.h \
			cJSON.h \
			clKeyboardBindingConfig.h \
			clKeyboardManager.h \
			configurationpanel.h \
			debugging.h \
			json_node.h \
			keybinder.h \
			macros.h \
			menuutils.h \
			wxstringhash.h

EXTRA_DIST = $(srcdir)/*.cbp manifest.xml


pkgdata_DATA = keybinder.zip
CLEANFILES = $(pkgdata_DATA)

keybinder.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/keybinder.zip manifest.xml > /dev/null
