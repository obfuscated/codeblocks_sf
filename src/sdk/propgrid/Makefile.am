INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include/propgrid/include

noinst_LTLIBRARIES = libpropgrid.la

libpropgrid_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libpropgrid_la_SOURCES = ./src/propgrid/advprops.cpp \
			./src/propgrid/extras.cpp \
			./src/propgrid/manager.cpp \
			./src/propgrid/odcombo.cpp \
			./src/propgrid/propgrid.cpp \
 			./src/propgrid/props.cpp

EXTRA_DIST = readme-propgrid.txt \
			CHANGES-propgrid.txt
