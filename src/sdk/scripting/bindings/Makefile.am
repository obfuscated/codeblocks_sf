INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk/scripting/include \
			-I$(top_srcdir)/src/sdk/scripting/sqplus

sdkdir=$(includedir)/codeblocks/scripting/bindings

noinst_LTLIBRARIES = libsqbindings.la

libsqbindings_la_SOURCES = scriptbindings.cpp \
                            sc_consts.cpp \
                            sc_globals.cpp \
                            sc_plugin.cpp \
                            sc_wxtypes.cpp \
                            sc_io.cpp

noinst_HEADERS = scriptbindings.h

sdk_HEADERS = sc_base_types.h sc_plugin.h
