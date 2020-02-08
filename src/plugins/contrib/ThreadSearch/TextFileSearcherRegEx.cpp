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

bool TextFileSearcherRegEx::MatchLine(std::vector<int> *outMatchedPositions, const wxString &line)
{
    if (!m_RegEx.IsValid())
        return false;

    const bool match = m_RegEx.Matches(line);
    if (!match)
        return false;

    size_t start, length;
    if (m_RegEx.GetMatch(&start, &length))
    {
        outMatchedPositions->push_back(1);
        outMatchedPositions->push_back(start);
        outMatchedPositions->push_back(length);
    }
    else
        outMatchedPositions->push_back(0);
    return true;
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
