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
    #include "configmanager.h"
    #include "projectbuildtarget.h"
#endif

#include "cbauibook.h"

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
        event.SetString(_("TextFileSearcher could not be instantiated."));

        // Using wxPostEvent, we avoid multi-threaded memory violation.
        wxPostEvent( m_pThreadSearchView,event);
    }
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("ThreadSearch"));
    m_ShowFileMissingError=pCfg->ReadBool(wxT("/ShowFileMissingError"),true);
    m_ShowCantOpenFileError=pCfg->ReadBool(wxT("/ShowCantOpenFileError"),true);

}


ThreadSearchThread::~ThreadSearchThread()
{
    //dtor
    if ( m_pTextFileSearcher != NULL )
    {
        delete m_pTextFileSearcher;
    }
}


void* ThreadSearchThread::Entry()
{
    // Tests if we have a working searcher object.
    // Cancel search if it is not the case
    if (m_pTextFileSearcher == nullptr)
        return nullptr;

    // For now, we look for all paths for the different search scopes
    // and store them in a sorted array to avoid pasing several times
    // the same file.
    // This will be changed to avoid consuming a lot of memory (parsing
    // form C:\ and storing all paths...). Aim is to avoid the use of the
    // array for storing items.

    // Search in directory files ?
    if (m_FindData.MustSearchInDirectory() == true)
    {
        int flags = wxDIR_FILES | wxDIR_DIRS | wxDIR_DOTDOT;
        flags    |= m_FindData.GetHiddenSearch() ? wxDIR_HIDDEN : 0;

        const wxString &searchPath = m_FindData.GetSearchPath(true);
        const std::vector<wxString> &paths = GetVectorFromString(searchPath, ";", true);

        wxString failedDirectories;
        int failedCount = 0;

        for (const wxString &path : paths)
        {
            if (!wxDir::Exists(path))
            {
                if (failedCount > 0)
                    failedDirectories += ", ";
                failedDirectories += '\'';
                failedDirectories += path;
                failedDirectories += '\'';
                failedCount++;
            }
            else
            {
                wxDir Dir(path);
                Dir.Traverse(*(static_cast<wxDirTraverser*>(this)), wxEmptyString, flags);
            }
        }

        // Tests thread stop (cancel search, app shutdown)
        if (TestDestroy() == true)
            return nullptr;

        if (failedCount > 0)
        {
            ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);

            wxString msg;
            if (failedCount == 1)
                msg = wxString::Format(_("Cannot open folder %s"), failedDirectories.wx_str());
            else
            {
                msg = wxString::Format(_("Cannot open %d folders %s"), failedCount,
                                       failedDirectories.wx_str());
            }
            event.SetString(msg);

            // Using wxPostEvent, we avoid multi-threaded memory violation.
            wxPostEvent(m_pThreadSearchView, event);
        }
    }

    // Search in workspace files ?
    if (m_FindData.MustSearchInWorkspace() == true)
    {
        ProjectsArray* pProjectsArray = Manager::Get()->GetProjectManager()->GetProjects();
        for (size_t j=0; j < pProjectsArray->GetCount(); ++j)
        {
            AddProjectFiles(m_FilePaths, *pProjectsArray->Item(j));
            if (TestDestroy() == true)
                return nullptr;
        }
    }
    else if (m_FindData.MustSearchInProject() == true)
    {
        // Search in project files ?
        // Necessary only if not already parsed in worspace part
        cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (pProject != nullptr)
        {
            AddProjectFiles(m_FilePaths, *pProject);
            if (TestDestroy() == true)
                return nullptr;
        }
    }
    else if (m_FindData.MustSearchInTarget() == true)
    {
        // Search in target files ?
        // Necessary only if not already parsed in project part
        cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (pProject != nullptr)
        {
            ProjectBuildTarget *pTarget = pProject->GetBuildTarget(pProject->GetActiveBuildTarget());
            if (pTarget != nullptr)
            {
                AddTargetFiles(m_FilePaths, *pTarget);
                if (TestDestroy() == true)
                    return nullptr;
            }
        }
    }

    // Tests thread stop (cancel search, app shutdown)
    if (TestDestroy() == true)
        return nullptr;

    // Open files
    if (m_FindData.MustSearchInOpenFiles() == true)
    {
        EditorManager* pEdManager = Manager::Get()->GetEditorManager();
        for (size_t i = 0; i < (size_t)pEdManager->GetNotebook()->GetPageCount(); ++i)
        {
            cbEditor* pEditor = pEdManager->GetBuiltinEditor(i);
            if (pEditor != nullptr)
            {
                AddNewItem(m_FilePaths, pEditor->GetFilename(), m_Masks);
            }
        }
    }

    // Tests thread stop (cancel search, app shutdown)
    if (TestDestroy() == true)
        return nullptr;

    // if the list is empty, leave
    if (m_FilePaths.GetCount() == 0)
    {
        //-cbMessageBox(wxT("No files to search in!"), wxT("Error"), wxICON_WARNING);
        ////(pecan 2008/4/26)
        // DO NOT issue graphics calls from this thread !!!!!!
        ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);
        event.SetString(_("No files to search.\nCheck options "));
        // Using wxPostEvent, we avoid multi-threaded memory violation.
        wxPostEvent(m_pThreadSearchView,event);
        return nullptr;
    }

    for (size_t i = 0; i < m_FilePaths.GetCount(); ++i)
    {
        FindInFile(m_FilePaths[i]);

        // Tests thread stop (cancel search, app shutdown)
        if (TestDestroy() == true)
            return nullptr;
    }

    return nullptr;
}


void ThreadSearchThread::OnExit()
{
    // Method is called automatically by wxWidgets framework
    // We inform thread caller about its termination.
    m_pThreadSearchView->OnThreadExit();
}


wxDirTraverseResult ThreadSearchThread::OnDir(const wxString& dirName)
{
    // Method is just used to test thread termination (user cancelled) and
    // stop recursive dir traversing if it is not required.
    if ( TestDestroy() == true )
    {
        return wxDIR_STOP;
    }

    wxString path = cbResolveSymLinkedDirPathRecursive(dirName);
    if (m_VisitedDirs.find(path) != m_VisitedDirs.end())
        return wxDIR_IGNORE;
    m_VisitedDirs.insert(path);

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
    m_MatchedPositions.clear();

    switch ( m_pTextFileSearcher->FindInFile(path, m_LineTextArray, m_MatchedPositions) )
    {
        case TextFileSearcher::idStringFound:
        {
            ThreadSearchEvent event(wxEVT_THREAD_SEARCH, -1);
            event.SetString(path);
            event.SetLineTextArray(m_LineTextArray);
            event.SetMatchedPositions(m_MatchedPositions);

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
            if(m_ShowFileMissingError)
            {
                ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);
                event.SetString(path + _(" does not exist."));

                // Using wxPostEvent, we avoid multi-threaded memory violation.
                wxPostEvent( m_pThreadSearchView,event);
            }
            break;
        }
        case TextFileSearcher::idFileOpenError:
        {
            if(m_ShowCantOpenFileError)
            {
                ThreadSearchEvent event(wxEVT_THREAD_SEARCH_ERROR, -1);
                event.SetString(_("Failed to open ") + path);

                // Using wxPostEvent, we avoid multi-threaded memory violation.
                wxPostEvent( m_pThreadSearchView,event);
            }
            break;
        }
        default:
        {
        }
    }
}


bool ThreadSearchThread::AddNewItem(wxSortedArrayString& sortedArrayString, const wxString& newItem, const wxArrayString& mask)
{
    // Adds item to array only if it does not exist
    bool added = false;
    if ( sortedArrayString.Index(newItem.c_str()) == wxNOT_FOUND )
    {
        // Looks if current file matches one of the file patterns
        size_t maskCount = mask.GetCount();
        if (maskCount == 0)
        {
            sortedArrayString.Add(newItem);
            added = true;
        }
        else
        {
            for (size_t i = 0; i < maskCount; ++i)
            {
                if ( newItem.Matches(mask[i].c_str() ) )
                {
                    sortedArrayString.Add(newItem);
                    added = true;
                    break;
                }
            }
        }
    }
    return added;
}


void ThreadSearchThread::AddProjectFiles(wxSortedArrayString& sortedArrayString, cbProject& project)
{
    // Adds project file paths to array only if they do not already exist.
    // Same path may exist if we parse both open files and project files
    // for examle.
    for (FilesList::iterator it = project.GetFilesList().begin(); it != project.GetFilesList().end(); ++it)
    {
        AddNewItem(sortedArrayString, (*it)->file.GetFullPath(), m_Masks);
        if ( TestDestroy() == true ) return;
    }
}


void ThreadSearchThread::AddTargetFiles(wxSortedArrayString& sortedArrayString, ProjectBuildTarget& target)
{
    // Adds target file paths to array only if they do not already exist.
    // Same path may exist if we parse both open files and target files
    // for examle.
    for (FilesList::iterator it = target.GetFilesList().begin(); it != target.GetFilesList().end(); it++)
    {
        ProjectFile* pf = *it;
        AddNewItem(sortedArrayString, pf->file.GetFullPath(), m_Masks);
        if ( TestDestroy() == true ) return;
    }
}



