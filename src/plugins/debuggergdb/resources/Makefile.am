pkgdata_DATA = debugger_gdb.zip
CLEANFILES = $(pkgdata_DATA)

DEBUG_RESOURCES = backtrace.xrc \
    breakpoints.xrc \
	debugger_menu.xrc \
	debugger_options_dlg.xrc \
	debugger_toolbar_16x16.xrc \
	debugger_toolbar.xrc \
	disassembly.xrc \
	memdump.xrc \
	threads.xrc \
	images/dbgnext.png \
	images/dbgrun.png \
	images/dbgrunto.png \
	images/dbgstepi.png \
	images/dbgstepout.png \
	images/dbgstep.png \
	images/stop.png \
	images/16x16/dbgnext.png \
	images/16x16/dbgrun.png \
	images/16x16/dbgrunto.png \
	images/16x16/dbgstepi.png \
	images/16x16/dbgstepout.png \
	images/16x16/dbgstep.png \
	images/16x16/stop.png

EXTRA_DIST = $(DEBUG_RESOURCES)

debugger_gdb.zip: $(DEBUG_RESOURCES)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/debugger_gdb.zip *.xrc images/*.png images/16x16/*.png > /dev/null
