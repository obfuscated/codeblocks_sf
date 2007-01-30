SUBDIRS = c_file console d directx dll empty empty_file fltk glfw glut gtk h_file irrlicht lf ogre opengl plugins qt4 sdl smartwin staticlib win32gui wxwidgets

pkgdatadir = $(datadir)/@PACKAGE@/templates/wizard

dist_pkgdata_DATA = common_functions.script \
					config.script

EXTRA_DIST = manifest.xml

scriptedwizard.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/scriptedwizard.zip manifest.xml > /dev/null

install-data-local: scriptedwizard.zip
	$(mkinstalldirs) $(DESTDIR)$(datadir)/@PACKAGE@/; \
	if test -f $(srcdir)/scriptedwizard.zip; then p=$(srcdir)/scriptedwizard.zip; else p=scriptedwizard.zip; fi; \
	$(INSTALL_DATA) $$p $(DESTDIR)$(datadir)/@PACKAGE@/scriptedwizard.zip
