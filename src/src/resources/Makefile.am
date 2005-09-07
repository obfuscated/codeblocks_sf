SUBDIRS = icons images

dist_noinst_DATA = $(srcdir)/*.xrc \
				$(srcdir)/*.rc \
				$(srcdir)/start_here/*.jpg \
				$(srcdir)/start_here/*.htm*

dist_pkgdata_DATA = resources.zip start_here.zip
CLEANFILES = $(pkgdata_DATA)

resources.zip:
	cd $(srcdir) && zip @builddir@/resources.zip *.xrc images/*.png images/16x16/*.png > /dev/null

start_here.zip:
	cd $(srcdir)/start_here && zip @builddir@/start_here.zip *.htm* *.jpg > /dev/null
