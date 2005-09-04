pkgdata_DATA = todo.zip
CLEANFILES = $(pkgdata_DATA)

todo.zip:
	cd $(top_srcdir)/src/plugins/todo/resources && \
		zip todo.zip *.xrc > /dev/null
	mv $(top_srcdir)/src/plugins/todo/resources/todo.zip .
