/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/app.h> // wxPostEvent
    #include <wx/dir.h> // wxDirTraverser
    #include <wx/event.h>
    #include <wx/filename.h>
#endif

#include "systemheadersthread.h"

#define CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT
        #define CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT
        #define CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT 2
    #endif
#endif

#if CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_SYSTEMHEADERSTHREAD_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif


// when finished collecting all files, the thread are going to die, send this event
long idSystemHeadersThreadFinish = wxNewId();
// when collect all files under one path, send this event
long idSystemHeadersThreadUpdate = wxNewId();
// could not open the path, send an error event
long idSystemHeadersThreadError  = wxNewId();


// internal class declaration of HeaderDirTraverser (implementation below)

class HeaderDirTraverser : public wxDirTraverser
{
public:
    HeaderDirTraverser(wxThread* thread, wxCriticalSection* critSect,
                       SystemHeadersMap& headersMap, const wxString& searchDir);
    virtual ~HeaderDirTraverser();

    /** call back function when we meet a file */
    virtual wxDirTraverseResult OnFile(const wxString& filename);

    /** call back function when we meet a dir */
    virtual wxDirTraverseResult OnDir(const wxString& dirname);

private:
    /** this function will be called every time we meet a file or a dir, and we count the file and
     * dir, we temporary leave the critical section to give other thread a change to access the file
     * maps.
     */
    void AddLock(bool is_file);

    wxDirTraverseResult GetStatus(const wxString &path);
private:
    /* the thread call Traverse() on this instance*/
    wxThread*               m_Thread;
    /* critical section to protect accessing m_SystemHeadersMap */
    wxCriticalSection*      m_SystemHeadersThreadCS;
    /* dir to files map, for example, you are two dirs c:/a and c:/b
     * so the map looks like: (usually the relative file path is stored
     * c:/a  ---> {c:/a/a1.h, c:/a/a2.h} ---> {a1.h, a2.h}
     * c:/b  ---> {c:/b/b1.h, c:/b/b2.h} ---> {b1.h, b2.h}
     */
    const SystemHeadersMap& m_SystemHeadersMap;

#ifndef _WIN32
    // Set of already visited directories (stored as absolute paths).
    std::set<wxString>      m_VisitedDirs;
#endif // _WIN32

    /* top level dir we are traversing header files */
    const wxString&         m_SearchDir;
    /* string set for header files */
    StringSet&              m_Headers;

    /** indicates whether the critical section is entered or not, used in AddLock() function*/
    bool                    m_Locked;
    /* numbers of dirs in the traversing */
    size_t                  m_Dirs;
    /* numbers of files in the traversing */
    size_t                  m_Files;
};

// class SystemHeadersThread

SystemHeadersThread::SystemHeadersThread(wxEvtHandler*        parent,
                                         wxCriticalSection*   critSect,
                                         SystemHeadersMap&    headersMap,
                                         const wxArrayString& incDirs) :
    wxThread(wxTHREAD_JOINABLE),
    m_Parent(parent),
    m_SystemHeadersThreadCS(critSect),
    m_SystemHeadersMap(headersMap),
    m_IncludeDirs(incDirs)
{
    Create();
    SetPriority(60u);
}

SystemHeadersThread::~SystemHeadersThread()
{
    TRACE(_T("SystemHeadersThread: Terminated."));
}

void* SystemHeadersThread::Entry()
{
    wxArrayString dirs;
    {
        wxCriticalSectionLocker locker(*m_SystemHeadersThreadCS);
        for (size_t i=0; i<m_IncludeDirs.GetCount(); ++i)
        {
            if (m_SystemHeadersMap.find(m_IncludeDirs[i]) == m_SystemHeadersMap.end())
            {
                dirs.Add(m_IncludeDirs[i]);
                m_SystemHeadersMap[m_IncludeDirs[i]] = StringSet();
            }
        }
    }
    // collect header files in each dir, this is done by HeaderDirTraverser
    for (size_t i=0; i<dirs.GetCount(); ++i)
    {
        if ( TestDestroy() )
            break;

        // check the dir is ready for traversing
        wxDir dir(dirs[i]);
        if ( !dir.IsOpened() )
        {
            CodeBlocksThreadEvent evt(wxEVT_COMMAND_MENU_SELECTED, idSystemHeadersThreadError);
            evt.SetClientData(this);
            evt.SetString(wxString::Format(_T("SystemHeadersThread: Unable to open: %s"), dirs[i].wx_str()));
            wxPostEvent(m_Parent, evt);
            continue;
        }

        TRACE(_T("SystemHeadersThread: Launching dir traverser for: %s"), dirs[i].wx_str());

        HeaderDirTraverser traverser(this, m_SystemHeadersThreadCS, m_SystemHeadersMap, dirs[i]);
        dir.Traverse(traverser, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

        TRACE(_T("SystemHeadersThread: Dir traverser finished for: %s"), dirs[i].wx_str());

        if ( TestDestroy() )
            break;

        CodeBlocksThreadEvent evt(wxEVT_COMMAND_MENU_SELECTED, idSystemHeadersThreadUpdate);
        evt.SetClientData(this);
        evt.SetString(wxString::Format(_T("SystemHeadersThread: %s , %lu"), dirs[i].wx_str(), static_cast<unsigned long>(m_SystemHeadersMap[dirs[i]].size())));
        wxPostEvent(m_Parent, evt);
    }

    if ( !TestDestroy() )
    {
        CodeBlocksThreadEvent evt(wxEVT_COMMAND_MENU_SELECTED, idSystemHeadersThreadFinish);
        evt.SetClientData(this);
        if (!dirs.IsEmpty())
            evt.SetString(wxString::Format(_T("SystemHeadersThread: Total number of paths: %lu"), static_cast<unsigned long>(dirs.GetCount())));
        wxPostEvent(m_Parent, evt);
    }

    TRACE(_T("SystemHeadersThread: Done."));

    return NULL;
}

// class HeaderDirTraverser

HeaderDirTraverser::HeaderDirTraverser(wxThread*          thread,
                                       wxCriticalSection* critSect,
                                       SystemHeadersMap&  headersMap,
                                       const              wxString& searchDir) :
    m_Thread(thread),
    m_SystemHeadersThreadCS(critSect),
    m_SystemHeadersMap(headersMap),
    m_SearchDir(searchDir),
    m_Headers(headersMap[searchDir]),
    m_Locked(false),
    m_Dirs(0),
    m_Files(0)
{
}

HeaderDirTraverser::~HeaderDirTraverser()
{
    if (m_Locked)
         m_SystemHeadersThreadCS->Leave();
}

wxDirTraverseResult HeaderDirTraverser::OnFile(const wxString& filename)
{
    // HeaderDirTraverser is used in a worker thread, so call TestDestroy() as often as it can to
    // quickly terminate the thread
    if (m_Thread->TestDestroy())
        return wxDIR_STOP;

    AddLock(true); // true means we are adding a file

    wxFileName fn(filename);
    if (!fn.HasExt() || fn.GetExt().GetChar(0) == _T('h'))
    {
        fn.MakeRelativeTo(m_SearchDir);
        wxString header(fn.GetFullPath());
        header.Replace(_T("\\"), _T("/"), true); // Unix style
        m_Headers.insert(header);
    }

    return wxDIR_CONTINUE;
}

bool ResolveSymLinkedPath(wxString& dirname)
{
#ifdef _WIN32
    return false;
#else
    if (dirname.Last() == wxFILE_SEP_PATH)
        dirname.RemoveLast();

    struct stat fileStats;
    if (lstat(dirname.mb_str(wxConvUTF8), &fileStats) != 0)
        return wxDIR_IGNORE;

    // If the path is a symbolic link, then try to resolve it.
    // This is needed to prevent infinite loops, when a folder is pointing to itself or its parent folder.
    if (S_ISLNK(fileStats.st_mode))
    {
        char buffer[4096];
        int result = readlink(dirname.mb_str(wxConvUTF8), buffer, WXSIZEOF(buffer) - 1);
        if (result != -1)
        {
            buffer[result] = '\0'; // readlink() doesn't NUL-terminate the buffer
            wxString pathStr(buffer, wxConvUTF8);
            wxFileName fileName = wxFileName::DirName(pathStr);

            // If this is a relative symbolic link, we need to make it absolute.
            if (!fileName.IsAbsolute())
            {
                wxFileName dirNamePath;
                if (dirname.Last() == wxFILE_SEP_PATH)
                    dirNamePath = wxFileName::DirName(dirname);
                else
                    dirNamePath = wxFileName::DirName(dirname + wxFILE_SEP_PATH);
                dirNamePath.RemoveLastDir();
                // Make the new filename absolute relative to the parent folder.
                fileName.MakeAbsolute(dirNamePath.GetFullPath());
            }

            wxString fullPath = fileName.GetFullPath();
            if (fullPath.Last() == wxT('.')) // this case should be handled because of a bug in wxWidgets
                fullPath.RemoveLast();
            if (fullPath.Last() == wxFILE_SEP_PATH)
                fullPath.RemoveLast();
            dirname = fullPath;
            return true;
        }
    }

    return false;
#endif // _WIN32
}

wxDirTraverseResult HeaderDirTraverser::OnDir(const wxString& dirname)
{
    // HeaderDirTraverser is used in a worker thread, so call TestDestroy() as often as it can to
    // quickly terminate the thread
    if (m_Thread->TestDestroy())
        return wxDIR_STOP;

    AddLock(false); // false means we are adding a dir

    wxString path = dirname;
    while (ResolveSymLinkedPath(path))
        ;

    if (path.Last() != wxFILE_SEP_PATH)
        path.Append(wxFILE_SEP_PATH);

    return GetStatus(path);
}

void HeaderDirTraverser::AddLock(bool is_file)
{
    if (is_file)
        m_Files++;
    else
        m_Dirs++;

    if ((m_Files+m_Dirs) % 100 == 1)
    {
        TRACE(_T("HeaderDirTraverser: %lu directories and %lu files traversed. Unlocking temporarily."), static_cast<unsigned long>(m_Dirs), static_cast<unsigned long>(m_Files));

        if (m_Locked)
        {
            m_SystemHeadersThreadCS->Leave();
            m_Locked = false;
        }

        m_SystemHeadersThreadCS->Enter();
        m_Locked = true;
    }
}

wxDirTraverseResult HeaderDirTraverser::GetStatus(const wxString &path)
{
    if (m_SystemHeadersMap.find(path) != m_SystemHeadersMap.end())
        return wxDIR_IGNORE;
#ifndef _WIN32
    if (m_VisitedDirs.find(path) != m_VisitedDirs.end())
        return wxDIR_IGNORE;
    m_VisitedDirs.insert(path);
#endif // _WIN32
    return wxDIR_CONTINUE;
}
