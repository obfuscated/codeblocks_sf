AM_CPPFLAGS = -I$(top_srcdir)/src/include/scripting/include \
				-I$(top_srcdir)/src/include/scripting/squirrel

noinst_LTLIBRARIES = libsquirrel.la

libsquirrel_la_SOURCES = sqfuncstate.cpp \
                        sqcompiler.cpp \
                        sqmem.cpp \
                        sqlexer.cpp \
                        sqstate.cpp \
                        sqvm.cpp \
                        sqtable.cpp \
                        sqbaselib.cpp \
                        sqclass.cpp \
                        sqobject.cpp \
                        sqapi.cpp \
                        sqdebug.cpp

EXTRA_DIST = squirrel71.vcproj \
            squirrel.cbp \
            squirrellib.cbp \
            squirrel.vcproj
