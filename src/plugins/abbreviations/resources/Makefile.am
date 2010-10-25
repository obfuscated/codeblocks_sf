pkgdata_DATA = abbreviations.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = manifest.xml $(srcdir)/*.xrc

abbreviations.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/abbreviations.zip manifest.xml *.xrc > /dev/null

