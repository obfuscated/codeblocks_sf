noinst_DATA = $(srcdir)/*.xrc \
				$(srcdir)/*.rc \
				$(srcdir)/icons/*.xpm \
				$(srcdir)/icons/*.ico \
				$(srcdir)/images/*.png \
				$(srcdir)/images/16x16/*.png \
				$(srcdir)/start_here/*.jpg \
				$(srcdir)/start_here/*.htm*

nobase_pkgdata_DATA = resources.zip \
				start_here.zip \
				$(top_srcdir)/src/src/resources/icons/app_64.xpm \
				$(top_srcdir)/src/src/resources/icons/app.xpm \
				$(top_srcdir)/src/src/resources/images/*.png \
				$(top_srcdir)/src/src/resources/images/16x16/*.png

CLEANFILES = $(nobase_pkgdata_DATA)

resources.zip:
	cd $(srcdir) && zip @builddir@/resources.zip *.xrc images/*.png images/16x16/*.png > /dev/null
	mv $(top_srcdir)/src/src/resources/resources.zip .

start_here.zip:
	cd $(srcdir)/start_here && zip @builddir@/start_here.zip *.htm* *.jpg > /dev/null
