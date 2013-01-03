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
                     :m_FindText       (findData.m_FindText)
                     ,m_MatchWord      (findData.m_MatchWord)
                     ,m_StartWord      (findData.m_StartWord)
                     ,m_MatchCase      (findData.m_MatchCase)
                     ,m_RegEx          (findData.m_RegEx)
                     ,m_Scope          (findData.m_Scope)
                     ,m_SearchPath     (findData.m_SearchPath)
                     ,m_SearchMask     (findData.m_SearchMask)
                     ,m_RecursiveSearch(findData.m_RecursiveSearch)
                     ,m_HiddenSearch   (findData.m_HiddenSearch)
{
}


ThreadSearchFindData& ThreadSearchFindData::operator=(const ThreadSearchFindData& findData)
{
    // protect against "self-assignment"
    if (this != &findData)
    {
        m_FindText        = findData.m_FindText;
        m_MatchWord       = findData.m_MatchWord;
        m_StartWord       = findData.m_StartWord;
        m_MatchCase       = findData.m_MatchCase;
        m_RegEx           = findData.m_RegEx;
        m_Scope           = findData.m_Scope;
        m_SearchPath      = findData.m_SearchPath;
        m_SearchMask      = findData.m_SearchMask;
        m_RecursiveSearch = findData.m_RecursiveSearch;
        m_HiddenSearch    = findData.m_HiddenSearch;
    }

    return *this;
}


void ThreadSearchFindData::UpdateSearchScope(eSearchScope scope, bool bValue)
{
    if (bValue)
        m_Scope |=  scope;
    else
        m_Scope &= ~scope;
}

wxString ThreadSearchFindData::GetSearchPath(bool bExpanded) const
{
    if (bExpanded)
        return Manager::Get()->GetMacrosManager()->ReplaceMacros(m_SearchPath);

    return m_SearchPath;
}
