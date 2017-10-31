pkgdata_DATA = debugger.zip
CLEANFILES = $(pkgdata_DATA)

DEBUG_RESOURCES = breakpoints.xrc \
	debugger_options_dlg.xrc \
	debugger_project_options_dlg.xrc \
	edit_watches_dlg.xrc \
	manifest.xml

EXTRA_DIST = $(DEBUG_RESOURCES)

debugger.zip: $(DEBUG_RESOURCES)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/debugger.zip manifest.xml *.xrc > /dev/null
