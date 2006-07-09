INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk/propgrid/include

sdkdir=$(includedir)/codeblocks/propgrid/include/wx/propgrid

noinst_LTLIBRARIES = libpropgrid.la

libpropgrid_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libpropgrid_la_SOURCES = ./src/propgrid/advprops.cpp \
			./src/propgrid/custctrl.cpp \
			./src/propgrid/manager.cpp \
			./src/propgrid/odcombo.cpp \
			./src/propgrid/propgrid.cpp

sdk_HEADERS = ./include/wx/propgrid/advprops.h \
			./include/wx/propgrid/custctrl.h \
			./include/wx/propgrid/manager.h \
			./include/wx/propgrid/odcombo.h \
			./include/wx/propgrid/pg_dox_mainpage.h \
			./include/wx/propgrid/propdev.h \
			./include/wx/propgrid/propgrid.h

EXTRA_DIST = readme-propgrid.txt \
			CHANGES-propgrid.txt
