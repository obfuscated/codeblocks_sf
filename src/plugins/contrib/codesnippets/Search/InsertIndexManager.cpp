/***************************************************************
 * Name:      InsertIndexManager
 * Purpose:   InsertIndexManager is used to get the insertion
 *            index of a path in a list. It is used by the
 *            ThreadSearchLoggerBase derived classes.
 *            File paths are sorted case insensitively either
 *            by file path or file name.
 * Author:    Jerome ANTOINE
 * Created:   2008-01-06
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <wx/string.h>
#include <wx/filename.h>

#include "InsertIndexManager.h"

long InsertIndexManager::GetInsertionIndex(const wxString& filePath, long nbItemstoInsert /*= 1*/)
{
	wxASSERT(nbItemstoInsert > 0);
	wxFileName fileName(filePath);
	wxString   string(filePath);
	long index = 0;

	if ( m_FileSorting == SortByFileName )
	{
		string = fileName.GetFullName();
	}
	string.MakeUpper();

	for (long i = 0; i < nbItemstoInsert; ++i)
	{
		m_SortedStringArray.Add(string);
		if ( i == 0 )
		{
			index = m_SortedStringArray.Index(string.c_str());
		}
	}
	return index;
}
