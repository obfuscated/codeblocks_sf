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


bool TextFileSearcherText::MatchLine(wxString line)
{
    bool match = false;
    if ( m_MatchCase == false )
    {
        line.LowerCase();
    }
    int pos = line.Find(m_SearchText.c_str());
    int nextPos;
    while ( (match == false) && (pos >= 0) )
    {
        char c = ' '; // c is either the preceeding char or a virtual char
                      // that matches systematically the required conditions
        match = true; // pos > 0 => expr found => Matches. Let's test start word
                      // and whole words conditions.
        if ( (m_MatchWordBegin == true) || (m_MatchWord == true) )
        {
            if ( pos > 0 )
            {
                c = line.GetChar(pos - 1);
            }
            //match = (__iscsym(c) == 0);
            match = !(isalnum(c) || ( c == '_' ));
        }

        if ( (match == true) && (m_MatchWord == true) )
        {
            c = ' ';
            if ( (pos + m_SearchText.Length()) < line.Length() )
            {
                c = line.GetChar(pos + m_SearchText.Length());
            }
            match = !(isalnum(c) || ( c == '_' ));
        }

        nextPos = line.Mid(pos+1).Find(m_SearchText.c_str());
        if ( nextPos >= 0 )
        {
            pos += nextPos + 1;
        }
        else
        {
            pos = -1;
        }
    }

    return match;
}

