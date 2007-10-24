pkgdata_DATA = projectsimporter.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = project_import_menu.xrc manifest.xml

projectsimporter.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/projectsimporter.zip manifest.xml *.xrc > /dev/null

