pkgdata_DATA = defaultmimehandler.zip
CLEANFILES = $(pkgdata_DATA)

defaultmimehandler.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/defaultmimehandler.zip *.xrc > /dev/null

EXTRA_DIST = edit_files_handling.xrc
