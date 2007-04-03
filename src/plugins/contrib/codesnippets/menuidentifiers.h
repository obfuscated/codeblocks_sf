#ifndef MENUIDENTIFIERS_H_INCLUDED
#define MENUIDENTIFIERS_H_INCLUDED

// Applications Main Menu ids
extern int idMenuFileOpen       ;
extern int idMenuFileSaveAs     ;
extern int idMenuSettingsOptions;
//-extern int idMnuOpenAsFileLink      ;
extern int idMnuConvertToFileLink      ;


#if defined(BUILDING_PLUGIN)
extern int idViewSnippets ;
#endif

#if !defined(BUILDING_PLUGIN)
extern int idMenuProperties    ;
#endif

// Search control menu ids
extern int idSearchSnippetCtrl ;
extern int idSearchCfgBtn      ;
extern int idSnippetsTreeCtrl  ;

// Context Menu items
extern int idMnuAddSubCategory ;
extern int idMnuRemove         ;
extern int idMnuConvertToCategory      ;
extern int idMnuAddSnippet     ;
extern int idMnuApplySnippet   ;
extern int idMnuLoadSnippetsFromFile   ;
extern int idMnuSaveSnippets    ;
extern int idMnuSaveSnippetsToFile     ;
extern int idMnuRemoveAll       ;
extern int idMnuCopyToClipboard ;
extern int idMnuEditSnippet     ;
extern int idMnuConvertToFileLink      ;
extern int idMnuProperties      ;
extern int idMnuSettings        ;


// Search config menu items
extern int idMnuCaseSensitive   ;
extern int idMnuClear           ;
extern int idMnuScope           ;
extern int idMnuScopeSnippets   ;
extern int idMnuScopeCategories ;
extern int idMnuScopeBoth       ;


#endif // MENUIDENTIFIERS_H_INCLUDED
