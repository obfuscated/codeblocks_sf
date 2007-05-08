INCLUDES = $(WX_CXXFLAGS) \
        $(WX_GTK2_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
        -I$(top_srcdir)/src/include/wxFlatNotebook/include


libdir = $(pkgdatadir)/plugins

##lib_LTLIBRARIES = codesnippets
bin_PROGRAMS = codesnippets

codesnippets_LDFLAGS = 

##codesnippets_LIBADD = $(PLUGIN_WX_LIBS) ../../../../sdk/libcodeblocks.la
codesnippets_LDADD = $(PLUGIN_WX_LIBS) ../../../../sdk/libcodeblocks.la

codesnippets_SOURCES = 	../codesnippetsapp.cpp \
			../codesnippetstreectrl.cpp \
			../codesnippetswindow.cpp \
			../edit.cpp \
			../editsnippetframe.cpp \
			../messagebox.cpp \
			../messageBoxForm.cpp \
			../prefs.cpp \
			../settingsdlg.cpp \
			../settingsdlgform.cpp \
			../snippetitemdata.cpp \
			../snippetproperty.cpp \
			../snippetpropertyform.cpp \
			../snippetsconfig.cpp \
			../snippetsimages.cpp \
			../finddialogs.cpp \
			../memorymappedfile.cpp \
			../version.cpp

noinst_HEADERS =  	../codesnippetsapp.h \
			../codesnippetstreectrl.h \
			../codesnippetswindow.h \
			../defsext.h \
			../edit.h \
			../editsnippetframe.h \
			../menuidentifiers.h \
			../messagebox.h \
			../messageBoxForm.h \
			../prefs.h \
			../settingsdlg.h \
			../settingsdlgform.h \
			../snippetitemdata.h \
			../snippetproperty.h \
			../snippetpropertyform.h \
			../snippetsconfig.h \
			../snippetsimages.h \
			../finddialogs.h \
			../memorymappedfile.h \
			../version.h \
			../wx_pch.h

pkgdatadir = $(datadir)/@PACKAGE@/images/codesnippets

dist_pkgdata_DATA = 	allsnippets.png \
			category.png \
			snippet.png

EXTRA_DIST = manifest.xml

codesnippets.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codesnippets.zip manifest.xml > /dev/null

install-data-local: codesnippets.zip
	$(mkinstalldirs) $(DESTDIR)$(datadir)/@PACKAGE@/; \
	if test -f $(srcdir)/codesnippets.zip; then p=$(srcdir)/codesnippets.zip; else p=codesnippets.zip; fi; \
	$(INSTALL_DATA) $$p $(DESTDIR)$(datadir)/@PACKAGE@/codesnippets.zip

