SUBDIRS = images

pkgdata_DATA = debugger.zip
CLEANFILES = $(pkgdata_DATA)

DEBUG_RESOURCES = backtrace.xrc \
	breakpoints.xrc \
	debugger_menu.xrc \
	debugger_options_dlg.xrc \
	debugger_project_options_dlg.xrc \
	debugger_toolbar_16x16.xrc \
	debugger_toolbar.xrc \
	disassembly.xrc \
	edit_watches_dlg.xrc \
	manifest.xml \
	memdump.xrc \
	threads.xrc \
	images/dbginfo.png \
	images/dbgnext.png \
	images/dbgrun.png \
	images/dbgrunto.png \
	images/dbgstepi.png \
	images/dbgstepout.png \
	images/dbgstep.png \
	images/dbgstop.png \
	images/window.png \
	images/16x16/dbginfo.png \
	images/16x16/dbgnext.png \
	images/16x16/dbgrun.png \
	images/16x16/dbgrunto.png \
	images/16x16/dbgstepi.png \
	images/16x16/dbgstepout.png \
	images/16x16/dbgstep.png \
	images/16x16/dbgstop.png \
	images/16x16/window.png

EXTRA_DIST = $(DEBUG_RESOURCES)

debugger.zip: $(DEBUG_RESOURCES)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/debugger.zip manifest.xml *.xrc images/*.png images/16x16/*.png > /dev/null
