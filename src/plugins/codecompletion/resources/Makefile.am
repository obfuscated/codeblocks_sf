pkgdata_DATA = code_completion.zip
CLEANFILES = $(pkgdata_DATA)

code_completion.zip:
	cd $(srcdir) && zip @builddir@/code_completion.zip *.xrc > /dev/null

install-exec-local:
	mkdir -p $(pkgdatadir)/images/codecompletion
	$(INSTALL_PROGRAM) $(srcdir)/images/*.png $(pkgdatadir)/images/codecompletion/

uninstall-local:
	rm -f $(pkgdatadir)/images/codecompletion/*.png
