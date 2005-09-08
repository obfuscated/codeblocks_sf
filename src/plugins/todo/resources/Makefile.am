pkgdata_DATA = todo.zip
CLEANFILES = $(pkgdata_DATA)

todo.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/todo.zip *.xrc > /dev/null

EXTRA_DIST = add_todo.xrc settings.xrc
