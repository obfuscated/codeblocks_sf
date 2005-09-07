pkgdata_DATA = defaultmimehandler.zip
CLEANFILES = $(pkgdata_DATA)

defaultmimehandler.zip:
	cd $(srcdir) && zip @builddir@/defaultmimehandler.zip *.xrc > /dev/null

EXTRA_DIST = edit_files_handling.xrc
