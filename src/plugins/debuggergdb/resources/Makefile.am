pkgdata_DATA = debugger_gdb.zip
CLEANFILES = $(pkgdata_DATA)

debugger_gdb.zip:
	cd $(top_srcdir)/src/plugins/debuggergdb/resources && \
		zip debugger_gdb.zip *.xrc \
		images/*.png \
		images/16x16/*.png > /dev/null
	mv $(top_srcdir)/src/plugins/debuggergdb/resources/debugger_gdb.zip .
