/***************************************************************
 * Name:      ThreadSearchFindData
 *
 * Purpose:   This class stores search data.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_FIND_DATA_H
#define THREAD_SEARCH_FIND_DATA_H


#include <wx/string.h>

// Possible search scopes.
enum eSearchScope
{
    ScopeOpenFiles      = 1,
    ScopeProjectFiles   = 2,
    ScopeWorkspaceFiles = 4,
    ScopeDirectoryFiles = 8,
    ScopeTargetFiles    = 16
};

// No comments, basic class
class ThreadSearchFindData
{
public:
    ThreadSearchFindData();
    ThreadSearchFindData(const ThreadSearchFindData& findData);
    ThreadSearchFindData& operator= (const ThreadSearchFindData& findData);

    ~ThreadSearchFindData() {}

    void UpdateSearchScope(eSearchScope scope, bool bValue);
    bool MustSearchInOpenFiles() {return (m_Scope & ScopeOpenFiles)      != 0;}
    bool MustSearchInProject  () {return (m_Scope & ScopeProjectFiles)   != 0;}
    bool MustSearchInWorkspace() {return (m_Scope & ScopeWorkspaceFiles) != 0;}
    bool MustSearchInDirectory() {return (m_Scope & ScopeDirectoryFiles) != 0;}
    bool MustSearchInTarget   () {return (m_Scope & ScopeTargetFiles)    != 0;}

    // Setters
    void SetFindText       (const wxString& findText)   {m_FindText        = findText;}
    void SetMatchWord      (bool matchWord)             {m_MatchWord       = matchWord;}
    void SetStartWord      (bool startWord)             {m_StartWord       = startWord;}
    void SetMatchCase      (bool matchCase)             {m_MatchCase       = matchCase;}
    void SetRegEx          (bool regEx)                 {m_RegEx           = regEx;}
    void SetScope          (int scope)                  {m_Scope           = scope;}
    void SetSearchPath     (const wxString& searchPath) {m_SearchPath      = searchPath;}
    void SetSearchMask     (const wxString& searchMask) {m_SearchMask      = searchMask;}
    void SetRecursiveSearch(bool recursiveSearch)       {m_RecursiveSearch = recursiveSearch;}
    void SetHiddenSearch   (bool hiddenSearch)          {m_HiddenSearch    = hiddenSearch;}

    wxString GetFindText()        const {return m_FindText;}
    bool     GetMatchWord()       const {return m_MatchWord;}
    bool     GetStartWord()       const {return m_StartWord;}
    bool     GetMatchCase()       const {return m_MatchCase;}
    bool     GetRegEx()           const {return m_RegEx;}
    int      GetScope()           const {return m_Scope;}
    wxString GetSearchMask()      const {return m_SearchMask;}
    bool     GetRecursiveSearch() const {return m_RecursiveSearch;}
    bool     GetHiddenSearch()    const {return m_HiddenSearch;}

    wxString GetSearchPath(bool bExpanded = false) const;

private:
    wxString m_FindText;
    bool     m_MatchWord;
    bool     m_StartWord;
    bool     m_MatchCase;
    bool     m_RegEx;
    int      m_Scope;
    wxString m_SearchPath;
    wxString m_SearchMask;
    bool     m_RecursiveSearch;
    bool     m_HiddenSearch;
};

#endif // THREAD_SEARCH_FIND_DATA_H
