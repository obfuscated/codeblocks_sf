SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/scripting/include \
			-I$(top_srcdir)/src/include/scripting/sqplus \
			-I$(top_srcdir)/src/include/mozilla_chardet

if CODEBLOCKS_LINUX
AM_CPPFLAGS = -DENABLE_BINRELOC -DAPP_PREFIX="\"@prefix@\"" $(WX_GTK2_CFLAGS)
endif

bin_PROGRAMS = codeblocks

# wxMac resource fork/unbundled app
install-exec-hook:
	$(POSTLINK_COMMAND) $(DESTDIR)$(bindir)/codeblocks$(EXEEXT) \
	                    $(srcdir)/resources/icons/app.r
	$(MACSETFILE) -a C $(DESTDIR)$(bindir)/codeblocks$(EXEEXT)

# wxAUI uses GTK+ libraries on wxGTK
# codeblocks_LDFLAGS = $(WX_GTK2_LIBS)

codeblocks_LDADD =  -L../sdk -lcodeblocks \
			$(WX_LIBS) \
			$(WX_GTK2_LIBS)

codeblocks_SOURCES = app.cpp \
			appglobals.cpp \
			compilersettingsdlg.cpp \
			crashhandler.cpp \
			dlgabout.cpp \
			dlgaboutplugin.cpp \
			environmentsettingsdlg.cpp \
			infopane.cpp \
			main.cpp \
			notebookstyles.cpp \
			prefix.cpp \
			printdlg.cpp \
			scriptconsole.cpp \
			scriptingsettingsdlg.cpp \
			splashscreen.cpp \
			startherepage.cpp \
			switcherdlg.cpp

noinst_HEADERS = app.h \
			appglobals.h \
			batchbuild.h \
			compilersettingsdlg.h \
			crashhandler.h \
			dlgabout.h \
			dlgaboutplugin.h \
			environmentsettingsdlg.h \
			infopane.h \
			main.h \
			notebookstyles.h \
			prefix.h \
			printdlg.h \
			scriptconsole.h \
			scriptingsettingsdlg.h \
			splashscreen.h \
			startherepage.h \
			switcherdlg.h

man_MANS = codeblocks.1

EXTRA_DIST = associations.cpp \
			associations.h \
			ipc.h \
			ipc.cpp \
			$(man_MANS)
