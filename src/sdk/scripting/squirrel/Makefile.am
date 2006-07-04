INCLUDES = -I$(top_srcdir)/src/sdk/scripting/include

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
                            
noinst_HEADERS = sqtable.h \
                sqopcodes.h \
                squtils.h \
                sqclass.h \
                sqstring.h \
                sqobject.h \
                sqpcheader.h \
                sqarray.h \
                sqfuncstate.h \
                squserdata.h \
                sqcompiler.h \
                sqclosure.h \
                sqfuncproto.h \
                sqlexer.h \
                sqstate.h \
                sqvm.h


EXTRA_DIST = squirrel71.vcproj \
            squirrel.cbp \
            squirrellib.cbp \
            squirrel.vcproj
