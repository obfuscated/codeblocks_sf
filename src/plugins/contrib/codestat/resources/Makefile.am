EXTRA_DIST = config_dialog.xrc main_dialog.xrc manifest.xml

pkgdata_DATA = codestat.zip
CLEANFILES = $(pkgdata_DATA)

codestat.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codestat.zip manifest.xml *.xrc > /dev/null
