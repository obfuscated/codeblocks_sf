pkgdata_DATA = plugin_wizard.zip
CLEANFILES = $(pkgdata_DATA)

plugin_wizard.zip:
	cd $(srcdir) && zip @builddir@/plugin_wizard.zip *.xrc > /dev/null

EXTRA_DIST = new_plugin.xrc plugin_info.xrc
