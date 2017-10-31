SUBDIRS = arduino \
	arm \
	avr \
	c_file \
	console \
	d \
	directx \
	dll \
	d_source \
	empty \
	empty_file \
	fltk  \
	fortran \
	glfw \
	glut \
	gtk \
	h_file \
	irrlicht \
	java \
	lf \
	matlab_csf \
	mcs51 \
	msp430 \
	ogre \
	opencv \
	opengl \
	plugins \
	ppc \
	qt4 \
	qt4dll \
	qt5 \
	sdl \
	sdl2 \
	sfml \
	smartwin \
	staticlib \
	stlport \
	sharedlib \
	sys \
	tricore \
	win32gui \
	wxwidgets

pkgdata_DATA = scriptedwizard.zip
CLEANFILES = $(pkgdata_DATA)

scriptdir = $(pkgdatadir)/templates/wizard

dist_script_DATA = common_functions.script \
		config.script

EXTRA_DIST = manifest.xml

scriptedwizard.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/scriptedwizard.zip manifest.xml > /dev/null

