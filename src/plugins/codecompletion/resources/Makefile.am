pkgdata_DATA = codecompletion.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = classbrowser.xrc \
	codecompletion_toolbar.xrc \
	insert_class_method.xrc \
	manifest.xml \
	project_settings.xrc \
	selectincludefile.xrc \
	settings.xrc \
	images/class_folder.png \
	images/class.png \
	images/ctor_private.png \
	images/ctor_protected.png \
	images/ctor_public.png \
	images/dtor_private.png \
	images/dtor_protected.png \
	images/dtor_public.png \
	images/enumerator.png \
	images/enum.png \
	images/enums_folder.png \
	images/method_private.png \
	images/method_protected.png \
	images/method_public.png \
	images/namespace.png \
	images/others_folder.png \
	images/preproc_folder.png \
	images/preproc.png \
	images/symbols_folder.png \
	images/typedef.png \
	images/typedefs_folder.png \
	images/var_private.png \
	images/var_protected.png \
	images/var_public.png

codecompletion.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codecompletion.zip manifest.xml *.xrc > /dev/null

install-exec-local:
	mkdir -p $(DESTDIR)$(pkgdatadir)/images/codecompletion
	$(INSTALL_DATA) $(srcdir)/images/*.png $(DESTDIR)$(pkgdatadir)/images/codecompletion/

uninstall-local:
	rm -f $(DESTDIR)$(pkgdatadir)/images/codecompletion/*.png

