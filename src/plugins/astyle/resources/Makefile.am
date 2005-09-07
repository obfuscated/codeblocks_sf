EXTRA_DIST = configuration.xrc

pkgdata_DATA = astyle.zip
CLEANFILES = $(pkgdata_DATA)

astyle.zip:
	cd $(srcdir) && zip @builddir@/astyle.zip *.xrc > /dev/null
