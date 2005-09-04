pkgdata_DATA = class_wizard.zip
CLEANFILES = $(pkgdata_DATA)

class_wizard.zip:
	cd $(top_srcdir)/src/plugins/classwizard/resources && \
		zip class_wizard.zip *.xrc > /dev/null
	mv $(top_srcdir)/src/plugins/classwizard/resources/class_wizard.zip .
