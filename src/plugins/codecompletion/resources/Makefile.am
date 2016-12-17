pkgdata_DATA = codecompletion.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = classbrowser.xrc \
             codecompletion_toolbar.xrc \
             goto_function_dlg.xrc \
             insert_class_method.xrc \
             manifest.xml \
             project_settings.xrc \
             selectincludefile.xrc \
             settings.xrc \
             images/class_folder.png \
             images/class.png \
             images/class_private.png \
             images/class_protected.png \
             images/class_public.png \
             images/ctor_private.png \
             images/ctor_protected.png \
             images/ctor_public.png \
             images/dtor_private.png \
             images/dtor_protected.png \
             images/dtor_public.png \
             images/enumerator.png \
             images/enum.png \
             images/enum_private.png \
             images/enum_protected.png \
             images/enum_public.png \
             images/enums_folder.png \
             images/folder_blue_open.png \
             images/folder_blue.png \
             images/folder_green_open.png \
             images/folder_green.png \
             images/folder_open.png \
             images/folder.png \
             images/funcs_folder.png \
             images/macro_def.png \
             images/macro_def_folder.png \
             images/macro_use.png \
             images/macro_use_folder.png \
             images/macro_use_private.png \
             images/macro_use_protected.png \
             images/macro_use_public.png \
             images/method_private.png \
             images/method_protected.png \
             images/method_public.png \
             images/namespace.png \
             images/others_folder.png \
             images/symbols_folder.png \
             images/typedef.png \
             images/typedef_private.png \
             images/typedef_protected.png \
             images/typedef_public.png \
             images/typedefs_folder.png \
             images/var_private.png \
             images/var_protected.png \
             images/var_public.png \
             images/vars_folder.png

codecompletion.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codecompletion.zip manifest.xml *.xrc > /dev/null

install-exec-local:
	mkdir -p $(DESTDIR)$(pkgdatadir)/images/codecompletion
	$(INSTALL_DATA) $(srcdir)/images/*.png $(DESTDIR)$(pkgdatadir)/images/codecompletion/

uninstall-local:
	rm -f $(DESTDIR)$(pkgdatadir)/images/codecompletion/*.png

