/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Copyright: 2010 Jens Lody
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/menu.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <logmanager.h>
    #include <cbeditor.h>
    #include <cbproject.h>
#endif

#include "ReopenEditor.h"
#include "ReopenEditorConfDLg.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<ReopenEditor> reg(_T("ReopenEditor"));
    const int idReopenEditor = wxNewId();
    const int idReopenEditorView = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(ReopenEditor, cbPlugin)
    EVT_UPDATE_UI(idReopenEditorView, ReopenEditor::OnUpdateUI)
    EVT_MENU(idReopenEditor, ReopenEditor::OnReopenEditor)
    EVT_MENU(idReopenEditorView, ReopenEditor::OnViewList)
END_EVENT_TABLE()

// constructor
ReopenEditor::ReopenEditor()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("ReopenEditor.zip")))
    {
        NotifyMissingFile(_T("ReopenEditor.zip"));
    }
}

// destructor
ReopenEditor::~ReopenEditor()
{
}

void ReopenEditor::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<ReopenEditor, CodeBlocksEvent>(this, &ReopenEditor::OnProjectClosed));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<ReopenEditor, CodeBlocksEvent>(this, &ReopenEditor::OnProjectOpened));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<ReopenEditor, CodeBlocksEvent>(this, &ReopenEditor::OnEditorClosed));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<ReopenEditor, CodeBlocksEvent>(this, &ReopenEditor::OnEditorOpened));

    wxArrayString titles;
    wxArrayInt widths;
    titles.Add(_("Editorfile"));
    titles.Add(_("Project"));
    titles.Add(_("Projectfile"));
    widths.Add(350);
    widths.Add(100);
    widths.Add(350);

    m_pListLog = new ReopenEditorListView(titles, widths);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    m_IsManaged = cfg->ReadBool(_T("/reopen_editor/managed"),true);

    ShowList();
}

void ReopenEditor::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
    if(Manager::Get()->GetLogManager() && m_pListLog)
    {
        if(m_IsManaged)
        {
            CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pListLog);
            Manager::Get()->ProcessEvent(evt);
        }
        else
        {
            CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
            evt.pWindow = m_pListLog;
            Manager::Get()->ProcessEvent(evt);
        }
//        m_pListLog->Destroy();
    }
    m_pListLog = nullptr;
}

cbConfigurationPanel* ReopenEditor::GetConfigurationPanel(wxWindow* parent)
{
    if ( !IsAttached() )
        return NULL;

    ReopenEditorConfDLg* cfg = new ReopenEditorConfDLg(parent);
    return cfg;
}

void ReopenEditor::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    if (!m_IsAttached || !menuBar)
    {
        return;
    }
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* menu = menuBar->GetMenu(idx);
        wxMenuItemList& items = menu->GetMenuItems();
        size_t i = 0;
        for (i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                break;
            }
        }
        // if not found, just append with seperator
        if(i == items.GetCount())
        {
            menu->AppendCheckItem(idReopenEditorView, _("Closed file list"), _("Toggle displaying the closed file list"));
        }
        else
        {
            menu->InsertCheckItem(i, idReopenEditorView, _("Closed file list"), _("Toggle displaying the closed file list"));
        }

        for (i = 0; i < items.GetCount(); ++i)
        {
#if wxCHECK_VERSION(2,8,5)
            if (items[i]->GetLabelText(items[i]->GetItemLabelText()) == _("Focus editor"))
#else
            if (items[i]->GetLabelFromText(items[i]->GetLabel()) == _("Focus editor"))
#endif

            {
                ++i;
                break;
            }
        }
        // if not found, just append with seperator
        if(i == items.GetCount())
        {
            menu->InsertSeparator(i++);
        }
        menu->Insert(i, idReopenEditor, _("&Reopen last closed editor\tCtrl-Shift-T"), _("Reopens the last closed editor"));
        menuBar->Enable(idReopenEditor, (m_pListLog->GetItemsCount() > 0));
    }
}

void ReopenEditor::OnReopenEditor(wxCommandEvent& event)
{
    if(m_pListLog->GetItemsCount() > 0)
    {
        EditorManager* em = Manager::Get()->GetEditorManager();
        wxString fname = m_pListLog->GetFilename(0);
        if(!fname.IsEmpty() && !em->IsOpen(fname))
        {
            em->Open(fname);
        }
    }
}

void ReopenEditor::OnEditorClosed(CodeBlocksEvent& event)
{
    EditorBase* eb = event.GetEditor();

    if(eb && eb->IsBuiltinEditor())
    {
        cbProject* prj = nullptr;
        bool isPrjClosing = false;

        ProjectFile* prjf = ((cbEditor*)eb)->GetProjectFile();
        if(prjf)
            prj = prjf->GetParentProject();

        wxString name = wxEmptyString;
        if(prj)
        {
            isPrjClosing = (m_ClosedProjects.Index(prj) != wxNOT_FOUND);
            name = prj->GetTitle();
        }
        if(!prj || (prj && !isPrjClosing))
        {
            wxArrayString list;
            list.Add(eb->GetFilename());
            if(prj)
            {
                list.Add(prj->GetTitle());
                list.Add(prj->GetFilename());
            }
            else
            {
                list.Add(_("<none>"));
                list.Add(_("<none>"));
            }
            m_pListLog->Prepend(list);
            m_pListLog->SetProject(0, prj);
        }
    }
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->Enable(idReopenEditor, (m_pListLog->GetItemsCount() > 0));
    event.Skip();
}

void ReopenEditor::OnEditorOpened(CodeBlocksEvent& event)
{
    if(m_pListLog->GetItemsCount() > 0)
    {
        EditorBase* eb = event.GetEditor();

        if(eb && eb->IsBuiltinEditor())
        {
            wxString fname = eb->GetFilename();
            for(size_t i = m_pListLog->GetItemsCount(); i > 0; --i)
            {
                if(fname == m_pListLog->GetFilename(i-1))
                {
                    m_pListLog->RemoveAt(i-1);
                    break;
                }
            }
        }
    }
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->Enable(idReopenEditor, (m_pListLog->GetItemsCount() > 0));
    event.Skip();
}

void ReopenEditor::OnProjectOpened(CodeBlocksEvent& event)
{
    cbProject* prj = event.GetProject();
    int index = m_ClosedProjects.Index(prj);
    if(index != wxNOT_FOUND)
        m_ClosedProjects.RemoveAt(index);
    event.Skip();
}

void ReopenEditor::OnProjectClosed(CodeBlocksEvent& event)
{
    cbProject* prj = event.GetProject();
    if(prj)
    {
        m_ClosedProjects.Add(prj);
        for(int i = m_pListLog->GetItemsCount() - 1; i >= 0; --i)
        {
            if(m_pListLog->GetProject(i) == prj)
            {
                m_pListLog->RemoveAt(i);
            }
        }
    }
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->Enable(idReopenEditor, (m_pListLog->GetItemsCount() > 0));
    event.Skip();
}

void ReopenEditor::OnViewList(wxCommandEvent& event)
{
    if(m_IsManaged)
    {
        if(event.IsChecked())
        {
                CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
                Manager::Get()->ProcessEvent(evtShow);
                CodeBlocksLogEvent event(cbEVT_SWITCH_TO_LOG_WINDOW, m_pListLog);
                Manager::Get()->ProcessEvent(event);
        }
        else
        {
            CodeBlocksLogEvent event(cbEVT_HIDE_LOG_WINDOW, m_pListLog);
            Manager::Get()->ProcessEvent(event);
        }
    }
    else
    {
        CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
        evt.pWindow = m_pListLog;
        Manager::Get()->ProcessEvent(evt);
    }
}

void ReopenEditor::OnUpdateUI(wxUpdateUIEvent& event)
{
    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idReopenEditorView, IsWindowReallyShown(m_pListLog));
}

void ReopenEditor::SetManaged(bool managed)
{
    m_IsManaged = managed;
}

void ReopenEditor::ShowList()
{
    CodeBlocksLogEvent evt1(cbEVT_REMOVE_LOG_WINDOW, m_pListLog);
    Manager::Get()->ProcessEvent(evt1);

    CodeBlocksDockEvent evt2(cbEVT_REMOVE_DOCK_WINDOW);
    evt2.pWindow = m_pListLog;
    Manager::Get()->ProcessEvent(evt2);

    if(m_IsManaged)
    {
        wxString prefix = ConfigManager::GetDataFolder() + _T("/images/16x16/");
        wxBitmap * bmp = new wxBitmap(cbLoadBitmap(prefix + _T("undo.png"), wxBITMAP_TYPE_PNG));

        CodeBlocksLogEvent evt1(cbEVT_ADD_LOG_WINDOW, m_pListLog, _("Closed files list"), bmp);
        Manager::Get()->ProcessEvent(evt1);
        CodeBlocksLogEvent evt2(cbEVT_SWITCH_TO_LOG_WINDOW, m_pListLog);
        Manager::Get()->ProcessEvent(evt2);
    }
    else
    {
        m_pListLog->Reparent(Manager::Get()->GetAppFrame());
        m_pListLog->SetSize(wxSize(800,94));
        m_pListLog->SetInitialSize(wxSize(800,94));

        CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
        evt.name = _T("ReopenEditorListPane");
        evt.title = _("Closed file list");
        evt.pWindow = m_pListLog;
        evt.dockSide = CodeBlocksDockEvent::dsBottom;
        evt.shown = true;
        evt.hideable = true;
        evt.desiredSize.Set(800, 94);
        evt.floatingSize.Set(800, 94);
        evt.minimumSize.Set(350, 94);
        Manager::Get()->ProcessEvent(evt);
    }

}
