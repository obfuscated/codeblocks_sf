EXTRA_DIST = configuration.xrc manifest.xml

pkgdata_DATA = Astyle.zip
CLEANFILES = $(pkgdata_DATA)

Astyle.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Astyle.zip manifest.xml *.xrc > /dev/null
