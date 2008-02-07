/***************************************************************
 * Name:      ThreadSearchThread
 *
 * Purpose:   This class implements the search worker thread.
 *            It sends events to the view when one or more
 *            search pattern is (are) found in a file.
 *            It derives from wxDirTraverser to test thread
 *            cancel when searching in directory.
 *            One event/file is sent.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "sdk.h"
#ifndef CB_PRECOMP
	#include "cbeditor.h"
#endif

#include <wx/wxFlatNotebook/wxFlatNotebook.h>

#include "ThreadSearchThread.h"
#include "ThreadSearchEvent.h"
#include "TextFileSearcher.h"

ThreadSearchThread::ThreadSearchThread(ThreadSearchView*           pThreadSearchView,
									   const ThreadSearchFindData& findData)
{
	m_pThreadSearchView = pThreadSearchView;
	m_FindData          = findData;

	// If wxDIR_IGNORE is used, we don't recurse in sub directories during directory search
	m_DefaultDirResult  = (findData.GetRecursiveSearch() == true) ? wxDIR_CONTINUE : wxDIR_IGNORE;

	// File patterns separator is ';'
	m_Masks             = GetArrayFromString(m_FindData.GetSearchMask());
	if ( m_Masks.GetCount() == 0 )
	{
		m_Masks.Add(_T("*"));
	}
	m_pTextFileSearcher = TextFileSearcher::BuildTextFileSearcher(findData.GetFindText(),
																  findData.GetMatchCase(),
																  findData.GetStartWord(),
																  findData.GetMatchWord(),
																  findData.GetRegEx());
	if (!m_pTextFileSearcher)
	{
		ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);
		event.SetString(_T("TextFileSearcher could not be instantiated."));
		
		// Using wxPostEvent, we avoid multi-threaded memory violation.
		wxPostEvent( m_pThreadSearchView,event);
	}
}


ThreadSearchThread::~ThreadSearchThread()
{
	//dtor
	if ( m_pTextFileSearcher != NULL )
	{
		delete m_pTextFileSearcher;
	}
}


void *ThreadSearchThread::Entry()
{
	// Tests if we have a working searcher object.
	// Cancel search if it is not the case
	if ( m_pTextFileSearcher == NULL )
		return 0;

	size_t i = 0;

	// For now, we look for all paths for the different search scopes
	// and store them in a sorted array to avoid pasing several times
	// the same file.
	// This will be changed to avoid consuming a lot of memory (parsing
	// form C:\ and storing all paths...). Aim is to avoid the use of the
	// array for storing items.

	// Search in directory files ?
	if ( m_FindData.MustSearchInDirectory() == true )
	{
		int flags = wxDIR_FILES | wxDIR_DIRS | wxDIR_DOTDOT;
		flags    |= m_FindData.GetHiddenSearch() ? wxDIR_HIDDEN : 0;

		wxDir Dir(m_FindData.GetSearchPath());
		Dir.Traverse(*(static_cast<wxDirTraverser*>(this)), wxEmptyString, flags);

		// Tests thread stop (cancel search, app shutdown)
		if ( TestDestroy() == true ) return 0;
	}

	// Search in workspace files ?
	if ( m_FindData.MustSearchInWorkspace() == true )
	{
		ProjectsArray* pProjectsArray = Manager::Get()->GetProjectManager()->GetProjects();
		for ( size_t i=0; i < pProjectsArray->GetCount(); ++i )
		{
			AddProjectFiles(m_FilePaths, *pProjectsArray->Item(i));
			if ( TestDestroy() == true ) return 0;
		}
	}
	else if ( m_FindData.MustSearchInProject() == true )
	{
		// Search in project files ?
		// Necessary only if not already parsed in worspace part
		cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
		if ( pProject != NULL )
		{
			AddProjectFiles(m_FilePaths, *pProject);
			if ( TestDestroy() == true ) return 0;
		}
	}

	// Tests thread stop (cancel search, app shutdown)
	if ( TestDestroy() == true ) return 0;

	// Open files
	if ( m_FindData.MustSearchInOpenFiles() == true )
	{
		EditorManager* pEdManager = Manager::Get()->GetEditorManager();
		for (i = 0; i < (size_t)pEdManager->GetNotebook()->GetPageCount(); ++i)
		{
			cbEditor* pEditor = pEdManager->GetBuiltinEditor(i);
			if ( pEditor != NULL )
			{
				AddNewItem(m_FilePaths, pEditor->GetFilename());
			}
		}
	}

	// Tests thread stop (cancel search, app shutdown)
	if ( TestDestroy() == true ) return 0;

	// if the list is empty, leave
	if (m_FilePaths.GetCount() == 0)
	{
		cbMessageBox(wxT("No files to search in!"), wxT("Error"), wxICON_WARNING);
		return 0;
	}

	for ( i = 0; i < m_FilePaths.GetCount(); ++i )
	{
		FindInFile(m_FilePaths[i]);

		// Tests thread stop (cancel search, app shutdown)
		if ( TestDestroy() == true ) return 0;
	}

	return 0;
}


void ThreadSearchThread::OnExit()
{
	// Method is called automatically by wxWidgets framework
	// We inform thread caller about its termination.
	m_pThreadSearchView->OnThreadExit();
}


wxDirTraverseResult ThreadSearchThread::OnDir(const wxString& WXUNUSED(dirName))
{
	// Method is just used to test thread termination (user cancelled) and
	// stop recursive dir traversing if it is not required.
	if ( TestDestroy() == true )
	{
		return wxDIR_STOP;
	}
	return m_DefaultDirResult;
}


wxDirTraverseResult ThreadSearchThread::OnFile(const wxString& fileName)
{
	// Tests thread termination (user cancelled)
	if ( TestDestroy() == true )
	{
		return wxDIR_STOP;
	}

	// Looks if current file matches one of the file patterns
	for (size_t i = 0; i < m_Masks.GetCount(); ++i)
	{
		if ( fileName.Matches(m_Masks[i].c_str() ) )
		{
			// Adds it to list of files to parse
			m_FilePaths.Add(fileName);
			break;
		}
	}

	return wxDIR_CONTINUE;
}


void ThreadSearchThread::FindInFile(const wxString& path)
{
	m_LineTextArray.Empty();

	switch ( m_pTextFileSearcher->FindInFile(path, m_LineTextArray) )
	{
		case TextFileSearcher::idStringFound:
		{
			ThreadSearchEvent event(wxEVT_THREAD_SEARCH, -1);
			event.SetString(path);
			event.SetLineTextArray(m_LineTextArray);
			
			// Using wxPostEvent, we avoid multi-threaded memory violation.
			m_pThreadSearchView->PostThreadSearchEvent(event);
			break;
		}
		case TextFileSearcher::idStringNotFound:
		{
			break;
		}
		case TextFileSearcher::idFileNotFound:
		{
			ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);
			event.SetString(path + _T(" does not exist."));
			
			// Using wxPostEvent, we avoid multi-threaded memory violation.
			wxPostEvent( m_pThreadSearchView,event);
			break;
		}
		case TextFileSearcher::idFileOpenError:
		{
			ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);
			event.SetString(_T("Failed to open ") + path);
			
			// Using wxPostEvent, we avoid multi-threaded memory violation.
			wxPostEvent( m_pThreadSearchView,event);
			break;
		}
		default:
		{
		}
	}
}


bool ThreadSearchThread::AddNewItem(wxSortedArrayString& sortedArrayString, const wxString& newItem)
{
	// Adds item to array only if it does not exist
	bool added = false;
	if ( sortedArrayString.Index(newItem.c_str()) == wxNOT_FOUND )
	{
		sortedArrayString.Add(newItem);
		added = true;
	}
	return added;
}


void ThreadSearchThread::AddProjectFiles(wxSortedArrayString& sortedArrayString, cbProject& project)
{
	// Adds project file paths to array only if they do not already exist.
	// Same path may exist if we parse both open files and project files
	// for examle.
	for ( int i = 0; i < project.GetFilesCount(); ++i )
	{
		AddNewItem(sortedArrayString, project.GetFile(i)->file.GetFullPath());
		if ( TestDestroy() == true ) return;
	}
}


