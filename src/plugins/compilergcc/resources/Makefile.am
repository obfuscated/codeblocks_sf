pkgdata_DATA = compiler_gcc.zip
CLEANFILES = $(pkgdata_DATA)

compiler_gcc.zip:
	cd $(srcdir) && \
		zip @builddir@/compiler_gcc.zip *.xrc images/*.png images/16x16/*.png > /dev/null

EXTRA_DIST = advanced_compiler_options.xrc \
	compiler_menu.xrc \
	compiler_options.xrc \
	compiler_toolbar_16x16.xrc \
	compiler_toolbar.xrc \
	images/compile.png \
	images/compilerun.png \
	images/rebuild.png \
	images/run.png \
	images/stop.png \
	images/16x16/compile.png \
	images/16x16/compilerun.png \
	images/16x16/rebuild.png \
	images/16x16/run.png \
	images/16x16/stop.png
