pkgdata_DATA = defaultmimehandler.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = edit_files_handling.xrc manifest.xml

defaultmimehandler.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/defaultmimehandler.zip manifest.xml *.xrc > /dev/null

