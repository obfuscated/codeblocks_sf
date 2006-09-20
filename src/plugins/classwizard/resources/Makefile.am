EXTRA_DIST = manifest.xml new_class.xrc

pkgdata_DATA = class_wizard.zip
CLEANFILES = $(pkgdata_DATA)

class_wizard.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/class_wizard.zip manifest.xml *.xrc > /dev/null
