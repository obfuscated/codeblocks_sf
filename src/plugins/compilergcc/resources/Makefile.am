pkgdata_DATA = compiler_gcc.zip
CLEANFILES = $(pkgdata_DATA)

compiler_gcc.zip:
	cd $(srcdir) && \
		zip @builddir@/compiler_gcc.zip *.xrc images/*.png images/16x16/*.png > /dev/null
