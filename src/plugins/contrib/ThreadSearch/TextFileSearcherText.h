/***************************************************************
 * Name:      TextFileSearcherText
 * Purpose:   TextFileSearcherText implements the TextFileSearcher
 *            interface is used to search text files for a text
 *            expression ( != regular expression).
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef TEXT_FILE_SEARCHER_TEXT_H
#define TEXT_FILE_SEARCHER_TEXT_H

#include <wx/string.h>

#include "TextFileSearcher.h"

class TextFileSearcherText : public TextFileSearcher
{
public:
	/** Constructor. */
	// We don't use ThreadSearchFindData to limit coupling
	TextFileSearcherText(const wxString& searchText, bool matchCase, bool matchWordBegin,
						 bool matchWord);

	/** Destructor. */
	virtual ~TextFileSearcherText()	{}

	/** Return true if Line matches search text.
	  * This method is inherited from TextFileSearcher and is used to implement
	  * different search strategies. In TextFileSearcherText, the basic text search
	  * is implemented.
	  * @param line : the text line to match.
	  * @return true if line matches search text.
	  */
	virtual bool MatchLine(wxString line);
};

#endif // TEXT_FILE_SEARCHER_TEXT_H
