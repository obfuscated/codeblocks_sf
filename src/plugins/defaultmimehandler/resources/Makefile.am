pkgdata_DATA = defaultmimehandler.zip
CLEANFILES = $(pkgdata_DATA)

defaultmimehandler.zip:
	cd $(top_srcdir)/src/plugins/defaultmimehandler/resources && \
		zip defaultmimehandler.zip *.xrc > /dev/null
	mv $(top_srcdir)/src/plugins/defaultmimehandler/resources/defaultmimehandler.zip .
