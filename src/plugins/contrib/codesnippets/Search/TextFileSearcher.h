/***************************************************************
 * Name:      TextFileSearcher
 * Purpose:   TextFileSearcher is an abstract class that is used
 *            to search text files.
 *            Derived classes allows text searches using text
 *            or regular expressions.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef TEXT_FILE_SEARCHER_H
#define TEXT_FILE_SEARCHER_H

#include <wx/string.h>
#include <wx/textfile.h>

class TextFileSearcher
{
public:
	enum eFileSearcherReturn
	{
		idStringFound=0,
		idStringNotFound,
		idFileNotFound,
		idFileOpenError
	};

	/** BuildTextFileSearcher
	  * Builds a TextFileSearcherRegEx or a TextFileSearcherText pointer depending on regEx
	  * @return TextFileSearcher* : NULL if failure (regular expression syntax for example)
	  */
	// We don't use ThreadSearchFindData to limit coupling
	static TextFileSearcher* BuildTextFileSearcher(const wxString& searchText,
												   bool matchCase,
												   bool matchWordBegin,
												   bool matchWord,
												   bool regEx);


	/** Destructor. */
	virtual ~TextFileSearcher() {};

	/** Return true if line matches search text.
	  * This method must be implemented in derived classes to allows using
	  * different search strategies (at least simple text and regular expressions).
	  * @param line : the text line to match.
	  * @return true if line matches search text.
	  */
	virtual bool MatchLine(wxString line) = 0;

	/** Return true if object is OK.
	  * Exists to test validity of the object, mainly for reg ex syntax errors.
	  * @param pErrorMessage : error message if object is not valid. May be NULL
	  *                        if it is not necessary to get it.
	  * @return true if object is Ok, ie usable.
	  */
	virtual bool IsOk(wxString* pErrorMessage = NULL);

	/** Return true if Line matches search text.
	  * This method must be implemented in derived classes to allows using
	  * different search strategies (at least simple text and regular expressions use.
	  * @param filePath : file path in which we look for m_SearchText
	  * @param foundLines : array that stores a list of line number and line text
	  * @return true if success (error can only come from bad reg ex or file open
	  * failure).
	  */
	eFileSearcherReturn FindInFile(const wxString& filePath, wxArrayString &foundLines);

protected:
	/** Constructor. */
	// We don't use ThreadSearchFindData to limit coupling
	TextFileSearcher(const wxString& searchText, bool matchCase, bool matchWordBegin, bool matchWord):
					 m_SearchText(searchText),
					 m_MatchCase(matchCase),
					 m_MatchWordBegin(matchWordBegin),
					 m_MatchWord(matchWord)
	{}

	wxString   m_SearchText;
	bool       m_MatchCase;
	bool       m_MatchWordBegin;
	bool       m_MatchWord;
	wxTextFile m_TextFile;
};

#endif // TEXT_FILE_SEARCHER_H
