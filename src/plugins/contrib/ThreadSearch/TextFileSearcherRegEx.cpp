/***************************************************************
 * Name:      TextFileSearcherRegEx
 * Purpose:   TextFileSearcherRegEx is used to search text files
 *            for regular expressions.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "sdk.h"
#ifndef CB_PRECOMP
    // Required extra includes
    #include <wx/intl.h>
    #include <wx/string.h>
#endif

#include "TextFileSearcherRegEx.h"


TextFileSearcherRegEx::TextFileSearcherRegEx(const wxString& searchText, bool matchCase, bool matchWordBegin,
                                            bool matchWord)
                      :TextFileSearcher(searchText, matchCase, matchWordBegin, matchWord)
{
    wxString pattern = searchText;

#ifdef wxHAS_REGEX_ADVANCED
    int flags = wxRE_ADVANCED;
#else
    int flags = wxRE_EXTENDED;
#endif
    if ( matchCase == false )
    {
        flags |= wxRE_ICASE;
    }

    if ( matchWord == true )
    {
        pattern = _T("([^[:alnum:]_]|^)") + pattern + _T("([^[:alnum:]_]|$)");
    }
    else if ( matchWordBegin == true )
    {
        pattern = _T("([^[:alnum:]_]|^)") + pattern;
    }

    m_RegEx.Compile(pattern, flags);
}


bool TextFileSearcherRegEx::MatchLine(wxString line)
{
    bool match = false;
    if ( m_RegEx.IsValid() )
    {
        match = m_RegEx.Matches(line.c_str());
    }
    return match;
}


bool TextFileSearcherRegEx::IsOk(wxString* pErrorMessage)
{
    bool ok = m_RegEx.IsValid();
    if ( !ok && pErrorMessage )
    {
        *pErrorMessage = _("Bad regular expression.");
    }
    return ok;
}
