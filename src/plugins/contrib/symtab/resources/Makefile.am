EXTRA_DIST = config_dialog.xrc exec_dialog.xrc manifest.xml

pkgdata_DATA = SymTab.zip
CLEANFILES = $(pkgdata_DATA)

SymTab.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/SymTab.zip manifest.xml *.xrc > /dev/null
