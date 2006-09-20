pkgdata_DATA = todo.zip
CLEANFILES = $(pkgdata_DATA)

todo.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/todo.zip manifest.xml *.xrc > /dev/null

EXTRA_DIST = add_todo.xrc manifest.xml settings.xrc
