SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(WX_GTK_CFLAGS) \
		$(WX_X11_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/contrib/codesnippets \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libcodesnippets.la

libcodesnippets_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodesnippets_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS) $(WX_GTK_LIBS) $(WX_X11_LIBS)

libcodesnippets_la_SOURCES = codesnippets.cpp \
		codesnippetstreectrl.cpp \
		codesnippetswindow.cpp \
		GenericMessageBox.cpp \
		settingsdlg.cpp \
		settingsdlgform.cpp \
		snippetitemdata.cpp \
		snippetproperty.cpp \
		snippetpropertyform.cpp \
		snippetsconfig.cpp \
		snippetsimages.cpp \
		version.cpp


noinst_HEADERS =  	codesnippets.h \
		codesnippetstreectrl.h \
		codesnippetswindow.h \
		menuidentifiers.h \
		GenericMessageBox.h \
		settingsdlg.h \
		settingsdlgform.h \
		snippetitemdata.h \
		snippetproperty.h \
		snippetpropertyform.h \
		snippetsconfig.h \
		snippetsimages.h \
		FileImport.h \
		version.h \
		wx_pch.h

man_MANS = codesnippets.1

EXTRA_DIST = 	$(srcdir)/*.cbp \
		generic.rc \
		generic.Manifest \
		$(srcdir)/update* \
		readme.html \
		license.txt \
		manifest.xml \
		$(man_MANS)

