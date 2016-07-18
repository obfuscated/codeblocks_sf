if BUILD_AUTOVERSIONING
MAYBE_AUTOVERSIONING=AutoVersioning appdata
endif

if BUILD_BROWSETRACKER
MAYBE_BROWSETRACKER=BrowseTracker appdata
endif

if BUILD_BYOGAMES
MAYBE_BYOGAMES=byogames appdata
endif

if BUILD_CBKODERS
MAYBE_CBKODERS=cb_koders appdata
endif

if BUILD_CODESNIPPETS
MAYBE_CODESNIPPETS=codesnippets appdata
endif

if BUILD_CODESTAT
MAYBE_CODESTAT=codestat appdata
endif

if BUILD_COPYSTRINGS
MAYBE_COPYSTRINGS=copystrings appdata
endif

if BUILD_CSCOPE
MAYBE_CSCOPE=Cscope appdata
endif

if BUILD_DOXYBLOCKS
MAYBE_DOXYBLOCKS=DoxyBlocks appdata
endif

if BUILD_DRAGSCROLL
MAYBE_DRAGSCROLL=dragscroll appdata
endif

if BUILD_EDITORCONFIG
MAYBE_EDITORCONFIG=EditorConfig appdata
endif

if BUILD_EDITORTWEAKS
MAYBE_EDITORTWEAKS=EditorTweaks appdata
endif

if BUILD_ENVVARS
MAYBE_ENVVARS=envvars appdata
endif

if BUILD_FILEMANAGER
MAYBE_FILEMANAGER=FileManager appdata
endif

if BUILD_HELP
MAYBE_HELP=help_plugin appdata
endif

if BUILD_HEADERFIXUP
MAYBE_HEADERFIXUP=headerfixup appdata
endif

if BUILD_KEYBINDER
MAYBE_KEYBINDER=keybinder appdata
endif

if BUILD_LIBFINDER
MAYBE_LIBFINDER=wxContribItems lib_finder appdata
endif

if BUILD_PROFILER
MAYBE_PROFILER=profiler appdata
endif

if BUILD_PROJECTOPTIONSMANIPULATOR
MAYBE_PROJECTOPTIONSMANIPULATOR=ProjectOptionsManipulator appdata
endif

if BUILD_REGEX
MAYBE_REGEX=regex_testbed appdata
endif

if BUILD_REOPENEDITOR
MAYBE_REOPENEDITOR=ReopenEditor appdata
endif

if BUILD_EXPORTER
MAYBE_EXPORTER=source_exporter appdata
endif

if BUILD_SMARTINDENT
MAYBE_SMARTINDENT=SmartIndent appdata
endif

if BUILD_SPELLCHECKER
MAYBE_SPELLCHECKER=SpellChecker appdata
endif

if BUILD_SYMTAB
MAYBE_SYMTAB=symtab appdata
endif

if BUILD_THREADSEARCH
MAYBE_THREADSEARCH=wxContribItems ThreadSearch appdata
endif

if BUILD_TOOLSPLUS
MAYBE_TOOLSPLUS=ToolsPlus appdata
endif

if BUILD_VALGRIND
MAYBE_VALGRIND=Valgrind appdata
endif

if BUILD_WXCONTRIB
MAYBE_WXCONTRIB=wxContribItems appdata
endif

if BUILD_WXSMITH
MAYBE_WXSMITH=wxSmith appdata
endif

if BUILD_WXSMITHCONTRIB
MAYBE_WXSMITHCONTRIB=wxSmith wxContribItems wxSmithContribItems appdata
endif

if BUILD_WXSMITHAUI
MAYBE_WXSMITHAUI=wxSmith wxSmithAui appdata
endif

if BUILD_HEXEDITOR
MAYBE_HEXEDITOR=HexEditor appdata
endif

if BUILD_INCSEARCH
MAYBE_INCSEARCH=IncrementalSearch appdata
endif

if BUILD_MOUSESAP
MAYBE_MOUSESAP=MouseSap appdata
endif

if BUILD_NASSISHNEIDERMAN
MAYBE_NASSISHNEIDERMAN=NassiShneiderman appdata
endif

if BUILD_CCCC
MAYBE_CCCC=Cccc appdata
endif

if BUILD_CPPCHECK
MAYBE_CPPCHECK=CppCheck appdata
endif

SUBDIRS = $(MAYBE_AUTOVERSIONING) \
		$(MAYBE_BROWSETRACKER) \
		$(MAYBE_BYOGAMES) \
		$(MAYBE_CBKODERS) \
		$(MAYBE_CCCC) \
		$(MAYBE_CODESNIPPETS) \
		$(MAYBE_CODESTAT) \
		$(MAYBE_COPYSTRINGS) \
		$(MAYBE_CPPCHECK) \
		$(MAYBE_CSCOPE) \
		$(MAYBE_DOXYBLOCKS) \
		$(MAYBE_DRAGSCROLL) \
		$(MAYBE_EDITORCONFIG) \
		$(MAYBE_EDITORTWEAKS) \
		$(MAYBE_ENVVARS) \
		$(MAYBE_FILEMANAGER) \
		$(MAYBE_HEADERFIXUP) \
		$(MAYBE_HELP) \
		$(MAYBE_HEXEDITOR) \
		$(MAYBE_INCSEARCH) \
		$(MAYBE_KEYBINDER) \
		$(MAYBE_LIBFINDER) \
		$(MAYBE_MOUSESAP) \
		$(MAYBE_NASSISHNEIDERMAN) \
		$(MAYBE_PROFILER) \
		$(MAYBE_PROJECTOPTIONSMANIPULATOR) \
		$(MAYBE_EXPORTER) \
		$(MAYBE_SYMTAB) \
		$(MAYBE_SMARTINDENT) \
		$(MAYBE_SPELLCHECKER) \
		$(MAYBE_REGEX) \
		$(MAYBE_REOPENEDITOR) \
		$(MAYBE_THREADSEARCH) \
		$(MAYBE_TOOLSPLUS) \
		$(MAYBE_VALGRIND) \
		$(MAYBE_WXCONTRIB) \
		$(MAYBE_WXSMITH) \
		$(MAYBE_WXSMITHCONTRIB) \
		$(MAYBE_WXSMITHAUI)

DIST_SUBDIRS = AutoVersioning \
		BrowseTracker \
		byogames \
		cb_koders \
		Cccc \
		codesnippets \
		codestat \
		copystrings \
		CppCheck \
		Cscope \
		devpak_plugin \
		DoxyBlocks \
		dragscroll \
		EditorConfig \
		EditorTweaks \
		envvars \
		FileManager \
		headerfixup \
		help_plugin \
		HexEditor \
		keybinder \
		IncrementalSearch \
		lib_finder \
		MouseSap \
		NassiShneiderman \
		profiler \
		ProjectOptionsManipulator \
		source_exporter \
		SmartIndent \
		SpellChecker \
		symtab \
		regex_testbed \
		ReopenEditor \
		ThreadSearch \
		ToolsPlus \
		Valgrind \
		wxContribItems \
		wxSmith \
		wxSmithContribItems \
		wxSmithAui \
		wxSmithDemo \
		wxSmithIExplore \
		wxSmithSTC

EXTRA_DIST = rndgen
if ENABLE_FORTRAN
EXTRA_DIST += $(srcdir)/FortranProject/*.h \
		$(srcdir)/FortranProject/*.cpp \
		$(srcdir)/FortranProject/*.xml \
		$(srcdir)/FortranProject/*.cbp \
		$(srcdir)/FortranProject/update* \
		$(srcdir)/FortranProject/images/*.png \
		$(srcdir)/FortranProject/images/16x16/*.png \
		$(srcdir)/FortranProject/images/fortranproject/*.png \
		$(srcdir)/FortranProject/images/fortranproject/*.f90 \
		$(srcdir)/FortranProject/resources/*.xrc
endif

