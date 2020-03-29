EXTRA_DIST = configuration.xrc main_dialog.xrc manifest.xml

pkgdata_DATA = Profiler.zip
CLEANFILES = $(pkgdata_DATA)

Profiler.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Profiler.zip manifest.xml *.xrc > /dev/null
