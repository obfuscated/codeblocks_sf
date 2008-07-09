EXTRA_DIST = manifest.xml

pkgdata_DATA = Valgrind.zip
CLEANFILES = $(pkgdata_DATA)

Valgrind.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Valgrind.zip manifest.xml *.xrc > /dev/null
