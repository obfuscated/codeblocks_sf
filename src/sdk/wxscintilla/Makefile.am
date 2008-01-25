INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include/wxscintilla/include \
			-I$(srcdir)/src/scintilla/include \
			-I$(srcdir)/src/scintilla/src
			
AM_CPPFLAGS = -DSCI_LEXER -DLINK_LEXERS -DGTK

noinst_LTLIBRARIES = libwxscintilla.la

libwxscintilla_la_SOURCES = src/PlatWX.cpp \
							src/ScintillaWX.cpp \
							src/wxscintilla.cpp \
							src/scintilla/src/AutoComplete.cxx \
							src/scintilla/src/CallTip.cxx \
							src/scintilla/src/CellBuffer.cxx \
							src/scintilla/src/ContractionState.cxx \
							src/scintilla/src/DocumentAccessor.cxx \
							src/scintilla/src/Document.cxx \
							src/scintilla/src/Editor.cxx \
							src/scintilla/src/ExternalLexer.cxx \
							src/scintilla/src/Indicator.cxx \
							src/scintilla/src/KeyMap.cxx \
							src/scintilla/src/KeyWords.cxx \
							src/scintilla/src/LexAda.cxx \
							src/scintilla/src/LexAPDL.cxx \
							src/scintilla/src/LexAsm.cxx \
							src/scintilla/src/LexAsn1.cxx \
							src/scintilla/src/LexAU3.cxx \
							src/scintilla/src/LexAVE.cxx \
							src/scintilla/src/LexBaan.cxx \
							src/scintilla/src/LexBash.cxx \
							src/scintilla/src/LexBasic.cxx \
							src/scintilla/src/LexBullant.cxx \
							src/scintilla/src/LexCaml.cxx \
							src/scintilla/src/LexCLW.cxx \
							src/scintilla/src/LexConf.cxx \
							src/scintilla/src/LexCPP.cxx \
							src/scintilla/src/LexCrontab.cxx \
							src/scintilla/src/LexCsound.cxx \
							src/scintilla/src/LexCSS.cxx \
							src/scintilla/src/LexEiffel.cxx \
							src/scintilla/src/LexErlang.cxx \
							src/scintilla/src/LexEScript.cxx \
							src/scintilla/src/LexFlagship.cxx \
							src/scintilla/src/LexForth.cxx \
							src/scintilla/src/LexFortran.cxx \
							src/scintilla/src/LexGui4Cli.cxx \
							src/scintilla/src/LexHaskell.cxx \
							src/scintilla/src/LexHTML.cxx \
							src/scintilla/src/LexKix.cxx \
							src/scintilla/src/LexLisp.cxx \
							src/scintilla/src/LexLout.cxx \
							src/scintilla/src/LexLua.cxx \
							src/scintilla/src/LexMatlab.cxx \
							src/scintilla/src/LexMetapost.cxx \
							src/scintilla/src/LexMMIXAL.cxx \
							src/scintilla/src/LexMPT.cxx \
							src/scintilla/src/LexMSSQL.cxx \
							src/scintilla/src/LexNsis.cxx \
							src/scintilla/src/LexOthers.cxx \
							src/scintilla/src/LexPascal.cxx \
							src/scintilla/src/LexPB.cxx \
							src/scintilla/src/LexPerl.cxx \
							src/scintilla/src/LexPOV.cxx \
							src/scintilla/src/LexPS.cxx \
							src/scintilla/src/LexPython.cxx \
							src/scintilla/src/LexRebol.cxx \
							src/scintilla/src/LexRuby.cxx \
							src/scintilla/src/LexScriptol.cxx \
							src/scintilla/src/LexSmalltalk.cxx \
							src/scintilla/src/LexSpecman.cxx \
							src/scintilla/src/LexSQL.cxx \
							src/scintilla/src/LexTADS3.cxx \
							src/scintilla/src/LexTeX.cxx \
							src/scintilla/src/LexVB.cxx \
							src/scintilla/src/LexVerilog.cxx \
							src/scintilla/src/LexVHDL.cxx \
							src/scintilla/src/LexYAML.cxx \
							src/scintilla/src/LineMarker.cxx \
							src/scintilla/src/PropSet.cxx \
							src/scintilla/src/RESearch.cxx \
							src/scintilla/src/ScintillaBase.cxx \
							src/scintilla/src/StyleContext.cxx \
							src/scintilla/src/Style.cxx \
							src/scintilla/src/UniConversion.cxx \
							src/scintilla/src/ViewStyle.cxx \
							src/scintilla/src/WindowAccessor.cxx \
							src/scintilla/src/XPM.cxx

noinst_HEADERS = src/scintilla/include/Accessor.h \
							src/scintilla/include/KeyWords.h \
							src/scintilla/include/Platform.h \
							src/scintilla/include/PropSet.h \
							src/scintilla/include/SString.h \
							src/scintilla/include/SciLexer.h \
							src/scintilla/include/Scintilla.h \
							src/scintilla/include/ScintillaWidget.h \
							src/scintilla/include/WindowAccessor.h \
							src/scintilla/src/AutoComplete.h \
							src/scintilla/src/CallTip.h \
							src/scintilla/src/CellBuffer.h \
							src/scintilla/src/ContractionState.h \
							src/scintilla/src/Document.h \
							src/scintilla/src/DocumentAccessor.h \
							src/scintilla/src/Editor.h \
							src/scintilla/src/ExternalLexer.h \
							src/scintilla/src/Indicator.h \
							src/scintilla/src/KeyMap.h \
							src/scintilla/src/LineMarker.h \
							src/scintilla/src/RESearch.h \
							src/scintilla/src/SVector.h \
							src/scintilla/src/ScintillaBase.h \
							src/scintilla/src/Style.h \
							src/scintilla/src/StyleContext.h \
							src/scintilla/src/UniConversion.h \
							src/scintilla/src/ViewStyle.h \
							src/scintilla/src/XPM.h \
							src/PlatWX.h \
							src/ScintillaWX.h

EXTRA_DIST = $(srcdir)/Readme \
	$(srcdir)/build/BuildInfo.txt \
	$(srcdir)/build/Makefile \
	$(srcdir)/build/wxscintilla.* \
	$(srcdir)/lib/dummy \
	$(srcdir)/samples/test/*.h \
	$(srcdir)/samples/test/*.cpp \
	$(srcdir)/samples/test/*.cbp \
	$(srcdir)/samples/test/*.ds* \
	$(srcdir)/samples/test/*.rc \
	$(srcdir)/samples/test/*.rc \
	$(srcdir)/samples/test/*.ico \
	$(srcdir)/website/*.css \
	$(srcdir)/website/*.png \
	$(srcdir)/website/*.php \
	$(srcdir)/website/*.txt \
	$(srcdir)/website/*.htm* \
	$(srcdir)/website/images/*.png \
	$(srcdir)/src/scintilla/*.txt \
	$(srcdir)/src/scintilla/include/*.py \
	$(srcdir)/src/scintilla/include/*.iface \
	$(srcdir)/src/scintilla/src/*.py \
	$(srcdir)/src/scintilla/src/*.properties

# add upper-case copy
CLEANFILES = README
README: $(srcdir)/Readme
	cp -p $< $@
