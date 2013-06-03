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

// internal class declaration of HeaderDirTraverser (implementation below)

class HeaderDirTraverser : public wxDirTraverser
{
public:
    HeaderDirTraverser(wxThread* thread, wxCriticalSection* critSect,
                       SystemHeadersMap& headersMap, const wxString& searchDir);
    virtual ~HeaderDirTraverser();
    virtual wxDirTraverseResult OnFile(const wxString& filename);
    virtual wxDirTraverseResult OnDir(const wxString& dirname);
    void AddLock(bool is_file);

private:
    /* the thread call Traverse() on this instance*/
    wxThread*               m_Thread;
    /* critical section to protect accessing m_SystemHeadersMap */
    wxCriticalSection*      m_SystemHeadersThreadCS;
    /* dir to files map, for example, you are two dirs c:/a and c:/b
     * so the map looks like:
     * c:/a  ---> {c:/a/a1.h, c:/a/a2.h}
     * c:/b  ---> {c:/b/b1.h, c:/b/b2.h}
     */
    const SystemHeadersMap& m_SystemHeadersMap;
    /* which dir we are traversing header files */
    const wxString&         m_SearchDir;
    /* string set for header files */
    StringSet&              m_Headers;
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
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, SystemHeadersThreadHelper::idSystemHeadersThreadError);
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

        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, SystemHeadersThreadHelper::idSystemHeadersThreadUpdate);
        evt.SetClientData(this);
        evt.SetString(wxString::Format(_T("SystemHeadersThread: %s , %lu"), dirs[i].wx_str(), static_cast<unsigned long>(m_SystemHeadersMap[dirs[i]].size())));
        wxPostEvent(m_Parent, evt);
    }

    if ( !TestDestroy() )
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, SystemHeadersThreadHelper::idSystemHeadersThreadCompleted);
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

    AddLock(true);

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

wxDirTraverseResult HeaderDirTraverser::OnDir(const wxString& dirname)
{
    // HeaderDirTraverser is used in a worker thread, so call TestDestroy() as often as it can to
    // quickly terminate the thread
    if (m_Thread->TestDestroy())
        return wxDIR_STOP;

    AddLock(false);

    wxString path(dirname);
    if (path.Last() != wxFILE_SEP_PATH)
        path.Append(wxFILE_SEP_PATH);

    if (m_SystemHeadersMap.find(path) != m_SystemHeadersMap.end())
        return wxDIR_IGNORE;

    return wxDIR_CONTINUE;
}

void HeaderDirTraverser::AddLock(bool is_file)
{
    if (is_file) m_Files++; else m_Dirs++;

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
