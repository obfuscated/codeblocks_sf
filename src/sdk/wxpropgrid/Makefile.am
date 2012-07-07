INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk/wxpropgrid/include

AM_CPPFLAGS = -DEXPORT_LIB -DwxPG_SUPPORT_TOOLTIPS

noinst_LTLIBRARIES = libwxpropgrid.la

libwxpropgrid_la_SOURCES = ./src/advprops.cpp \
			./src/editors.cpp \
			./src/extras.cpp \
			./src/manager.cpp \
			./src/odcombo.cpp \
			./src/propgrid.cpp \
 			./src/props.cpp \
 			./src/xh_propgrid.cpp

EXTRA_DIST = ./readme.txt \
			./changes.txt

sdkdir=$(includedir)/codeblocks/wxpropgrid/include/wx/propgrid/

sdk_HEADERS = ./include/wx/propgrid/advprops.h \
			./include/wx/propgrid/editors.h \
			./include/wx/propgrid/extras.h \
			./include/wx/propgrid/manager.h \
			./include/wx/propgrid/odcombo.h \
			./include/wx/propgrid/pg_dox_mainpage.h \
			./include/wx/propgrid/propdev.h \
			./include/wx/propgrid/propgrid.h \
			./include/wx/propgrid/props.h \
			./include/wx/propgrid/xh_propgrid.h
