AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(srcdir)/include \
		-I$(srcdir)/images

wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems

wxcontribitems_LTLIBRARIES = libwxdatetimepicker.la

libwxdatetimepicker_la_LDFLAGS = -version-info 0:1:0

libwxdatetimepicker_la_LIBADD = $(WX_LIBS)

libwxdatetimepicker_la_SOURCES = ./src/DateTimeCtrl.cpp \
./src/TimeCtrl.cpp

sdkdir = $(includedir)/codeblocks/wxContribItems/wxDateTimePicker/include

sdk_HEADERS = ./include/DateTimeCtrl.h \
./include/TimeCtrl.h

EXTRA_DIST = $(srcdir)/images/*.xpm
