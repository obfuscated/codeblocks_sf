INCLUDES = $(WX_CXXFLAGS) -I$(top_srcdir)/src/src/wxDockit/include

lib_LTLIBRARIES = libwxdockit.la

libwxdockit_la_SOURCES = src/generic/barholder.cpp \
				src/generic/dockhost.cpp \
				src/generic/dockpanel.cpp \
				src/generic/dockwindow.cpp \
				src/generic/exsplitter.cpp \
				src/generic/gdi.cpp \
				src/generic/gripper.cpp \
				src/generic/layoutmanager.cpp \
				src/generic/pane.cpp \
				src/generic/slidebar.cpp \
				src/generic/toolbutton.cpp \
				src/generic/util.cpp
if LINUX
libwxdockit_la_SOURCES += src/gtk/dockwindow_gtk.cpp \
				src/gtk/gdi_gtk.cpp \
				src/gtk/pane_gtk.cpp
else
libwxdockit_la_SOURCES += src/msw/dockwindow_msw.cpp \
				src/msw/gdi_msw.cpp \
				src/msw/pane_msw.cpp \
				src/msw/toolbutton_msw.cpp
endif
