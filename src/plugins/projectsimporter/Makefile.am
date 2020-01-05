SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libprojectsimporter.la

libprojectsimporter_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libprojectsimporter_la_LIBADD = ../../sdk/libcodeblocks.la \
				$(WX_LIBS) \
				$(CB_TINYXML_LIBS)

libprojectsimporter_la_SOURCES = projectsimporter.cpp \
				devcpploader.cpp \
				msvc10loader.cpp \
				msvc7loader.cpp \
				msvc7workspaceloader.cpp \
				msvcloader.cpp \
				msvcworkspacebase.cpp \
				msvcworkspaceloader.cpp

noinst_HEADERS = projectsimporter.h \
				devcpploader.h \
				msvc10loader.h \
				msvc7loader.h \
				msvc7workspaceloader.h \
				msvcloader.h \
				msvcworkspacebase.h \
				msvcworkspaceloader.h
