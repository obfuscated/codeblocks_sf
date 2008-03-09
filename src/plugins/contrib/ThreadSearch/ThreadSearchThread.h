/***************************************************************
 * Name:      ThreadSearchThread
 *
 * Purpose:   This class implements the search worker thread.
 *            It sends events to the view when one or more
 *            search pattern is (are) found in a file; one
 *            event/file.
 *            It derives from wxDirTraverser to test thread
 *            cancel when searching in directory.
 *            One event/file is sent.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_THREAD_H
#define THREAD_SEARCH_THREAD_H

#include <wx/thread.h>
#include <wx/dir.h>
#include <wx/arrstr.h>

#include "ThreadSearchFindData.h"
#include "ThreadSearch.h"
#include "ThreadSearchView.h"


class wxString;

class cbProject;
class cbStyledTextCtrl;

class TextFileSearcher;
class ThreadSearchView;
class ThreadSearchThread;


class ThreadSearchThread : public wxThread, public wxDirTraverser
{
public:
	/** Constructor.
	  * @param pThreadSearchView : worker thread caller
	  * @param findData          : search informations (text, dir, case)
	  */
	ThreadSearchThread(ThreadSearchView* pThreadSearchView, const ThreadSearchFindData& findData);

	/** Destructor. */
	~ThreadSearchThread();

	/** OnExit wxThread override. Used to inform pThreadSearchView
	  * from thread termination.
	  */
	void OnExit();

protected:
	/** Entry wxThread override. Entry point of the thread.
	  */
	void *Entry();

private:
	/** wxDirTraverser OnDir override.
	  * @param dirName : current directory path.
	  */
	wxDirTraverseResult OnDir(const wxString& dirName);

	/** wxDirTraverser OnFile override.
	  * @param fileName : current file path.
	  */
	wxDirTraverseResult OnFile(const wxString& fileName);

	/** FindInFile
	  * Uses the TextFileSearcher to search in the specified file
	  * for expression and sends one event to the view if one or
	  * more occurrences are found.
	  * @param path : file path to search in.
	  */
	void FindInFile(const wxString& path);

	/** Adds item to strings array only if it does not exist.
	  * @param sortedArrayString : strings array to which item may be added.
	  * @param newItem           : item to add if not present in array
	  * @return true if added to array.
	  */
    static bool AddNewItem(wxSortedArrayString& sortedArrayString, const wxString& newItem);

    /** Adds project file paths to the strings array avoiding duplicated paths.
	  * @param sortedArrayString : strings array containing file paths.
	  * @param project           : project whose file paths will be added to sortedArrayString
      */
    void AddProjectFiles(wxSortedArrayString& sortedArrayString, cbProject& project);

	ThreadSearchView*    m_pThreadSearchView; // Worker thread caller
	ThreadSearchFindData m_FindData;          // Structure containing search parameters

	wxDirTraverseResult  m_DefaultDirResult;  // Used to recurse or not in directory search
	wxArrayString        m_LineTextArray;     // Array containing one file search results.
                                              // Succession of 'line number' and line of text
	wxArrayString        m_Masks;             // Used in directory search to filter file paths.
	wxSortedArrayString  m_FilePaths;         // Array containing all file paths that will be
	                                          // searched in.
	TextFileSearcher*    m_pTextFileSearcher; // Searcher object
	bool m_ShowFileMissingError;
	bool m_ShowCantOpenFileError;
};


#endif // THREAD_SEARCH_THREAD_H
