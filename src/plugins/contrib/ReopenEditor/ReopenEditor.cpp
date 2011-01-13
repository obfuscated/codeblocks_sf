/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Copyright: 2010 Jens Lody
 *
 * $Revision:$
 * $Id:$
 * $HeadURL:$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/menu.h>
    #include <editormanager.h>
    #include <cbeditor.h>
    #include <cbproject.h>
#endif

#include "ReopenEditor.h"
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ReopenEditorArray);

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<ReopenEditor> reg(_T("ReopenEditor"));
    int idReopenEditor = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(ReopenEditor, cbPlugin)
    EVT_MENU(idReopenEditor, ReopenEditor::OnReopenEditor)
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
}

void ReopenEditor::OnRelease(bool /*appShutDown*/)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
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
        wxMenuItem* itemTmp = new wxMenuItem(   menu,
                                                idReopenEditor,
                                                _("&Reopen last closed editor\tCtrl-Shift-T"),
                                                _("Reopens the last closed editor") );

        size_t i = 0;
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
        menu->Insert(i, itemTmp);
        menuBar->Enable(idReopenEditor, (m_PrjEditorList.GetCount() > 0));
    }
}

void ReopenEditor::OnReopenEditor(wxCommandEvent& event)
{
    if(m_PrjEditorList.GetCount() > 0)
    {
        PrjEditor pe = m_PrjEditorList.Last();
        EditorManager* em = Manager::Get()->GetEditorManager();
        if(!em->IsOpen(pe.fname))
        {
            cbEditor* ed = em->Open(pe.fname);
        }
        m_PrjEditorList.RemoveAt(m_PrjEditorList.GetCount()-1);
    }
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->Enable(idReopenEditor, (m_PrjEditorList.GetCount() > 0));
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
            PrjEditor pe(prj, eb->GetFilename());
            for(size_t i = m_PrjEditorList.GetCount(); i > 0; --i)
            {
                if(m_PrjEditorList[i-1] == pe)
                {
                    m_PrjEditorList.RemoveAt(i-1);
                    break;
                }
            }
            m_PrjEditorList.Add(pe);
        }
    }
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->Enable(idReopenEditor, (m_PrjEditorList.GetCount() > 0));
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
        PrjEditor pe(prj, wxEmptyString);
        m_ClosedProjects.Add(prj);
        for(size_t i = m_PrjEditorList.GetCount(); i > 0; --i)
        {
            if(m_PrjEditorList[i-1] == pe)
            {
                m_PrjEditorList.RemoveAt(i-1);
            }
        }
    }
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->Enable(idReopenEditor, (m_PrjEditorList.GetCount() > 0));
    event.Skip();
}
