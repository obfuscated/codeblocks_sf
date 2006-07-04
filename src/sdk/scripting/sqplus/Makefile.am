INCLUDES = -I$(top_srcdir)/src/sdk/scripting/include

noinst_LTLIBRARIES = libsqplus.la

libsqplus_la_SOURCES = SqPlus.cpp \
                        SquirrelVM.cpp \
                        SquirrelBindingsUtils.cpp \
                        SquirrelObject.cpp
                        
noinst_HEADERS = SqPlusConst.h \
                SquirrelBindingsUtils.h \
                SquirrelVM.h \
                sqplus.h \
                SquirrelObject.h

EXTRA_DIST = sqplus71.vcproj \
            sqplus.cbp \
            sqplus.vcproj \
            SquirrelBindingsUtilsWin32.cpp \
            SquirrelBindingsUtilsWin32.h \
            sqplusWin32.h
