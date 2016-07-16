SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_TINYXML_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libcodestat.la

libcodestat_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodestat_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libcodestat_la_SOURCES = codestatconfig.cpp \
			codestat.cpp \
			codestatexec.cpp

noinst_HEADERS = codestatconfig.h \
			codestatexec.h \
			codestat.h \
			language_def.h

EXTRA_DIST = $(srcdir)/*.cbp \
			InstallationInstructions.txt \
			BuildInstructions.txt
