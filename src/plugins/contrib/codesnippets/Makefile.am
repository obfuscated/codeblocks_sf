SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
        $(WX_GTK2_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
        -I$(top_srcdir)/src/include/wxFlatNotebook/include \
        -DBUILDING_PLUGIN

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodesnippets.la

libcodesnippets_la_LDFLAGS = $(WX_GTK2_LIBS) -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodesnippets_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libcodesnippets_la_SOURCES = codesnippets.cpp \
			codesnippetstreectrl.cpp \
			codesnippetswindow.cpp \
			edit.cpp \
			editsnippetframe.cpp \
			messagebox.cpp \
			messageBoxForm.cpp \
			prefs.cpp \
			settingsdlg.cpp \
			settingsdlgform.cpp \
			snippetitemdata.cpp \
			snippetproperty.cpp \
			snippetpropertyform.cpp \
			snippetsconfig.cpp \
			snippetsimages.cpp \
			finddialogs.cpp \
			memorymappedfile.cpp \
			version.cpp

noinst_HEADERS =  	codesnippets.h \
			codesnippetstreectrl.h \
			codesnippetswindow.h \
			defsext.h \
			edit.h \
			editsnippetframe.h \
			menuidentifiers.h \
			messagebox.h \
			messageBoxForm.h \
			prefs.h \
			settingsdlg.h \
			settingsdlgform.h \
			snippetitemdata.h \
			snippetproperty.h \
			snippetpropertyform.h \
			snippetsconfig.h \
			snippetsimages.h \
			finddialogs.h \
			memorymappedfile.h \
			version.h \
			wx_pch.h

man_MANS = codesnippets.1

EXTRA_DIST = 	codesnippets.cbp \
		codesnippets-unix.cbp \
		update.bat \
		update \
		readme.html \
		license.txt \
		$(man_MANS)

