AM_CPPFLAGS = $(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

noinst_LTLIBRARIES = libsqbindings.la

libsqbindings_la_SOURCES = \
	sc_consts.cpp \
	sc_globals.cpp \
	sc_io.cpp \
	sc_plugin.cpp \
	sc_progress.cpp \
	sc_util_dialogs.cpp \
	sc_utils.cpp \
	sc_wxtypes.cpp \
	scriptbindings.cpp
