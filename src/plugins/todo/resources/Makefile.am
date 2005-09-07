pkgdata_DATA = todo.zip
CLEANFILES = $(pkgdata_DATA)

todo.zip:
	cd $(srcdir) && zip @builddir@/todo.zip *.xrc > /dev/null

EXTRA_DIST = add_todo.xrc settings.xrc
