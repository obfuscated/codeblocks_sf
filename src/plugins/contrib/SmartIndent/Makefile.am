INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libSmartIndentHDL.la \
			libSmartIndentPascal.la \
			libSmartIndentCpp.la \
			libSmartIndentPython.la \
			libSmartIndentLua.la \
			libSmartIndentFortran.la \
			libSmartIndentXML.la

libSmartIndentHDL_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentHDL_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentHDL_la_SOURCES = SmartIndentHDL.cpp



libSmartIndentPascal_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentPascal_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentPascal_la_SOURCES = SmartIndentPascal.cpp



libSmartIndentCpp_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentCpp_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentCpp_la_SOURCES = SmartIndentCpp.cpp



libSmartIndentPython_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentPython_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentPython_la_SOURCES = SmartIndentPython.cpp



libSmartIndentLua_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentLua_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentLua_la_SOURCES = SmartIndentLua.cpp



libSmartIndentFortran_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentFortran_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentFortran_la_SOURCES = SmartIndentFortran.cpp


libSmartIndentXML_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSmartIndentXML_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentXML_la_SOURCES = SmartIndentXML.cpp

noinst_HEADERS = SmartIndentHDL.h \
		SmartIndentPascal.h \
		SmartIndentCpp.h \
		SmartIndentPython.h \
		SmartIndentLua.h \
		SmartIndentFortran.h \
		SmartIndentXML.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		manifest_hdl.xml \
		manifest_pascal.xml \
		manifest_cpp.xml \
		manifest_python.xml \
		manifest_lua.xml \
		manifest_fortran.xml \
		manifest_xml.xml

pkgdata_DATA = SmartIndentHDL.zip \
		SmartIndentPascal.zip \
		SmartIndentCpp.zip \
		SmartIndentPython.zip \
		SmartIndentLua.zip \
		SmartIndentFortran.zip \
		SmartIndentXML.zip

CLEANFILES = $(pkgdata_DATA)

.NOTPARALLEL: $(pkgdata_DATA)

SmartIndentHDL.zip: manifest_hdl.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_hdl.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentHDL.zip manifest.xml

SmartIndentPascal.zip: manifest_pascal.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_pascal.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentPascal.zip manifest.xml

SmartIndentCpp.zip: manifest_cpp.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_cpp.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentCpp.zip manifest.xml

SmartIndentPython.zip: manifest_python.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_python.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentPython.zip manifest.xml

SmartIndentLua.zip: manifest_lua.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_lua.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentLua.zip manifest.xml

SmartIndentFortran.zip: manifest_fortran.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_fortran.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentFortran.zip manifest.xml

SmartIndentXML.zip: manifest_xml.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_xml.xml ./manifest.xml && \
	zip -R $(PWD)/SmartIndentXML.zip manifest.xml

