noinst_DATA = $(srcdir)/common/*.template \
			$(srcdir)/common/*.c* \
			$(srcdir)/common/*.h* \
			$(srcdir)/common/*.png \
			$(srcdir)/common/*.bmp \
			$(srcdir)/unix/*.template \
			$(srcdir)/unix/*.c* \
			$(srcdir)/unix/*.h* \
			$(srcdir)/unix/*.png \
			$(srcdir)/unix/*.bmp \
			$(srcdir)/win32/*.template \
			$(srcdir)/win32/*.c* \
			$(srcdir)/win32/*.h* \
			$(srcdir)/win32/*.png \
			$(srcdir)/win32/*.bmp


if CODEBLOCKS_LINUX
PLATFORM=unix
else
PLATFORM=win32
endif

install-exec-local:
	mkdir -p $(pkgdatadir)/templates
	cp `find $(srcdir)/common -maxdepth 1 ! -type d` $(pkgdatadir)/templates
	cp `find $(srcdir)/$(PLATFORM) -maxdepth 1 ! -type d` $(pkgdatadir)/templates

uninstall-local:
	rm -f $(pkgdatadir)/templates/*

