EXTRA_DIST = manifest.xml

pkgdata_DATA = Cccc.zip
CLEANFILES = $(pkgdata_DATA)

Cccc.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Cccc.zip manifest.xml > /dev/null
