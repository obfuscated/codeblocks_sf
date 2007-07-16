SUBDIRS = lexers

dist_noinst_DATA = $(srcdir)/*.xrc \
			$(srcdir)/images/*.png

pkgdata_DATA = manager_resources.zip
CLEANFILES = $(pkgdata_DATA)

manager_resources.zip: $(dist_noinst_DATA)
	PWD=`pwd` cd $(srcdir) && zip -j $(PWD)/manager_resources.zip *.xrc images/*.png > /dev/null
