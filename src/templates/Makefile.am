if LINUX
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

