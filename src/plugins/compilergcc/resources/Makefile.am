pkgdata_DATA = compiler_gcc.zip
CLEANFILES = $(pkgdata_DATA)

compiler_gcc.zip:
	cd $(top_srcdir)/src/plugins/compilergcc/resources && \
		zip compiler_gcc.zip *.xrc \
		images/*.png \
		images/16x16/*.png > /dev/null
	mv $(top_srcdir)/src/plugins/compilergcc/resources/compiler_gcc.zip .
