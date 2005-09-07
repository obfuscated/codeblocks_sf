EXTRA_DIST = new_class.xrc

pkgdata_DATA = class_wizard.zip
CLEANFILES = $(pkgdata_DATA)

class_wizard.zip:
	cd $(srcdir) && zip @builddir@/class_wizard.zip *.xrc > /dev/null
