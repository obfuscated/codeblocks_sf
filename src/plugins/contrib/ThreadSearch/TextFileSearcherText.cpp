/***************************************************************
 * Name:      TextFileSearcherText
 * Purpose:   TextFileSearcherText is used to search text files
 *            for text pattern.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "TextFileSearcherText.h"


TextFileSearcherText::TextFileSearcherText(const wxString& searchText, bool matchCase, bool matchWordBegin,
                                           bool matchWord)
                     :TextFileSearcher(searchText, matchCase, matchWordBegin, matchWord)
{
    if ( matchCase == false )
    {
        m_SearchText.LowerCase();
    }
}


bool TextFileSearcherText::MatchLine(std::vector<int> *outMatchedPositions,
                                     const wxString &originalLine)
{
    wxString line;
    if ( m_MatchCase == false )
    {
        line = originalLine.Lower();
    }
    else
    {
        line = originalLine;
    }

    wxString::size_type start = 0;
    int count = 0;

    const std::vector<int>::size_type countIdx = outMatchedPositions->size();

    do
    {
        // "TestTest Test"
        // start word [0;4], [9;4]
        // match word [9;4]
        // none [0;4] [4;4] [9;4]

        wxString::size_type pos = line.find(m_SearchText, start);
        if (pos == wxString::npos)
            break;

        if ((m_MatchWordBegin || m_MatchWord) && pos > 0)
        {
            // Try to see if this is the start of the word.
            const char prevChar = line.GetChar(pos - 1);
            if (isalnum(prevChar) || prevChar == '_')
            {
                start++;
                continue;
            }
        }

        if (m_MatchWord && (pos + m_SearchText.length() < line.length()))
        {
            // Try to see if this is the end of the word.
            const char nextChar = line.GetChar(pos + m_SearchText.length());
            if (isalnum(nextChar) || nextChar == '_')
            {
                start++;
                continue;
            }
        }

        // We have a match add positions for it.
        if (count == 0)
            outMatchedPositions->push_back(0);
        ++count;
        outMatchedPositions->push_back(pos);
        outMatchedPositions->push_back(m_SearchText.length());

        start = pos + m_SearchText.length();
    } while (1);

    if (count > 0)
    {
        (*outMatchedPositions)[countIdx] = count;
        return true;
    }
    else
        return false;
}

