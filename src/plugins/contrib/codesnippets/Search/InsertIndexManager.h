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

#ifndef INSERT_INDEX_MANAGER_H
#define INSERT_INDEX_MANAGER_H

#include <wx/arrstr.h>

class InsertIndexManager
{
public:
	enum eFileSorting
	{
		SortByFilePath = 0,    // C:\a\b.txt < C:\b\a.txt
		SortByFileName         // C:\a\b.txt > C:\b\a.txt
	};

public:
	/** Constructor. */
	InsertIndexManager(eFileSorting fileSorting = SortByFilePath)
					: m_SortedStringArray()
	                , m_FileSorting(fileSorting)
	{
	}

	/** Destructor. */
	virtual ~InsertIndexManager() {}

	/** GetInsertionIndex
	  * Return the index where the file must be inserted.
	  * Index is found according to sorting strategy (m_FileSorting)
	  * @param filePath        : path of the file to insert
	  * @param nbItemstoInsert : number of items to insert
	  * @return index of the file to insert
	  */
	long GetInsertionIndex(const wxString& filePath, long nbItemstoInsert = 1);

	/** Reset : clears array of paths */
	void Reset() {m_SortedStringArray.Empty();}

	// Accessors
	eFileSorting GetFileSorting() const                   {return m_FileSorting;}
	void         SetFileSorting(eFileSorting fileSorting) {m_FileSorting = fileSorting;}

private:
	wxSortedArrayString m_SortedStringArray;
	eFileSorting        m_FileSorting;
};

#endif // INSERT_INDEX_MANAGER_H
