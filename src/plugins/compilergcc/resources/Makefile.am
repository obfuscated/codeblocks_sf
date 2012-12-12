pkgdata_DATA = compiler.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = advanced_compiler_options.xrc \
	compiler_flag.xrc \
	compiler_menu.xrc \
	compiler_options.xrc \
	compiler_toolbar_16x16.xrc \
	compiler_toolbar.xrc \
	manifest.xml \
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

compiler.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/compiler.zip manifest.xml *.xrc images/*.png images/16x16/*.png > /dev/null

compilers_xmldir = $(datadir)/@PACKAGE@/compilers

dist_compilers_xml_DATA = $(srcdir)/compilers/*.xml
