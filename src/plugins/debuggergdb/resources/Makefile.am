pkgdata_DATA = debugger_gdb.zip
CLEANFILES = $(pkgdata_DATA)

debugger_gdb.zip:
	cd $(srcdir) && \
		zip @builddir@/debugger_gdb.zip *.xrc images/*.png images/16x16/*.png > /dev/null
