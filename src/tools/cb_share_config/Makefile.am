bin_PROGRAMS = cb_share_config

AM_CPPFLAGS = $(WX_CXXFLAGS) \
			$(CB_GLIB2_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/tinyxml

cb_share_config_LDFLAGS =

cb_share_config_LDADD =  ../../base/tinyxml/libtinyxml.la \
			$(WX_LIBS) \
			$(CB_GLIB2_LIBS)

cb_share_config_SOURCES = app.cpp \
			mainframe.cpp

noinst_HEADERS = app.h \
			mainframe.h

man_MANS = cb_share_config.1

EXTRA_DIST = $(srcdir)/*.cbp \
		$(srcdir)/wxsmith/*.wxs \
		$(man_MANS)
