pkgdata_DATA = code_completion.zip
CLEANFILES = $(pkgdata_DATA)

code_completion.zip:
	cd $(top_srcdir)/src/plugins/codecompletion/resources && \
		zip code_completion.zip *.xrc > /dev/null
	mv $(top_srcdir)/src/plugins/codecompletion/resources/code_completion.zip .

install-exec-local:
	mkdir -p $(pkgdatadir)/images/codecompletion
	cp $(top_srcdir)/src/plugins/codecompletion/resources/images/*.png $(pkgdatadir)/images/codecompletion/

uninstall-local:
	rm -f $(pkgdatadir)/images/codecompletion/*.png
