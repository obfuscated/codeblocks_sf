SUBDIRS = lexers

dist_noinst_DATA = $(srcdir)/*.xrc \
			$(srcdir)/images/*.png

pkgdata_DATA = manager_resources.zip
CLEANFILES = $(pkgdata_DATA)

manager_resources.zip:
	cd $(srcdir) && zip -j @builddir@/manager_resources.zip *.xrc images/*.png > /dev/null
