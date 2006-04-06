# Makefile for the Code::Blocks docguide documentations; produces HTML files from XML files

# Currently the xsltproc program and the docbook xsl files are required
#for producing HTML files

# Tools
DOCBOOK_DIR     = /usr/local/share/xsl/docbook
HTML_DIR_XSL    = $(DOCBOOK_DIR)/html/chunk.xsl
HTML_FILE_XSL   = $(DOCBOOK_DIR)/html/docbook.xsl
XML_FILES       = $(wildcard *.xml)
MAKEFILE        = Makefile

# Output files
BASENAME        = docguide
HTML_DIR        = html.d
HTML_FILE       = $(BASENAME).html
CSS_FILE        = codeblocks.css

default:	html_dir

all:	html_dir html_file

# Produce a directory with a set of HTML files, one per chapter or section
html_dir:	$(XML_FILES) $(MAKEFILE)
	@echo "Creating HTML directory"
	[ -d $(HTML_DIR) ] || mkdir $(HTML_DIR)
	xsltproc \
		--stringparam section.autolabel 1 \
		--stringparam section.label.includes.component.label 1 \
		--stringparam toc.max.depth 2 \
		--stringparam html.stylesheet $(CSS_FILE) \
		--stringparam chunker.output.indent yes \
		--stringparam base.dir $(HTML_DIR)/ \
		$(HTML_DIR_XSL) \
		book.xml

# Produce a single HTML file with everything in it
html_file:	$(XML_FILES) $(MAKEFILE)
	@echo Creating HTML file
	xsltproc \
		--stringparam section.autolabel 1 \
		--stringparam section.label.includes.component.label 1 \
		--stringparam toc.max.depth 2 \
		--stringparam html.stylesheet $(CSS_FILE) \
		-o $(HTML_FILE) \
		$(HTML_FILE_XSL) \
		book.xml

# Remove all generated files
clean:  clean_html_dir clean_html_file

# Remove the HTML directory
clean_html_dir:;	rm -rf $(HTML_DIR)

# Remove the all-in-one HTML output file
clean_html_file:;	rm -f $(HTML_FILE)


