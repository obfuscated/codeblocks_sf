/***************************************************************
 * Name:      FindDataUtils
 *
 * Purpose:   This class redefines the ThreadSearchFindData
 *            structure defined in cbeditor.cpp.
 *            It improves the search scope.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "ThreadSearchFindData.h"
#include "macrosmanager.h"

ThreadSearchFindData::ThreadSearchFindData()
                     :m_FindText(wxEmptyString)
                     ,m_MatchWord(true)
                     ,m_StartWord(false)
                     ,m_MatchCase(true)
                     ,m_RegEx(false)
                     ,m_Scope(ScopeProjectFiles)
                     ,m_SearchPath(wxT("."))
                     ,m_SearchMask(wxT("*.cpp;*.c;*.h"))
                     ,m_RecursiveSearch(true)
                     ,m_HiddenSearch(true)
{
}


ThreadSearchFindData::ThreadSearchFindData(const ThreadSearchFindData& findData)
                     :m_FindText       (findData.GetFindText())
                     ,m_MatchWord      (findData.GetMatchWord())
                     ,m_StartWord      (findData.GetStartWord())
                     ,m_MatchCase      (findData.GetMatchCase())
                     ,m_RegEx          (findData.GetRegEx())
                     ,m_Scope          (findData.GetScope())
                     ,m_SearchPath     (findData.GetSearchPath())
                     ,m_SearchMask     (findData.GetSearchMask())
                     ,m_RecursiveSearch(findData.GetRecursiveSearch())
                     ,m_HiddenSearch   (findData.GetHiddenSearch())
{
}


ThreadSearchFindData& ThreadSearchFindData::operator=(const ThreadSearchFindData& findData)
{
    m_FindText        = findData.GetFindText();
    m_MatchWord       = findData.GetMatchWord();
    m_StartWord       = findData.GetStartWord();
    m_MatchCase       = findData.GetMatchCase();
    m_RegEx           = findData.GetRegEx();
    m_Scope           = findData.GetScope();
    m_SearchPath      = findData.GetSearchPath();
    m_SearchMask      = findData.GetSearchMask();
    m_RecursiveSearch = findData.GetRecursiveSearch();
    m_HiddenSearch    = findData.GetHiddenSearch();

    return *this;
}


void ThreadSearchFindData::UpdateSearchScope(eSearchScope scope, bool bValue)
{
    if ( bValue == true )
    {
        m_Scope |= scope;
    }
    else
    {
        m_Scope &= ~scope;
    }
}

wxString ThreadSearchFindData::GetSearchPath(bool expanded)  const
{
    if(expanded)
        return Manager::Get()->GetMacrosManager()->ReplaceMacros(m_SearchPath);
    else
        return m_SearchPath;
}
