SUBDIRS = arm avr c_file console d directx dll empty empty_file fltk fortran glfw glut gtk h_file irrlicht lf mcs51 matlab_csf ogre opencv opengl plugins ppc qt4 sdl sfml smartwin staticlib stlport sharedlib sys tricore win32gui wxwidgets

pkgdata_DATA = scriptedwizard.zip
CLEANFILES = $(pkgdata_DATA)

scriptdir = $(pkgdatadir)/templates/wizard

dist_script_DATA = common_functions.script \
					config.script

EXTRA_DIST = manifest.xml

scriptedwizard.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/scriptedwizard.zip manifest.xml > /dev/null

