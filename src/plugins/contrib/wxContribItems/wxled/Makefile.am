SUBDIRS = gizmosled/include/wx/gizmos lcdWindow/include/wx led/include/wx ledpanel/include/wx

INCLUDES = -I$(srcdir)/gizmosled/include \
 $(WX_CXXFLAGS) \
-I$(srcdir)/ledpanel/include \
-I$(srcdir)/lcdWindow/include \
-I$(srcdir)/led/include

wxcontribitemsdir=$(libdir)/codeblocks/wxContribItems

wxcontribitems_LTLIBRARIES = libwxled.la

libwxled_la_LDFLAGS = -version-info 0:1:0

libwxled_la_LIBADD = $(WX_LIBS)

libwxled_la_SOURCES = \
./gizmosled/src/ledctrl.cpp \
./lcdWindow/src/lcdwindow.cpp\
./led/src/led.cpp \
./ledpanel/src/advancedmatrixobject.cpp \
./ledpanel/src/font75.cpp \
./ledpanel/src/font77.cpp \
./ledpanel/src/matrixobject.cpp \
./ledpanel/src/wxledfont.cpp \
./ledpanel/src/wxledpanel.cpp
