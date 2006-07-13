gnomemenudir = $(datadir)/applications
gnomemenu_DATA = $(srcdir)/codeblocks.desktop

appdir = $(datadir)/application-registry
app_DATA = $(srcdir)/codeblocks.applications

mimedir = $(datadir)/mime-info
mime_DATA = $(srcdir)/codeblocks.keys $(srcdir)/codeblocks.mime

mimexmldir = $(datadir)/mime/packages
mimexml_DATA = codeblocks.xml

mimeicondir = $(datadir)/icons/gnome/48x48/mimetypes
mimeicon_DATA = $(srcdir)/gnome-mime-application-x-codeblocks.png \
                            $(srcdir)/gnome-mime-application-x-codeblocks-workspace.png

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = $(srcdir)/codeblocks.png

EXTRA_DIST = \
	$(app_DATA) \
	$(mime_DATA) \
	$(mimexml_DATA) \
	$(mimeicon_DATA) \
	$(gnomemenu_DATA) \
	$(pixmap_DATA)

## Update mime only when system wide installation takes place

install-data-local: install-mimexmlDATA
	if [ -f $(DESTDIR)$(datadir)/mime/packages/freedesktop.org.xml ] ; then \
	  if which update-mime-database>/dev/null 2>&1; then \
	    update-mime-database $(DESTDIR)$(datadir)/mime; \
	  fi; \
	  if which update-desktop-database>/dev/null 2>&1; then \
		update-desktop-database; \
	  fi \
	fi

uninstall-local:
	if [ -f $(DESTDIR)$(datadir)/mime/packages/freedesktop.org.xml ] ; then \
	  if which update-mime-database>/dev/null 2>&1; then \
	    update-mime-database $(DESTDIR)$(datadir)/mime; \
	  fi; \
	  if which update-desktop-database>/dev/null 2>&1; then \
		update-desktop-database; \
	  fi \
	fi
