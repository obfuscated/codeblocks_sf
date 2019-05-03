pkgdata_DATA = compiler.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = advanced_compiler_options.xrc \
	compiler_flag.xrc \
	compiler_menu.xrc \
	compiler_options.xrc \
	compiler_toolbar.xrc \
	manifest.xml \
	images/16x16/compile.png \
	images/16x16/compilerun.png \
	images/16x16/rebuild.png \
	images/16x16/run.png \
	images/16x16/select_target.png \
	images/16x16/stop.png \
	images/20x20/compile.png \
	images/20x20/compilerun.png \
	images/20x20/rebuild.png \
	images/20x20/run.png \
	images/20x20/select_target.png \
	images/20x20/stop.png \
	images/24x24/compile.png \
	images/24x24/compilerun.png \
	images/24x24/rebuild.png \
	images/24x24/run.png \
	images/24x24/select_target.png \
	images/24x24/stop.png \
	images/28x28/compile.png \
	images/28x28/compilerun.png \
	images/28x28/rebuild.png \
	images/28x28/run.png \
	images/28x28/select_target.png \
	images/28x28/stop.png \
	images/32x32/compile.png \
	images/32x32/compilerun.png \
	images/32x32/rebuild.png \
	images/32x32/run.png \
	images/32x32/select_target.png \
	images/32x32/stop.png \
	images/40x40/compile.png \
	images/40x40/compilerun.png \
	images/40x40/rebuild.png \
	images/40x40/run.png \
	images/40x40/select_target.png \
	images/40x40/stop.png \
	images/48x48/compile.png \
	images/48x48/compilerun.png \
	images/48x48/rebuild.png \
	images/48x48/run.png \
	images/48x48/select_target.png \
	images/48x48/stop.png \
	images/56x56/compile.png \
	images/56x56/compilerun.png \
	images/56x56/rebuild.png \
	images/56x56/run.png \
	images/56x56/select_target.png \
	images/56x56/stop.png \
	images/64x64/compile.png \
	images/64x64/compilerun.png \
	images/64x64/rebuild.png \
	images/64x64/run.png \
	images/64x64/select_target.png \
	images/64x64/stop.png

compiler.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/compiler.zip $(EXTRA_DIST) > /dev/null

compilers_xmldir = $(datadir)/@PACKAGE@/compilers

dist_compilers_xml_DATA = $(srcdir)/compilers/*.xml
