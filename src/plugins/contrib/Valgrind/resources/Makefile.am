EXTRA_DIST = manifest.xml \
		$(srcdir)/images/*.png

pkgdata_DATA = Valgrind.zip
CLEANFILES = $(pkgdata_DATA)

Valgrind.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Valgrind.zip manifest.xml > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings

dist_confimages_DATA = $(srcdir)/images/*.png
