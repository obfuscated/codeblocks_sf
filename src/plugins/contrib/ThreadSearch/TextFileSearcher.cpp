/***************************************************************
 * Name:      TextFileSearcher
 * Purpose:   TextFileSearcher is used to search text files
 *            for text or regular expressions.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
	#include <globals.h>
#endif

#include <wx/filename.h>

#include "TextFileSearcher.h"
#include "TextFileSearcherText.h"
#include "TextFileSearcherRegEx.h"


TextFileSearcher* TextFileSearcher::BuildTextFileSearcher(const wxString& searchText,
														  bool matchCase,
														  bool matchWordBegin,
														  bool matchWord,
														  bool regEx)
{
	TextFileSearcher* pFileSearcher = NULL;
	if ( regEx == true )
	{
		pFileSearcher = new TextFileSearcherRegEx(searchText, matchCase, matchWordBegin, matchWord);
	}
	else
	{
		pFileSearcher = new TextFileSearcherText(searchText, matchCase, matchWordBegin, matchWord);
	}

	// Tests if construction is OK
	wxString errorMessage(wxEmptyString);
	if ( pFileSearcher && !pFileSearcher->IsOk(&errorMessage)  )
	{
		delete pFileSearcher;
		pFileSearcher = 0;
	}

	return pFileSearcher;
}


TextFileSearcher::eFileSearcherReturn TextFileSearcher::FindInFile(const wxString& filePath, wxArrayString &foundLines)
{
	eFileSearcherReturn success=idStringNotFound;
	wxString line;

	// Tests file existence
	if ( !wxFileName::FileExists(filePath) )
	{
		// We skip missing files without alerting user.
		// If a file has disappeared, it is not our problem.
		// cbMessageBox( filePath + _T(" does not exist."), _T("Error"), wxICON_ERROR);
		return idFileNotFound;
	}

	// File open
	if ( !m_TextFile.Open(filePath, wxConvFile) )
	{
		return idFileOpenError;
	}

	// Tests all file lines
	for ( size_t i = 0; i < m_TextFile.GetLineCount(); ++i )
	{
		line = m_TextFile.GetLine(i);
		if ( MatchLine(line) )
		{
			success=idStringFound;
			// An interesting line is found. We clean and add it to the provided array
			line.Replace(_T("\t"), _T(" "));
			line.Replace(_T("\r"), _T(" "));
			line.Replace(_T("\n"), _T(" "));
			line.Trim(false);
			line.Trim(true);

			foundLines.Add(wxString::Format(wxT("%d"), i + 1));
			foundLines.Add(line);
		}
	}

	// File close
	m_TextFile.Close();

	return success;
}


bool TextFileSearcher::IsOk(wxString* WXUNUSED(pErrorMessage))
{
	return true;
}
