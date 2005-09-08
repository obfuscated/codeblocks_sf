EXTRA_DIST = configuration.xrc

pkgdata_DATA = astyle.zip
CLEANFILES = $(pkgdata_DATA)

astyle.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/astyle.zip *.xrc > /dev/null
