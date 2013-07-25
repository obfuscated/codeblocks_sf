AM_CPPFLAGS = -I$(top_srcdir)/src/include/scripting/include \
					-I$(top_srcdir)/src/include/scripting/sqplus

noinst_LTLIBRARIES = libsqplus.la

libsqplus_la_SOURCES = SqPlus.cpp \
                        SquirrelVM.cpp \
                        SquirrelBindingsUtils.cpp \
                        SquirrelObject.cpp

EXTRA_DIST = sqplus71.vcproj \
            sqplus.cbp \
            sqplus.vcproj \
            SquirrelBindingsUtilsWin32.cpp
