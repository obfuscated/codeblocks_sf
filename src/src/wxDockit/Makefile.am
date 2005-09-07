INCLUDES = $(WX_CXXFLAGS) -I$(top_srcdir)/src/src/wxDockit/include

lib_LTLIBRARIES = libwxdockit.la

libwxdockit_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

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
if CODEBLOCKS_LINUX
libwxdockit_la_SOURCES += src/gtk/dockwindow_gtk.cpp \
				src/gtk/gdi_gtk.cpp \
				src/gtk/pane_gtk.cpp
else
libwxdockit_la_SOURCES += src/msw/dockwindow_msw.cpp \
				src/msw/gdi_msw.cpp \
				src/msw/pane_msw.cpp \
				src/msw/toolbutton_msw.cpp
endif

noinst_HEADERS = include/wx/generic/toolbutton.h \
				include/wx/barholder.h \
				include/wx/dockhost.h \
				include/wx/dockit_defs.h \
				include/wx/dockpanel.h \
				include/wx/dockwindow.h \
				include/wx/exsplitter.h \
				include/wx/gdi.h \
				include/wx/gripper.h \
				include/wx/layoutmanager.h \
				include/wx/pane.h \
				include/wx/slidebar.h \
				include/wx/toolbutton.h \
				include/wx/util.h \
				include/wx/gtk/dockwindow.h \
				include/wx/gtk/gdi.h \
				include/wx/gtk/pane.h \
				include/wx/msw/dockwindow.h \
				include/wx/msw/gdi.h \
				include/wx/msw/pane.h \
				include/wx/msw/toolbutton.h

EXTRA_DIST = $(srcdir)/*.txt \
				$(srcdir)/lib/readme.txt \
				$(srcdir)/src
