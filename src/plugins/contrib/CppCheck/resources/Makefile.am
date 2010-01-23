EXTRA_DIST = manifest.xml

pkgdata_DATA = CppCheck.zip
CLEANFILES = $(pkgdata_DATA)

CppCheck.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/CppCheck.zip manifest.xml > /dev/null
