INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		$(WXPROPGRID_INCLUDE)

pluginlib_LTLIBRARIES = libwxsmith.la

pluginlibdir = $(pkglibdir)/plugins

libwxsmith_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_la_LIBADD = ../../../../sdk/libcodeblocks.la ../libwxsmithlib.la $(WX_LIBS)

libwxsmith_la_SOURCES = wxsmithpluginregistrants.cpp

