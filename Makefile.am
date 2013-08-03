SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

if KEEP_DLLS
ADDITIONAL_MAKE_DIST_OPTIONS = --exclude=".svn"
else
ADDITIONAL_MAKE_DIST_OPTIONS = --exclude=".svn" --exclude="*.dll"
endif

am__tar = @am__tar@ $(ADDITIONAL_MAKE_DIST_OPTIONS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = codeblocks.pc

EXTRA_DIST = bootstrap \
		codeblocks.spec \
		codeblocks.plist \
		BUGS \
		BUILD \
		COMPILERS \
		$(srcdir)/update* \
		ChangeLog.xsl \
		debian

clean-zipfiles:
	find . -name "*.zip" | xargs rm -f

clean-bin:
	find . -name "*.la" | xargs rm -f
	rm -f src/src/codeblocks
