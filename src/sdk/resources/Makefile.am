noinst_DATA = $(srcdir)/*.xrc \
			$(srcdir)/lexers/lexer_* \
			$(srcdir)/images/*.png

nobase_pkgdata_DATA = manager_resources.zip $(srcdir)/lexers/lexer_*
CLEANFILES = $(nobase_pkgdata_DATA)

manager_resources.zip:
	cd $(srcdir) && zip -j @builddir@/manager_resources.zip *.xrc images/*.png > /dev/null
