if ! HAVE_WX29
TREELIST_DIR=wxTreeList
TREELIST_PC=cb_wxtreelist.pc
endif

SUBDIRS = wxchart wxFlatNotebook wxthings wxImagePanel wxSpeedButton KWIC $(TREELIST_DIR) wxled .

EXTRA_DIST = $(srcdir)/*.cbp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wxsmith-contrib.pc cb_wxcontrib.pc cb_wxchartctrl.pc cb_wxcustombutton.pc cb_wxflatnotebook.pc cb_wximagepanel.pc cb_wxspeedbutton.pc cb_wxKWIC.pc $(TREELIST_PC) cb_wxled.pc
