bin_PROGRAMS = cb_share_config

INCLUDES = $(WX_CXXFLAGS)

cb_share_config_LDFLAGS = 

cb_share_config_LDADD =  -L../../sdk/tinyxml -ltinyxml \
			$(WX_LIBS)

cb_share_config_SOURCES = app.cpp \
			mainframe.cpp

noinst_HEADERS = app.h \
			mainframe.h \
			wx_pch.h
