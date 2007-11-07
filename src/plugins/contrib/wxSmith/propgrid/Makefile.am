INCLUDES = $(WX_CXXFLAGS) \
		-I$(srcdir)/contrib/include

AM_CPPFLAGS = -DWXMAKINGLIB_PROPGRID -DwxPG_USE_WXMODULE=0

noinst_LTLIBRARIES = libpropgrid.la

libpropgrid_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libpropgrid_la_SOURCES = ./contrib/src/propgrid/advprops.cpp \
			./contrib/src/propgrid/extras.cpp \
			./contrib/src/propgrid/manager.cpp \
			./contrib/src/propgrid/odcombo.cpp \
			./contrib/src/propgrid/propgrid.cpp \
 			./contrib/src/propgrid/props.cpp

EXTRA_DIST = ./contrib/readme-propgrid.txt \
			./contrib/CHANGES-propgrid.txt


noinst_HEADERS = ./contrib/include/wx/propgrid/advprops.h \
			./contrib/include/wx/propgrid/extras.h \
			./contrib/include/wx/propgrid/manager.h \
			./contrib/include/wx/propgrid/odcombo.h \
			./contrib/include/wx/propgrid/pg_dox_mainpage.h \
			./contrib/include/wx/propgrid/propdev.h \
			./contrib/include/wx/propgrid/propgrid.h
