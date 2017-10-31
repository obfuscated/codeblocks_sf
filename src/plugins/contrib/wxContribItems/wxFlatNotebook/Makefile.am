AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(srcdir)/include

wxcontribitemsdir=$(libdir)/codeblocks/wxContribItems

wxcontribitems_LTLIBRARIES = libwxflatnotebook.la

libwxflatnotebook_la_LDFLAGS = -version-info 0:1:0

libwxflatnotebook_la_LIBADD = $(WX_LIBS)

libwxflatnotebook_la_SOURCES = \
./src/wxFlatNotebook/fnb_customize_dlg.cpp \
./src/wxFlatNotebook/fnb_resources.cpp \
./src/wxFlatNotebook/popup_dlg.cpp \
./src/wxFlatNotebook/renderer.cpp \
./src/wxFlatNotebook/wxFlatNotebook.cpp \
./src/wxFlatNotebook/xh_fnb.cpp

sdkdir=$(includedir)/codeblocks/wxContribItems/wxFlatNotebook/include/wx/wxFlatNotebook

sdk_HEADERS = \
./include/wx/wxFlatNotebook/wxFlatNotebook.h \
./include/wx/wxFlatNotebook/wxFlatNotebookSDK.h \
./include/wx/wxFlatNotebook/wxFNBDropTarget.h \
./include/wx/wxFlatNotebook/fnb_singleton.h \
./include/wx/wxFlatNotebook/fnb_smart_ptr.h \
./include/wx/wxFlatNotebook/renderer.h


noinst_HEADERS = \
./include/wx/wxFlatNotebook/fnb_customize_dlg.h \
./include/wx/wxFlatNotebook/fnb_resources.h \
./include/wx/wxFlatNotebook/popup_dlg.h \
./include/wx/wxFlatNotebook/xh_fnb.h

EXTRA_DIST = README
