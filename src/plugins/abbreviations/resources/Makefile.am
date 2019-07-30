pkgdata_DATA = abbreviations.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = \
	abbreviationsconfigpanel.xrc \
	images/16x16/arrow.png \
	images/20x20/arrow.png \
	images/24x24/arrow.png \
	images/28x28/arrow.png \
	images/32x32/arrow.png \
	images/40x40/arrow.png \
	images/48x48/arrow.png \
	images/56x56/arrow.png \
	images/64x64/arrow.png \
	manifest.xml

abbreviations.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/abbreviations.zip $(EXTRA_DIST) > /dev/null

