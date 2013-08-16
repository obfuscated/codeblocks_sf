pkgdata_DATA = OccurrencesHighlighting.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = manifest.xml $(srcdir)/*.xrc

OccurrencesHighlighting.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/OccurrencesHighlighting.zip manifest.xml *.xrc > /dev/null

