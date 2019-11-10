pkgdata_DATA = occurrenceshighlighting.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = manifest.xml $(srcdir)/*.xrc

occurrenceshighlighting.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/occurrenceshighlighting.zip manifest.xml *.xrc > /dev/null
