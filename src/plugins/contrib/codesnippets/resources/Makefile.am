pkgdatadir = $(datadir)/@PACKAGE@/images/codesnippets

dist_pkgdata_DATA = 	allsnippets.png \
			category.png \
			snippet.png

EXTRA_DIST = manifest.xml

pkgdata_DATA = codesnippets.zip
CLEANFILES = $(pkgdata_DATA)

codesnippets.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codesnippets.zip manifest.xml > /dev/null
