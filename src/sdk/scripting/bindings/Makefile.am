AM_CPPFLAGS = $(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			$(CB_TINYXML_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include/scripting/bindings \
			-I$(top_srcdir)/src/include/scripting/sqplus

noinst_LTLIBRARIES = libsqbindings.la

libsqbindings_la_SOURCES = scriptbindings.cpp \
                            sc_consts.cpp \
                            sc_dialog.cpp \
                            sc_globals.cpp \
                            sc_io.cpp \
                            sc_plugin.cpp \
                            sc_progress.cpp \
                            sc_util_dialogs.cpp \
                            sc_wxtypes.cpp
