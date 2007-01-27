SUBDIRS = src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = codeblocks.pc

EXTRA_DIST = $(top_srcdir)/bootstrap \
		$(top_srcdir)/codeblocks.spec \
		$(top_srcdir)/codeblocks.plist \
		$(top_srcdir)/BUGS \
		$(top_srcdir)/BUILD \
		$(top_srcdir)/COMPILERS

clean-zipfiles:
	find . -name "*.zip" | xargs rm -f

clean-bin:
	find . -name "*.la" | xargs rm -f
	rm -f src/src/codeblocks
