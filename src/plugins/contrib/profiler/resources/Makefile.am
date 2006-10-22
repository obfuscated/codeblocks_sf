EXTRA_DIST = configuration.xrc main_dialog.xrc manifest.xml

pkgdata_DATA = profiler.zip
CLEANFILES = $(pkgdata_DATA)

profiler.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/profiler.zip manifest.xml *.xrc > /dev/null
