/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/fs_zip.h>
    #include <wx/menu.h>

    #include "manager.h" // class's header file
    #include "sdk_events.h"
    #include "cbexception.h"
    #include "projectmanager.h"
    #include "editormanager.h"
    #include "logmanager.h"
    #include "pluginmanager.h"
    #include "toolsmanager.h"
    #include "macrosmanager.h"
    #include "configmanager.h"
    #include "scriptingmanager.h"
    #include "templatemanager.h"
    #include "personalitymanager.h"
    #include "uservarmanager.h"
    #include "filemanager.h"
    #include "globals.h"
    #include "xtra_res.h" // our new ToolBarAddOn handler
#endif

#include <wx/app.h>    // wxTheApp
#include <wx/toolbar.h>
#include <wx/fs_mem.h>

#include "debuggermanager.h"

static Manager* s_ManagerInstance = 0;

Manager::Manager() : m_pAppWindow(0)
{
}

Manager::~Manager()
{
    // remove all event sinks
    for (EventSinksMap::iterator mit = m_EventSinks.begin(); mit != m_EventSinks.end(); ++mit)
    {
        while (mit->second.size())
        {
            delete (*(mit->second.begin()));
            mit->second.erase(mit->second.begin());
        }
    }

    for (DockEventSinksMap::iterator mit = m_DockEventSinks.begin(); mit != m_DockEventSinks.end(); ++mit)
    {
        while (mit->second.size())
        {
            delete (*(mit->second.begin()));
            mit->second.erase(mit->second.begin());
        }
    }

    for (LayoutEventSinksMap::iterator mit = m_LayoutEventSinks.begin(); mit != m_LayoutEventSinks.end(); ++mit)
    {
        while (mit->second.size())
        {
            delete (*(mit->second.begin()));
            mit->second.erase(mit->second.begin());
        }
    }

    for (LogEventSinksMap::iterator mit = m_LogEventSinks.begin(); mit != m_LogEventSinks.end(); ++mit)
    {
        while (mit->second.size())
        {
            delete (*(mit->second.begin()));
            mit->second.erase(mit->second.begin());
        }
    }

//    Shutdown();
    CfgMgrBldr::Free(); // only terminate config at the very last moment
//    FileManager::Free();
}


Manager* Manager::Get(wxFrame *appWindow)
{
    if (appWindow)
    {
        if (Get()->m_pAppWindow)
            cbThrow(_T("Illegal argument to Manager::Get()"));
        else
        {
            Get()->m_pAppWindow = appWindow;
            LoadResource(_T("manager_resources.zip"));
            Get()->GetLogManager()->Log(_("Manager initialized"));
        }
    }
    return Get();
}

Manager* Manager::Get()
{
    if (!s_ManagerInstance)
        s_ManagerInstance = new Manager;
    return s_ManagerInstance;
}

void Manager::Free()
{
    delete s_ManagerInstance;
    s_ManagerInstance = 0;
}

void Manager::SetAppStartedUp(bool app_started_up)
{
    m_AppStartedUp = app_started_up;
}

void Manager::SetAppShuttingDown(bool app_shutting_down)
{
    m_AppShuttingDown = app_shutting_down;
}

void Manager::SetBatchBuild(bool is_batch)
{
    m_IsBatch = is_batch;
}

void Manager::BlockYields(bool block)
{
    m_BlockYields = block;
}

void Manager::ProcessPendingEvents()
{
    if (!m_BlockYields && !m_AppShuttingDown)
        wxTheApp->ProcessPendingEvents();
}

void Manager::Yield()
{
    if (!m_BlockYields && !m_AppShuttingDown)
        wxTheApp->Yield(true);
}

void Manager::Shutdown()
{
    m_AppShuttingDown = true;

    ToolsManager::Free();
    TemplateManager::Free();
    PluginManager::Free();
    ScriptingManager::Free();
    ProjectManager::Free();
    EditorManager::Free();
    PersonalityManager::Free();
    MacrosManager::Free();
    UserVariableManager::Free();
    LogManager::Free();
}

bool Manager::ProcessEvent(CodeBlocksEvent& event)
{
    if (IsAppShuttingDown())
        return false;

    EventSinksMap::iterator mit = m_EventSinks.find(event.GetEventType());
    if (mit != m_EventSinks.end())
    {
        for (EventSinksArray::iterator it = mit->second.begin(); it != mit->second.end(); ++it)
            (*it)->Call(event);
    }
    return true;
}

bool Manager::ProcessEvent(CodeBlocksDockEvent& event)
{
    if (IsAppShuttingDown())
        return false;

    DockEventSinksMap::iterator mit = m_DockEventSinks.find(event.GetEventType());
    if (mit != m_DockEventSinks.end())
    {
        for (DockEventSinksArray::iterator it = mit->second.begin(); it != mit->second.end(); ++it)
            (*it)->Call(event);
    }
    return true;
}

bool Manager::ProcessEvent(CodeBlocksLayoutEvent& event)
{
    if (IsAppShuttingDown())
        return false;

    LayoutEventSinksMap::iterator mit = m_LayoutEventSinks.find(event.GetEventType());
    if (mit != m_LayoutEventSinks.end())
    {
        for (LayoutEventSinksArray::iterator it = mit->second.begin(); it != mit->second.end(); ++it)
            (*it)->Call(event);
    }
    return true;
}

bool Manager::ProcessEvent(CodeBlocksLogEvent& event)
{
    if (IsAppShuttingDown())
        return false;

    LogEventSinksMap::iterator mit = m_LogEventSinks.find(event.GetEventType());
    if (mit != m_LogEventSinks.end())
    {
        for (LogEventSinksArray::iterator it = mit->second.begin(); it != mit->second.end(); ++it)
            (*it)->Call(event);
    }
    return true;
}

bool Manager::IsAppShuttingDown()
{
    return m_AppShuttingDown;
}

bool Manager::IsAppStartedUp()
{
    return m_AppStartedUp;
}

void Manager::InitXRC(bool force)
{
    static bool xrcok = false;
    if (!xrcok || force)
    {
        wxFileSystem::AddHandler(new wxZipFSHandler);
        wxXmlResource::Get()->InsertHandler(new wxToolBarAddOnXmlHandler);
        wxXmlResource::Get()->InitAllHandlers();

        xrcok = true;
    }
}

void Manager::LoadXRC(wxString relpath)
{
    LoadResource(relpath);
}

wxMenuBar *Manager::LoadMenuBar(wxString resid,bool createonfailure)
{
    wxMenuBar *m = wxXmlResource::Get()->LoadMenuBar(resid);
    if (!m && createonfailure) m = new wxMenuBar();
    return m;
}

wxMenu *Manager::LoadMenu(wxString menu_id,bool createonfailure)
{
    wxMenu *m = wxXmlResource::Get()->LoadMenu(menu_id);
    if (!m && createonfailure) m = new wxMenu(_T(""));
    return m;
}

wxToolBar *Manager::LoadToolBar(wxFrame *parent,wxString resid,bool defaultsmall)
{
    if (!parent)
        return 0L;
    wxToolBar *tb = wxXmlResource::Get()->LoadToolBar(parent,resid);
    if (!tb)
    {
        int flags = wxTB_HORIZONTAL;

        if (platform::WindowsVersion() < platform::winver_WindowsXP)
            flags |= wxTB_FLAT;

        tb = parent->CreateToolBar(flags, wxID_ANY);
        tb->SetToolBitmapSize(defaultsmall ? wxSize(16, 16) : wxSize(22, 22));
    }

    return tb;
}

wxToolBar* Manager::CreateEmptyToolbar()
{
    bool smallToolBar = Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/toolbar_size"), true);

    wxSize size = smallToolBar ? wxSize(16, 16) : (platform::macosx ? wxSize(32, 32) : wxSize(22, 22));
    wxToolBar* toolbar = new wxToolBar(GetAppFrame(), -1, wxDefaultPosition, size, wxTB_FLAT | wxTB_NODIVIDER);
    toolbar->SetToolBitmapSize(size);

    return toolbar;
}

void Manager::AddonToolBar(wxToolBar* toolBar,wxString resid)
{
    if (!toolBar)
        return;
    wxXmlResource::Get()->LoadObject(toolBar,NULL,resid,_T("wxToolBarAddOn"));
}

bool Manager::isToolBar16x16(wxToolBar* toolBar)
{
    if (!toolBar) return true; // Small by default
    wxSize mysize=toolBar->GetToolBitmapSize();
    return (mysize.GetWidth()<=16 && mysize.GetHeight()<=16);
}

wxFrame* Manager::GetAppFrame() const
{
    return m_pAppWindow;
}

wxWindow* Manager::GetAppWindow() const
{
    return (wxWindow*)m_pAppWindow;
}

ProjectManager* Manager::GetProjectManager() const
{
    return ProjectManager::Get();
}

EditorManager* Manager::GetEditorManager() const
{
    return EditorManager::Get();
}

LogManager* Manager::GetLogManager() const
{
    return LogManager::Get();
}

PluginManager* Manager::GetPluginManager() const
{
    return PluginManager::Get();
}

ToolsManager* Manager::GetToolsManager() const
{
    return ToolsManager::Get();
}

MacrosManager* Manager::GetMacrosManager() const
{
    return MacrosManager::Get();
}

PersonalityManager* Manager::GetPersonalityManager() const
{
    return PersonalityManager::Get();
}

UserVariableManager* Manager::GetUserVariableManager() const
{
    return UserVariableManager::Get();
}

ScriptingManager* Manager::GetScriptingManager() const
{
    return ScriptingManager::Get();
}

ConfigManager* Manager::GetConfigManager(const wxString& name_space) const
{
    return CfgMgrBldr::GetConfigManager(name_space);
}

FileManager* Manager::GetFileManager() const
{
    return FileManager::Get();
}

DebuggerManager* Manager::GetDebuggerManager() const
{
    return DebuggerManager::Get();
}

bool Manager::LoadResource(const wxString& file)
{
    wxString resourceFile = ConfigManager::LocateDataFile(file, sdDataGlobal | sdDataUser);
    wxString memoryFile = _T("memory:") + file;

    if (wxFile::Access(resourceFile, wxFile::read) == false)
        return false;

    // The code below forces a reload of the resource
    // Currently unused...

//    {
//        wxMemoryFSHandler::RemoveFile(file);
//    }
//#if wxABI_VERSION > 20601
//    // unload old resources with the same name
//    wxXmlResource::Get()->Unload(memoryFile);
//#endif

    wxFile f(resourceFile, wxFile::read);
    char *buf = 0;

    try
    {
        size_t len = f.Length();
        buf = new char[len];
        f.Read(buf, len);
        {
            wxMemoryFSHandler::AddFile(file, buf, len);
        }
        wxXmlResource::Get()->Load(memoryFile);
        delete[] buf;
        return true;
    }
    catch (...)
    {
        delete[] buf;
        return false;
    }
}

wxCmdLineParser* Manager::GetCmdLineParser()
{
    return &m_CmdLineParser;
}

void Manager::RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksEvent>* functor)
{
    m_EventSinks[eventType].push_back(functor);
}

void Manager::RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksDockEvent>* functor)
{
    m_DockEventSinks[eventType].push_back(functor);
}

void Manager::RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksLayoutEvent>* functor)
{
    m_LayoutEventSinks[eventType].push_back(functor);
}

void Manager::RegisterEventSink(wxEventType eventType, IEventFunctorBase<CodeBlocksLogEvent>* functor)
{
    m_LogEventSinks[eventType].push_back(functor);
}

void Manager::RemoveAllEventSinksFor(void* owner)
{
    for (EventSinksMap::iterator mit = m_EventSinks.begin(); mit != m_EventSinks.end(); ++mit)
    {
        EventSinksArray::iterator it = mit->second.begin();
        bool endIsInvalid = false;
        while (!endIsInvalid && it != mit->second.end())
        {
            if ((*it) && (*it)->GetThis() == owner)
            {
                EventSinksArray::iterator it2 = it++;
                endIsInvalid = it == mit->second.end();
                delete (*it2);
                mit->second.erase(it2);
            }
            else
                ++it;
        }
    }

    for (DockEventSinksMap::iterator mit = m_DockEventSinks.begin(); mit != m_DockEventSinks.end(); ++mit)
    {
        DockEventSinksArray::iterator it = mit->second.begin();
        bool endIsInvalid = false;
        while (!endIsInvalid && it != mit->second.end())
        {
            if ((*it) && (*it)->GetThis() == owner)
            {
                DockEventSinksArray::iterator it2 = it++;
                endIsInvalid = it == mit->second.end();
                delete (*it2);
                mit->second.erase(it2);
            }
            else
                ++it;
        }
    }

    for (LayoutEventSinksMap::iterator mit = m_LayoutEventSinks.begin(); mit != m_LayoutEventSinks.end(); ++mit)
    {
        LayoutEventSinksArray::iterator it = mit->second.begin();
        bool endIsInvalid = false;
        while (!endIsInvalid && it != mit->second.end())
        {
            if ((*it) && (*it)->GetThis() == owner)
            {
                LayoutEventSinksArray::iterator it2 = it++;
                endIsInvalid = it == mit->second.end();
                delete (*it2);
                mit->second.erase(it2);
            }
            else
                ++it;
        }
    }

    for (LogEventSinksMap::iterator mit = m_LogEventSinks.begin(); mit != m_LogEventSinks.end(); ++mit)
    {
        LogEventSinksArray::iterator it = mit->second.begin();
        bool endIsInvalid = false;
        while (!endIsInvalid && it != mit->second.end())
        {
            if ((*it) && (*it)->GetThis() == owner)
            {
                LogEventSinksArray::iterator it2 = it++;
                endIsInvalid = it == mit->second.end();
                delete (*it2);
                mit->second.erase(it2);
            }
            else
                ++it;
        }
    }
}

bool            Manager::m_AppShuttingDown = false;
bool            Manager::m_AppStartedUp    = false;
bool            Manager::m_BlockYields     = false;
bool            Manager::m_IsBatch         = false;
wxCmdLineParser Manager::m_CmdLineParser;
