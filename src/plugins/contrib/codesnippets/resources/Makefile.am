AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(WX_GTK_CFLAGS) \
		$(WX_X11_CFLAGS)

pluginlibdir = $(pkglibdir)/plugins

##pluginlib_LTLIBRARIES = codesnippets
##bin_PROGRAMS = codesnippets

##codesnippets_LIBADD = $(WX_LIBS) ../../../../sdk/libcodeblocks.la
##codesnippets_LDADD = $(WX_LIBS) ../../../../sdk/libcodeblocks.la
##codesnippets_LDADD =  -L../../../../sdk ../../../../sdk/libcodeblocks.la \
##            ../../../../base/tinyxml/libtinyxml.la \
##			$(WX_LIBS) $(WX_GTK_LIBS) $(WX_X11_LIBS)

pkgdata_DATA = codesnippets.zip
CLEANFILES = $(pkgdata_DATA)

imagedir = $(pkgdatadir)/images/codesnippets

dist_image_DATA =  allsnippets.png \
		category.png \
		snippet.png \
		findf.png \
		findfdisabled.png \
		options.png \
		optionsdisabled.png \
		stop.png \
		stopdisabled.png

EXTRA_DIST = manifest.xml

codesnippets.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codesnippets.zip manifest.xml > /dev/null

