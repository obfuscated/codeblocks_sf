/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/filename.h>
    #include <wx/notebook.h>
    #include <wx/menu.h>
    #include "manager.h"
    #include "editorbase.h"
    #include "cbeditor.h"
    #include "editormanager.h"
    #include "pluginmanager.h"
    #include "cbproject.h" // FileTreeData
    #include <wx/wfstream.h>
#endif

#include "wx/wxFlatNotebook/wxFlatNotebook.h"

// needed for initialization of variables
int editorbase_RegisterId(int id)
{
    wxRegisterId(id);
    return id;
}

struct EditorBaseInternalData
{
	EditorBaseInternalData(EditorBase* owner)
		: m_pOwner(owner),
		m_DisplayingPopupMenu(false),
		m_CloseMe(false)
	{}

	EditorBase* m_pOwner;
	bool m_DisplayingPopupMenu;
	bool m_CloseMe;
};

// The following lines reserve 255 consecutive id's
const int EditorMaxSwitchTo = 255;
const int idSwitchFile1 = wxNewId();
const int idSwitchFileMax = editorbase_RegisterId(idSwitchFile1 + EditorMaxSwitchTo -1);

const int idCloseMe = wxNewId();
const int idCloseAll = wxNewId();
const int idCloseAllOthers = wxNewId();
const int idSaveMe = wxNewId();
const int idSaveAll = wxNewId();
const int idSwitchTo = wxNewId();
const int idGoogle = wxNewId();
const int idGoogleCode = wxNewId();
const int idMsdn = wxNewId();

BEGIN_EVENT_TABLE(EditorBase, wxPanel)
    EVT_MENU_RANGE(idSwitchFile1, idSwitchFileMax,EditorBase::OnContextMenuEntry)
    EVT_MENU(idCloseMe, EditorBase::OnContextMenuEntry)
    EVT_MENU(idCloseAll, EditorBase::OnContextMenuEntry)
    EVT_MENU(idCloseAllOthers, EditorBase::OnContextMenuEntry)
    EVT_MENU(idSaveMe, EditorBase::OnContextMenuEntry)
    EVT_MENU(idSaveAll, EditorBase::OnContextMenuEntry)
    EVT_MENU(idGoogle, EditorBase::OnContextMenuEntry)
    EVT_MENU(idGoogleCode, EditorBase::OnContextMenuEntry)
    EVT_MENU(idMsdn, EditorBase::OnContextMenuEntry)
END_EVENT_TABLE()

void EditorBase::InitFilename(const wxString& filename)
{
    if (filename.IsEmpty())
        m_Filename = CreateUniqueFilename();
    else
        m_Filename = filename;

    wxFileName fname;
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
    //    Manager::Get()->GetLogManager()->DebugLog("ctor: Filename=%s\nShort=%s", m_Filename.c_str(), m_Shortname.c_str());
}

wxString EditorBase::CreateUniqueFilename()
{
    const wxString prefix = _("Untitled");
    const wxString path = wxGetCwd() + wxFILE_SEP_PATH;
    wxString tmp;
    int iter = 0;
    while (true)
    {
        tmp.Clear();
        tmp << path << prefix << wxString::Format(_T("%d"), iter);
        if (!Manager::Get()->GetEditorManager()->GetEditor(tmp) &&
                !wxFileExists(path + tmp))
        {
            return tmp;
        }
        ++iter;
    }
}

EditorBase::EditorBase(wxWindow* parent, const wxString& filename)
        : wxPanel(parent, -1),
        m_IsBuiltinEditor(false),
        m_Shortname(_T("")),
        m_Filename(_T("")),
        m_WinTitle(filename)
{
	m_pData = new EditorBaseInternalData(this);

    Manager::Get()->GetEditorManager()->AddCustomEditor(this);
    InitFilename(filename);
    SetTitle(m_Shortname);
}

EditorBase::~EditorBase()
{
    if (Manager::Get()->GetPluginManager())
    {
        CodeBlocksEvent event(cbEVT_EDITOR_CLOSE);
        event.SetEditor(this);
        event.SetString(m_Filename);

        Manager::Get()->GetPluginManager()->NotifyPlugins(event);
    }

    if (Manager::Get()->GetEditorManager()) // sanity check
        Manager::Get()->GetEditorManager()->RemoveCustomEditor(this);

	delete m_pData;
}

const wxString& EditorBase::GetTitle()
{
    return m_WinTitle;
}

void EditorBase::SetTitle(const wxString& newTitle)
{
    m_WinTitle = newTitle;
    int mypage = Manager::Get()->GetEditorManager()->FindPageFromEditor(this);
    if (mypage != -1)
        Manager::Get()->GetEditorManager()->GetNotebook()->SetPageText(mypage, newTitle);
}

void EditorBase::Activate()
{
    Manager::Get()->GetEditorManager()->SetActiveEditor(this);
}

bool EditorBase::Close()
{
    Destroy();
    return true;
}

bool EditorBase::IsBuiltinEditor() const
{
    return m_IsBuiltinEditor;
}

bool EditorBase::ThereAreOthers() const
{
    bool hasOthers = false;
    hasOthers = Manager::Get()->GetEditorManager()->GetEditorsCount() > 1;
    //    for(int i = 0; i < Manager::Get()->GetEditorManager()->GetEditorsCount(); ++i)
    //    {
    //        EditorBase* other = Manager::Get()->GetEditorManager()->GetEditor(i);
    //        if (!other || other == (EditorBase*)this)
    //            continue;
    //        hasOthers = true;
    //        break;
    //    }
    return hasOthers;
}

wxMenu* EditorBase::CreateContextSubMenu(int id) // For context menus
{
    wxMenu* menu = 0;

    if(id == idSwitchTo)
    {
        menu = new wxMenu;
        m_SwitchTo.clear();
        for (int i = 0; i < EditorMaxSwitchTo && i < Manager::Get()->GetEditorManager()->GetEditorsCount(); ++i)
        {
            EditorBase* other = Manager::Get()->GetEditorManager()->GetEditor(i);
            if (!other || other == this)
                continue;
            int id = idSwitchFile1+i;
            m_SwitchTo[id] = other;
            menu->Append(id, other->GetShortName());
        }
        if(!menu->GetMenuItemCount())
        {
            delete menu;
            menu = 0;
        }
    }
    return menu;
}

void EditorBase::BasicAddToContextMenu(wxMenu* popup,ModuleType type)   //pecan 2006/03/22
{
    bool noeditor = (type != mtEditorManager);                          //pecan 2006/03/22
    if (type == mtOpenFilesList)                                        //pecan 2006/03/22
    {
      popup->Append(idCloseMe, _("Close"));
      popup->Append(idCloseAll, _("Close all"));
      popup->Append(idCloseAllOthers, _("Close all others"));
      popup->AppendSeparator();
      popup->Append(idSaveMe, _("Save"));
      popup->Append(idSaveAll, _("Save all"));
      popup->AppendSeparator();
      // enable/disable some items, based on state
      popup->Enable(idSaveMe, GetModified());

      bool hasOthers = ThereAreOthers();
      popup->Enable(idCloseAll, hasOthers);
      popup->Enable(idCloseAllOthers, hasOthers);
    }
    if(!noeditor)
    {
        wxMenu* switchto = CreateContextSubMenu(idSwitchTo);
        if(switchto)
            popup->Append(idSwitchTo, _("Switch to"), switchto);
    }
}

void EditorBase::DisplayContextMenu(const wxPoint& position, ModuleType type)   //pecan 2006/03/22
{
    bool noeditor = (type != mtEditorManager);                                  //pecan 2006/03/22
    // noeditor:
    // True if context menu belongs to open files tree;
    // False if belongs to cbEditor

    // inform the editors we 're just about to create a context menu
    if (!OnBeforeBuildContextMenu(position, type))              //pecan 2006/03/22
        return;

    wxMenu* popup = new wxMenu;

    if(!noeditor && wxGetKeyState(WXK_CONTROL))
    {
        cbStyledTextCtrl* control = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();
        wxString text = control->GetSelectedText();
        if (text.IsEmpty())
        {
            const int pos = control->GetCurrentPos();
            text = control->GetTextRange(control->WordStartPosition(pos, true), control->WordEndPosition(pos, true));
        }

        if(wxMinimumVersion<2,6,1>::eval)
        {
            popup->Append(idGoogle, _("Search the Internet for \"") + text + _("\""));
            popup->Append(idMsdn, _("Search MSDN for \"") + text + _("\""));
            popup->Append(idGoogleCode, _("Search Google Code for \"") + text + _("\""));
        }
        lastWord = text;

        wxMenu* switchto = CreateContextSubMenu(idSwitchTo);
        if(switchto)
        {
            popup->AppendSeparator();
            popup->Append(idSwitchTo, _("Switch to"), switchto);
        }
    }
    else if(!noeditor && wxGetKeyState(WXK_ALT))
    { // run a script
    }
    else
    {
        // Basic functions
        BasicAddToContextMenu(popup, type);         //pecan 2006/03/22

        // Extended functions, part 1 (virtual)
        AddToContextMenu(popup, type, false);       //pecan 2006/03/22

        // ask other editors / plugins if they need to add any entries in this menu...
        FileTreeData* ftd = new FileTreeData(0, FileTreeData::ftdkUndefined);
        ftd->SetFolder(m_Filename);
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(type, popup, ftd);              //pecan 2006/03/22
        delete ftd;

        popup->AppendSeparator();
        // Extended functions, part 2 (virtual)
        AddToContextMenu(popup, type, true);            //pecan 2006/03/22
    }
    // inform the editors we 're done creating a context menu (just about to show it)
    OnAfterBuildContextMenu(type);              //pecan 2006/03/22

    // display menu
    wxPoint clientpos;
    if (position==wxDefaultPosition) // "context menu" key
    {
        // obtain the caret point (on the screen) as we assume
        // that the user wants to work with the keyboard
        cbStyledTextCtrl* const control = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();
        clientpos = control->PointFromPosition(control->GetCurrentPos());
    }
    else
    {
        clientpos = ScreenToClient(position);
    }

	m_pData->m_DisplayingPopupMenu = true;
    PopupMenu(popup, clientpos);
    delete popup;
    m_pData->m_DisplayingPopupMenu = false;

    // this code *must* be the last code executed by this function
    // because it *will* invalidate 'this'
    if (m_pData->m_CloseMe)
		Manager::Get()->GetEditorManager()->Close(this);
}

void EditorBase::OnContextMenuEntry(wxCommandEvent& event)
{
    // we have a single event handler for all popup menu entries
    // This was ported from cbEditor and used for the basic operations:
    // Switch to, close, save, etc.

    const int id = event.GetId();
    m_pData->m_CloseMe = false;

    if (id == idCloseMe)
    {
    	if (m_pData->m_DisplayingPopupMenu)
			m_pData->m_CloseMe = true; // defer delete 'this' until after PopupMenu() call returns
		else
			Manager::Get()->GetEditorManager()->Close(this);
    }
    else if (id == idCloseAll)
    {
    	if (m_pData->m_DisplayingPopupMenu)
    	{
			Manager::Get()->GetEditorManager()->CloseAllExcept(this);
			m_pData->m_CloseMe = true; // defer delete 'this' until after PopupMenu() call returns
    	}
		else
			Manager::Get()->GetEditorManager()->CloseAll();
    }
    else if (id == idCloseAllOthers)
    {
        Manager::Get()->GetEditorManager()->CloseAllExcept(this);
    }
    else if (id == idSaveMe)
    {
        Save();
    }
    else if (id == idSaveAll)
    {
        Manager::Get()->GetEditorManager()->SaveAll();
    }
    else
    if (id >= idSwitchFile1 && id <= idSwitchFileMax)
    {
        // "Switch to..." item
        EditorBase *const ed = m_SwitchTo[id];
        if (ed)
        {
            Manager::Get()->GetEditorManager()->SetActiveEditor(ed);
        }
        m_SwitchTo.clear();
    }
    else if(wxMinimumVersion<2,6,1>::eval)
    {
        if (id == idGoogleCode)
        {
            wxLaunchDefaultBrowser(wxString(_T("http://www.google.com/codesearch?q=")) << URLEncode(lastWord));
        }
        else if (id == idGoogle)
        {
            wxLaunchDefaultBrowser(wxString(_T("http://www.google.com/search?q=")) << URLEncode(lastWord));
        }
        else if (id == idMsdn)
        {
            wxLaunchDefaultBrowser(wxString(_T("http://search.microsoft.com/search/results.aspx?qu=")) << URLEncode(lastWord) << _T("&View=msdn"));
        }
    }
    else
    {
        event.Skip();
    }
}
