EXTRA_DIST = configuration.xrc main_dialog.xrc manifest.xml

pkgdata_DATA = CBProfiler.zip
CLEANFILES = $(pkgdata_DATA)

CBProfiler.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/CBProfiler.zip manifest.xml *.xrc > /dev/null
