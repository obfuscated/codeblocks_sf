pkgdata_DATA = plugin_wizard.zip
CLEANFILES = $(pkgdata_DATA)

plugin_wizard.zip:
	cd $(top_srcdir)/src/plugins/pluginwizard/resources && \
		zip plugin_wizard.zip *.xrc > /dev/null
	mv $(top_srcdir)/src/plugins/pluginwizard/resources/plugin_wizard.zip .
