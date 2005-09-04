pkgdata_DATA = astyle.zip
CLEANFILES = $(pkgdata_DATA)

astyle.zip:
	cd $(top_srcdir)/src/plugins/astyle/resources && \
		zip astyle.zip *.xrc > /dev/null
	mv $(top_srcdir)/src/plugins/astyle/resources/astyle.zip .
