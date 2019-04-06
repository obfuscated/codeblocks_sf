pkgdata_DATA = compiler.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = advanced_compiler_options.xrc \
	compiler_flag.xrc \
	compiler_menu.xrc \
	compiler_options.xrc \
	compiler_toolbar.xrc \
	manifest.xml \
	images/32x32/compile.png \
	images/32x32/compilerun.png \
	images/32x32/rebuild.png \
	images/32x32/run.png \
	images/32x32/select_target.png \
	images/32x32/stop.png \
	images/22x22/compile.png \
	images/22x22/compilerun.png \
	images/22x22/rebuild.png \
	images/22x22/run.png \
	images/22x22/select_target.png \
	images/22x22/stop.png \
	images/16x16/compile.png \
	images/16x16/compilerun.png \
	images/16x16/rebuild.png \
	images/16x16/run.png \
	images/16x16/stop.png \
	images/16x16/select_target.png

compiler.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/compiler.zip $(EXTRA_DIST) > /dev/null

compilers_xmldir = $(datadir)/@PACKAGE@/compilers

dist_compilers_xml_DATA = $(srcdir)/compilers/*.xml
