pkgdata_DATA = projectsimporter.zip
CLEANFILES = $(pkgdata_DATA)

projectsimporter.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/projectsimporter.zip manifest.xml > /dev/null

EXTRA_DIST = manifest.xml
