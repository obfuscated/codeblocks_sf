/***************************************************************
 * Name:      TextFileSearcherRegEx
 * Purpose:   TextFileSearcherRegEx implements the TextFileSearcher
 *            interface. It is used to search text files for a
 *            regular expression.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef TEXT_FILE_SEARCHER_REG_EX_H
#define TEXT_FILE_SEARCHER_REG_EX_H

#include <wx/regex.h>
#include <wx/string.h>

#include "TextFileSearcher.h"

class TextFileSearcherRegEx : public TextFileSearcher
{
public:
    /** Constructor. */
    TextFileSearcherRegEx(const wxString& searchText, bool matchCase, bool matchWordBegin,
                          bool matchWord);

    /** Return true if Line matches search text.
      * This method is inherited from TextFileSearcher and is used to implement
      * different search strategies. In TextFileSearcherRegEx, the searchText provided
      * in the constructor is a regular expression pattern.
      * @param Line : the text line to match.
      * @return true if line matches search text.
      */
    bool MatchLine(std::vector<int> *outMatchedPositions, const wxString &line) override;

    /** Return true if object is OK.
      * @return true if object is Ok, ie usable.
      */
    bool IsOk(wxString* pErrorMessage = nullptr) override;

protected:
    wxRegEx m_RegEx;
    int m_IndexToMatch;
};

#endif // TEXT_FILE_SEARCHER_REG_EX_H
