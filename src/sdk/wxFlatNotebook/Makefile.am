INCLUDES = $(WX_CXXFLAGS) \
			-I$(srcdir)/include

sdkdir=$(includedir)/codeblocks/wxFlatNotebook

noinst_LTLIBRARIES = libwxflatnotebook.la

libwxflatnotebook_la_SOURCES = src/wxFlatNotebook/wxFlatNotebook.cpp \
src/wxFlatNotebook/fnb_resources.cpp \
src/wxFlatNotebook/popup_dlg.cpp \
src/wxFlatNotebook/renderer.cpp

sdk_HEADERS = include/wx/wxFlatNotebook/wxFNBDropTarget.h \
include/wx/wxFlatNotebook/wxFlatNotebook.h \
include/wx/wxFlatNotebook/fnb_resources.h \
include/wx/wxFlatNotebook/popup_dlg.h \
include/wx/wxFlatNotebook/renderer.h \
include/wx/wxFlatNotebook/singleton.h \
include/wx/wxFlatNotebook/smart_ptr.h

noinst_DATA = AUTHORS README README.patch cb_wxfn.patch
EXTRA_DIST = AUTHORS README README.patch cb_wxfn.patch
