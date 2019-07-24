SUBDIRS = wxspellchecker

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_HUNSPELL_CFLAGS) \
		-I$(srcdir)/wxspellchecker/include \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libSpellChecker.la

libSpellChecker_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libSpellChecker_la_LIBADD = ../../../sdk/libcodeblocks.la \
							wxspellchecker/libwxSpellChecker.la \
							$(WX_LIBS) \
							$(CB_HUNSPELL_LIBS) \
							$(CB_TINYXML_LIBS)

libSpellChecker_la_SOURCES = DictionariesNeededDialog.cpp \
                            MySpellingDialog.cpp \
                            OnlineSpellChecker.cpp \
                            SpellCheckHelper.cpp \
                            SpellCheckSettingsPanel.cpp \
                            SpellCheckerConfig.cpp \
                            SpellCheckerPlugin.cpp \
                            StatusField.cpp \
                            Thesaurus.cpp \
                            ThesaurusDialog.cpp \
                            mythes.cxx \
                            wxThes.cpp

noinst_HEADERS = DictionariesNeededDialog.h \
                MySpellingDialog.h \
                SpellCheckHelper.h \
                SpellCheckerConfig.h \
                StatusField.h \
                ThesaurusDialog.h \
                OnlineSpellChecker.h \
                SpellCheckSettingsPanel.h \
                SpellCheckerPlugin.h \
                Thesaurus.h \
                mythes.hxx \
                wxThes.h

EXTRA_DIST = \
		$(srcdir)/*.cbp \
		$(srcdir)/*.xrc \
		$(srcdir)/*.png \
		wxsmith \
		hunspell \
		dictionaries \
		manifest.xml \
		$(srcdir)/update*

pkgdata_DATA = SpellChecker.zip

CLEANFILES = $(pkgdata_DATA)

SpellChecker.zip: manifest.xml SpellCheckSettingsPanel.xrc
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/SpellChecker.zip manifest.xml SpellCheckSettingsPanel.xrc > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings

dist_confimages_DATA = $(srcdir)/*.png

onlinecheckerconfigdir = $(datadir)/@PACKAGE@/SpellChecker
dist_onlinecheckerconfig_DATA = \
	$(srcdir)/OnlineSpellChecking.xml

onlinecheckerconfig_16dir = $(datadir)/@PACKAGE@/SpellChecker/16x16
dist_onlinecheckerconfig_16_DATA = \
	rc/16x16/de_AT.png \
	rc/16x16/de_CH.png \
	rc/16x16/de_DE.png \
	rc/16x16/disabled.png \
	rc/16x16/en_AU.png \
	rc/16x16/en_CA.png \
	rc/16x16/en_GB.png \
	rc/16x16/en_NZ.png \
	rc/16x16/en_US.png \
	rc/16x16/fr_FR.png \
	rc/16x16/ru_RU.png

onlinecheckerconfig_20dir = $(datadir)/@PACKAGE@/SpellChecker/20x20
dist_onlinecheckerconfig_20_DATA = \
	rc/20x20/de_AT.png \
	rc/20x20/de_CH.png \
	rc/20x20/de_DE.png \
	rc/20x20/disabled.png \
	rc/20x20/en_AU.png \
	rc/20x20/en_CA.png \
	rc/20x20/en_GB.png \
	rc/20x20/en_NZ.png \
	rc/20x20/en_US.png \
	rc/20x20/fr_FR.png \
	rc/20x20/ru_RU.png

onlinecheckerconfig_24dir = $(datadir)/@PACKAGE@/SpellChecker/24x24
dist_onlinecheckerconfig_24_DATA = \
	rc/24x24/de_AT.png \
	rc/24x24/de_CH.png \
	rc/24x24/de_DE.png \
	rc/24x24/disabled.png \
	rc/24x24/en_AU.png \
	rc/24x24/en_CA.png \
	rc/24x24/en_GB.png \
	rc/24x24/en_NZ.png \
	rc/24x24/en_US.png \
	rc/24x24/fr_FR.png \
	rc/24x24/ru_RU.png

onlinecheckerconfig_28dir = $(datadir)/@PACKAGE@/SpellChecker/28x28
dist_onlinecheckerconfig_28_DATA = \
	rc/28x28/de_AT.png \
	rc/28x28/de_CH.png \
	rc/28x28/de_DE.png \
	rc/28x28/disabled.png \
	rc/28x28/en_AU.png \
	rc/28x28/en_CA.png \
	rc/28x28/en_GB.png \
	rc/28x28/en_NZ.png \
	rc/28x28/en_US.png \
	rc/28x28/fr_FR.png \
	rc/28x28/ru_RU.png

onlinecheckerconfig_32dir = $(datadir)/@PACKAGE@/SpellChecker/32x32
dist_onlinecheckerconfig_32_DATA = \
	rc/32x32/de_AT.png \
	rc/32x32/de_CH.png \
	rc/32x32/de_DE.png \
	rc/32x32/disabled.png \
	rc/32x32/en_AU.png \
	rc/32x32/en_CA.png \
	rc/32x32/en_GB.png \
	rc/32x32/en_NZ.png \
	rc/32x32/en_US.png \
	rc/32x32/fr_FR.png \
	rc/32x32/ru_RU.png

onlinecheckerconfig_40dir = $(datadir)/@PACKAGE@/SpellChecker/40x40
dist_onlinecheckerconfig_40_DATA = \
	rc/40x40/de_AT.png \
	rc/40x40/de_CH.png \
	rc/40x40/de_DE.png \
	rc/40x40/disabled.png \
	rc/40x40/en_AU.png \
	rc/40x40/en_CA.png \
	rc/40x40/en_GB.png \
	rc/40x40/en_NZ.png \
	rc/40x40/en_US.png \
	rc/40x40/fr_FR.png \
	rc/40x40/ru_RU.png

onlinecheckerconfig_48dir = $(datadir)/@PACKAGE@/SpellChecker/48x48
dist_onlinecheckerconfig_48_DATA = \
	rc/48x48/de_AT.png \
	rc/48x48/de_CH.png \
	rc/48x48/de_DE.png \
	rc/48x48/disabled.png \
	rc/48x48/en_AU.png \
	rc/48x48/en_CA.png \
	rc/48x48/en_GB.png \
	rc/48x48/en_NZ.png \
	rc/48x48/en_US.png \
	rc/48x48/fr_FR.png \
	rc/48x48/ru_RU.png

onlinecheckerconfig_56dir = $(datadir)/@PACKAGE@/SpellChecker/56x56
dist_onlinecheckerconfig_56_DATA = \
	rc/56x56/de_AT.png \
	rc/56x56/de_CH.png \
	rc/56x56/de_DE.png \
	rc/56x56/disabled.png \
	rc/56x56/en_AU.png \
	rc/56x56/en_CA.png \
	rc/56x56/en_GB.png \
	rc/56x56/en_NZ.png \
	rc/56x56/en_US.png \
	rc/56x56/fr_FR.png \
	rc/56x56/ru_RU.png

onlinecheckerconfig_64dir = $(datadir)/@PACKAGE@/SpellChecker/64x64
dist_onlinecheckerconfig_64_DATA = \
	rc/64x64/de_AT.png \
	rc/64x64/de_CH.png \
	rc/64x64/de_DE.png \
	rc/64x64/disabled.png \
	rc/64x64/en_AU.png \
	rc/64x64/en_CA.png \
	rc/64x64/en_GB.png \
	rc/64x64/en_NZ.png \
	rc/64x64/en_US.png \
	rc/64x64/fr_FR.png \
	rc/64x64/ru_RU.png
