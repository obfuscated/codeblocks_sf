SpellChecker plugin

It can be configured (through OnlineSpellChecking.xml file) which lexer-styles have to be spell-checked. Sample config file provides: spell-check only comments in C/C++, VHDL and Verilog and spell-check sources for LaTeX.
It needs dictionaries (from OpenOffice) to work correctly. 

Hunspell is used for the spell checker and wxSpellChecker (from wxCode) for the GUI. Code::Blocks project files (msw and linux) are provided (tested on winXP-sp3 and fedora 11). For linux, hunspell is assumed to be installed as a system library.
MyThes is used for the thesaurus.

Dictionaries for hunspell and MyThes can be downloaded from "http://wiki.services.openoffice.org/wiki/Dictionaries" OOo 2.x files. OOo 3.x Extensions (.oxt) are zip'ed archives containing these files. The

The path to the hunspell, thesaurus dictionaries and DictionarySwitcher-bitmaps can be configured through the editor configuration-panel (can be the same path for all files). The files for the thesaurus must have the same name as the hunspell dic/aff files (expect file-endings) with the prefix "th_" the bitmap must have the same name (but .png extension). For example de_CH.aff/de_CH.dic -> th_de_CH.dat/th_de_CH.idx and de_CH.png. Thesaurus will be disabled if the files for the selected language can not be found.

If the CB_STATUS_BAR is defined and the patch cbStatusBar.patch is applyed to C::B, then the plugin provides a "status-field" to switch between dictionaries. The prject files have a NO_CB_STATUS_BAR define prepared, you have to simply remove the NO_ (and patch cb).

Thanks for any comments/hints/test-reports/questions