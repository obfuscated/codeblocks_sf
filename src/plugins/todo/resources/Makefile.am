pkgdata_DATA = todo.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = manifest.xml *.xrc

todo.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/todo.zip manifest.xml *.xrc > /dev/null

