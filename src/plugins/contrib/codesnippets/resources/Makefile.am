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
