pkgdatadir = $(datadir)/@PACKAGE@/images/codesnippets

dist_pkgdata_DATA = 	allsnippets.png \
			category.png \
			snippet.png

EXTRA_DIST = manifest.xml

pkgdata_DATA = codesnippets.zip
CLEANFILES = $(pkgdata_DATA)

codesnippets.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codesnippets.zip manifest.xml > /dev/null

install-data-local: codesnippets.zip
	if test -f $(srcdir)/codesnippets.zip; then p=$(srcdir)/codesnippets.zip; else p=codesnippets.zip; fi; \
	$(INSTALL_DATA) $$p $(datadir)/@PACKAGE@/codesnippets.zip
