pkgdata_DATA = plugin_wizard.zip
CLEANFILES = $(pkgdata_DATA)

plugin_wizard.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/plugin_wizard.zip *.xrc > /dev/null

EXTRA_DIST = new_plugin.xrc plugin_info.xrc
