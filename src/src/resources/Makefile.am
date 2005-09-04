nobase_pkgdata_DATA = resources.zip \
				start_here.zip \
				$(top_srcdir)/src/src/resources/icons/app_64.xpm \
				$(top_srcdir)/src/src/resources/icons/app.xpm \
				$(top_srcdir)/src/src/resources/images/*.png \
				$(top_srcdir)/src/src/resources/images/16x16/*.png

CLEANFILES = $(nobase_pkgdata_DATA)

resources.zip:
	cd $(top_srcdir)/src/src/resources && \
		zip  resources.zip *.xrc images/*.png images/16x16/*.png > /dev/null
	mv $(top_srcdir)/src/src/resources/resources.zip .

start_here.zip:
	cd $(top_srcdir)/src/src/resources/start_here && \
		zip start_here.zip *.htm* *.jpg > /dev/null
	mv $(top_srcdir)/src/src/resources/start_here/start_here.zip .
