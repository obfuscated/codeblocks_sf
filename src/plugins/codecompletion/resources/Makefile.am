pkgdata_DATA = code_completion.zip
CLEANFILES = $(pkgdata_DATA)

code_completion.zip:
	zip code_completion.zip *.xrc > /dev/null

install-exec-local:
	mkdir -p $(pkgdatadir)/images/codecompletion
	cp images/*.png $(pkgdatadir)/images/codecompletion/

uninstall-local:
	rm -f $(pkgdatadir)/images/codecompletion/*.png
