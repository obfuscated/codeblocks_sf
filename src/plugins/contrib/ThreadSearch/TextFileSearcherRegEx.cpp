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


TextFileSearcherRegEx::TextFileSearcherRegEx(const wxString& searchText, bool matchCase,
                                             bool matchWordBegin, bool matchWord) :
    TextFileSearcher(searchText, matchCase, matchWordBegin, matchWord)
{
#ifdef wxHAS_REGEX_ADVANCED
    int flags = wxRE_ADVANCED;
#else
    int flags = wxRE_EXTENDED;
#endif
    if (matchCase == false)
    {
        flags |= wxRE_ICASE;
    }

    wxString pattern;
    if (matchWord == true)
    {
        pattern = _T("([^[:alnum:]_]|^)(") + searchText + _T(")([^[:alnum:]_]|$)");
        m_IndexToMatch = 2;
    }
    else if (matchWordBegin == true)
    {
        pattern = _T("([^[:alnum:]_]|^)(") + searchText + wxT(")");
        m_IndexToMatch = 2;
    }
    else
    {
        m_IndexToMatch = 0;
        pattern = searchText;
    }

    m_RegEx.Compile(pattern, flags);
}

bool TextFileSearcherRegEx::MatchLine(std::vector<int> *outMatchedPositions, const wxString &line)
{
    if (!m_RegEx.IsValid())
        return false;

    const wxChar *lineBuffer = line.wx_str();
    const bool match = m_RegEx.Matches(lineBuffer, 0, line.length());
    if (!match)
        return false;

    const std::vector<int>::size_type countIdx = outMatchedPositions->size();
    outMatchedPositions->push_back(0);

    int count = 0;

    size_t start, length;

    // GetMatch returns the values relative to the start of the string, so for the second and later
    // matches we need to know the position of the string relative to the full string.
    int offset = 0;
    while (m_RegEx.GetMatch(&start, &length, m_IndexToMatch))
    {
        count++;
        outMatchedPositions->push_back(start + offset);
        outMatchedPositions->push_back(length);

        offset += start + length;

        if (!m_RegEx.Matches(lineBuffer + offset, 0, line.length() - offset))
            break;
    }

    (*outMatchedPositions)[countIdx] = count;
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
