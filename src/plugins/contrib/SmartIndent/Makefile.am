INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libHDLSmartIndent.la \
			libPascalSmartIndent.la \
			libCppSmartIndent.la \
			libPythonSmartIndent.la \
			libLuaSmartIndent.la \
			libFortranSmartIndent.la \
			libXMLSmartIndent.la

libHDLSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libHDLSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libHDLSmartIndent_la_SOURCES = HDLSmartIndent.cpp



libPascalSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libPascalSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libPascalSmartIndent_la_SOURCES = PascalSmartIndent.cpp



libCppSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libCppSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libCppSmartIndent_la_SOURCES = CppSmartIndent.cpp



libPythonSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libPythonSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libPythonSmartIndent_la_SOURCES = PythonSmartIndent.cpp



libLuaSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libLuaSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libLuaSmartIndent_la_SOURCES = LuaSmartIndent.cpp



libFortranSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libFortranSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libFortranSmartIndent_la_SOURCES = FortranSmartIndent.cpp


libXMLSmartIndent_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libXMLSmartIndent_la_LIBADD = $(PLUGIN_WX_LIBS) \
			../../../sdk/libcodeblocks.la

libXMLSmartIndent_la_SOURCES = XMLSmartIndent.cpp

noinst_HEADERS = HDLSmartIndent.h \
		PascalSmartIndent.h \
		CppSmartIndent.h \
		PythonSmartIndent.h \
		LuaSmartIndent.h \
		FortranSmartIndent.h \
		XMLSmartIndent.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		hdlmanifest.xml \
		pascalmanifest.xml \
		cppmanifest.xml \
		pythonmanifest.xml \
		luamanifest.xml \
		fortranmanifest.xml \
		xmlmanifest.xml

pkgdata_DATA = HDLSmartIndent.zip \
		PascalSmartIndent.zip \
		CppSmartIndent.zip \
		PythonSmartIndent.zip \
		LuaSmartIndent.zip \
		FortranSmartIndent.zip \
		XMLSmartIndent.zip

CLEANFILES = $(pkgdata_DATA)

HDLSmartIndent.zip: hdlmanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp hdlmanifest.xml ./manifest.xml && \
	zip -R $(PWD)/HDLSmartIndent.zip manifest.xml

PascalSmartIndent.zip: pascalmanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp pascalmanifest.xml ./manifest.xml && \
	zip -R $(PWD)/PascalSmartIndent.zip manifest.xml

CppSmartIndent.zip: cppmanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp cppmanifest.xml ./manifest.xml && \
	zip -R $(PWD)/CppSmartIndent.zip manifest.xml

PythonSmartIndent.zip: pythonmanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp pythonmanifest.xml ./manifest.xml && \
	zip -R $(PWD)/PythonSmartIndent.zip manifest.xml

LuaSmartIndent.zip: luamanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp luamanifest.xml ./manifest.xml && \
	zip -R $(PWD)/LuaSmartIndent.zip manifest.xml

FortranSmartIndent.zip: fortranmanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp fortranmanifest.xml ./manifest.xml && \
	zip -R $(PWD)/FortranSmartIndent.zip manifest.xml

XMLSmartIndent.zip: xmlmanifest.xml
	PWD=`pwd` cd $(srcdir) && \
	cp xmlmanifest.xml ./manifest.xml && \
	zip -R $(PWD)/XMLSmartIndent.zip manifest.xml

