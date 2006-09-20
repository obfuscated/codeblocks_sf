EXTRA_DIST = configuration.xrc manifest.xml

pkgdata_DATA = astyle.zip
CLEANFILES = $(pkgdata_DATA)

astyle.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/astyle.zip manifest.xml *.xrc > /dev/null
