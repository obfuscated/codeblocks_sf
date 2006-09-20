SUBDIRS = c_file console d directx dll empty empty_file fltk glfw glut h_file irrlicht ogre opengl plugins qt4 sdl smartwin staticlib win32gui wxwidgets

pkgdatadir = $(datadir)/@PACKAGE@/templates/wizard

dist_pkgdata_DATA = common_functions.script \
					config.script

EXTRA_DIST = manifest.xml
pkgdata_DATA = scriptedwizard.zip
CLEANFILES = $(pkgdata_DATA)

scriptedwizard.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/scriptedwizard.zip manifest.xml > /dev/null
