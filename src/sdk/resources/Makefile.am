nobase_pkgdata_DATA = manager_resources.zip $(srcdir)/lexers/lexer_*
CLEANFILES = $(nobase_pkgdata_DATA)

manager_resources.zip:
	cd $(srcdir) && \
		zip -j manager_resources.zip *.xrc images/*.png > /dev/null
	mv $(srcdir)/manager_resources.zip .
