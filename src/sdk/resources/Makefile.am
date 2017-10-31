SUBDIRS = lexers

dist_noinst_DATA = $(srcdir)/*.xrc \
			$(srcdir)/images/*.png \
			$(srcdir)/images/16x16/*.png \
			$(srcdir)/images/12x12/*.png 

pkgdata_DATA = manager_resources.zip
CLEANFILES = $(pkgdata_DATA)

manager_resources.zip: $(dist_noinst_DATA)
	PWD=`pwd` cd $(srcdir) && zip -qu $(PWD)/manager_resources.zip *.xrc images/*.png images/16x16/*.png images/12x12/*.png > /dev/null
