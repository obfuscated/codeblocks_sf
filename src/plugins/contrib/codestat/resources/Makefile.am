EXTRA_DIST = config_dialog.xrc main_dialog.xrc

pkgdata_DATA = codestat.zip
CLEANFILES = $(pkgdata_DATA)

codestat.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codestat.zip *.xrc > /dev/null
