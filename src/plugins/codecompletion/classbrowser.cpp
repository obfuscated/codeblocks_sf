/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include <sdk.h>
#include "classbrowser.h" // class's header file
#include "nativeparser.h"
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/menu.h>
#include <manager.h>
#include <configmanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <cbeditor.h>
#include <globals.h>

class myTextCtrl : public wxTextCtrl
{
    public:
        myTextCtrl(ClassBrowser* cb,
                    wxWindow* parent,
                    wxWindowID id,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTE_PROCESS_ENTER,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxTextCtrlNameStr)
            : wxTextCtrl(parent, id, value, pos, size, style, validator, name),
            m_CB(cb)
            {}
        virtual ~myTextCtrl() {}
    protected:
        ClassBrowser* m_CB;

        // Intercept key presses to handle Enter. */
        void OnKey(wxKeyEvent& event)
        {
            if (event.GetKeyCode() == WXK_RETURN)
            {
                wxCommandEvent e;
                m_CB->OnSearch(e);
            }
            else
                event.Skip();
        }
        DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(myTextCtrl, wxTextCtrl)
	EVT_KEY_DOWN(myTextCtrl::OnKey)
END_EVENT_TABLE()

int ID_TopPanel = wxNewId();
int ID_Search = wxNewId();
int ID_ClassBrowser = wxNewId();
int ID_ClassMembers = wxNewId();
int idBtnSearch = wxNewId();
int idCombo = wxNewId();
int idMenuJumpToDeclaration = wxNewId();
int idMenuJumpToImplementation = wxNewId();
int idMenuRefreshTree = wxNewId();
int idCBViewInheritance = wxNewId();
int idCBViewModeFlat = wxNewId();
int idCBViewModeStructured = wxNewId();
int idMenuForceReparse = wxNewId();

BEGIN_EVENT_TABLE(ClassBrowser, wxPanel)
	EVT_TREE_SEL_CHANGED(ID_ClassBrowser, ClassBrowser::OnTreeItemClick)
	EVT_TREE_ITEM_ACTIVATED(ID_ClassBrowser, ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_ClassBrowser, ClassBrowser::OnTreeItemRightClick)

	EVT_TREE_ITEM_ACTIVATED(ID_ClassMembers, ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_ClassMembers, ClassBrowser::OnListItemRightClick)

    EVT_MENU(idMenuJumpToDeclaration, ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuJumpToImplementation, ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuRefreshTree, ClassBrowser::OnRefreshTree)
    EVT_MENU(idMenuForceReparse, ClassBrowser::OnForceReparse)
    EVT_MENU(idCBViewInheritance, ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBViewModeFlat, ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBViewModeStructured, ClassBrowser::OnCBViewMode)
    EVT_COMBOBOX(idCombo, ClassBrowser::OnViewScope)
    EVT_BUTTON(idBtnSearch, ClassBrowser::OnSearch)
END_EVENT_TABLE()

// class constructor
ClassBrowser::ClassBrowser(wxNotebook* parent, NativeParser* np)
    : wxSplitPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN),
    m_Parent(parent),
    m_NativeParser(np),
    m_TreeForPopupMenu(0),
	m_pParser(0L)
{
    SetConfigEntryForSplitter(_T("class_browser_tree_height"));
    wxPanel* top = new wxPanel(GetSplitter(), ID_TopPanel, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* fs = new wxFlexGridSizer(2, 4, 0);
    fs->AddGrowableCol(1);

    fs->Add(new wxStaticText(top, -1, _T("View:")), 0, wxALIGN_CENTER_VERTICAL);

    wxArrayString choices;
    choices.Add(_T("Current file's symbols only..."));
    choices.Add(_T("All project symbols"));
    wxComboBox* cmb = new wxComboBox(top, idCombo, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
    bool all = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/show_all_symbols"), false);
    cmb->SetSelection(all ? 1 : 0);
    fs->Add(cmb, 1, wxEXPAND);

    fs->Add(new wxStaticText(top, -1, _T("Search:")), 0, wxALIGN_CENTER_VERTICAL);

    wxBoxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
    m_Search = new myTextCtrl(this, top, ID_Search);
    hs->Add(m_Search, 1, wxEXPAND);
    wxButton* search = new wxButton(top, idBtnSearch, _T(">"), wxDefaultPosition, wxSize(24, 24));
    hs->Add(search, 0, wxALIGN_CENTER_VERTICAL);
    fs->Add(hs, 0, wxEXPAND | wxBOTTOM, 4);

    bs->Add(fs, 0, wxEXPAND | wxTOP, 4);

	m_Tree = new wxTreeCtrl(top, ID_ClassBrowser, wxPoint(0, 0), wxSize(100, 64), wxTR_HAS_BUTTONS | wxTR_DEFAULT_STYLE);
    bs->Add(m_Tree, 1, wxEXPAND | wxALL);
    top->SetAutoLayout(TRUE);
    top->SetSizer(bs);

	m_List = new wxTreeCtrl(GetSplitter(), ID_ClassMembers, wxPoint(0, 0), wxSize(100, 64), wxTR_DEFAULT_STYLE);

    GetSplitter()->SetMinimumPaneSize(64);
    SetAutoLayout(true);
	RefreshSplitter(ID_TopPanel, ID_ClassMembers);
    Refresh();
    m_Tree->Refresh();
    m_List->Refresh();

    m_Parent->AddPage(this, _("Symbols"));
    m_PageIndex = m_Parent->GetPageCount() - 1;
}

// class destructor
ClassBrowser::~ClassBrowser()
{
//	m_Parent->RemovePage(m_PageIndex);
}

void ClassBrowser::SetParser(Parser* parser)
{
	if (parser != m_pParser)
	{
		m_pParser = parser;
		Update();
	}
}

void ClassBrowser::Update()
{
	if (m_pParser)
	{
		wxArrayString treeState;
		::SaveTreeState(m_Tree, m_pParser->GetRootNode(), treeState);
		m_pParser->BuildTree(*m_Tree);
		::RestoreTreeState(m_Tree, m_pParser->GetRootNode(), treeState);
		if (!m_Tree->IsExpanded(m_pParser->GetRootNode()))
            m_Tree->Expand(m_pParser->GetRootNode());
	}
	else
		m_Tree->DeleteAllItems();
}

void ClassBrowser::ShowMenu(wxTreeCtrl* tree, wxTreeItemId id, const wxPoint& pt)
{
// NOTE: local variables are tricky! If you build two local menus
// and attach menu B to menu A, on function exit both menu A and menu B
// will be destroyed. But when destroying menu A, menu B will be destroyed
// again. Its already-freed memory will be accessed, generating a segfault.

// A safer approach is to make all menus heap-based, and delete the topmost
// on exit.

    m_TreeForPopupMenu = tree;
    if ( !id.IsOk() )
        return;

#if wxUSE_MENUS
	wxString caption;
    wxMenu *menu=new wxMenu(wxEmptyString);

	ClassTreeData* ctd = (ClassTreeData*)tree->GetItemData(id);
    if (ctd)
    {
        switch (ctd->GetToken()->m_TokenKind)
        {
            case tkConstructor:
            case tkDestructor:
            case tkFunction:
                if (ctd->GetToken()->m_ImplLine != 0 && !ctd->GetToken()->m_ImplFilename.IsEmpty())
                    menu->Append(idMenuJumpToImplementation, _("Jump to &implementation"));
                // intentionally fall through
            default:
                menu->Append(idMenuJumpToDeclaration, _("Jump to &declaration"));
        }
    }

    // ask any plugins to add items in this menu
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtClassBrowser, menu, tree->GetItemText(id));

    if (tree == m_Tree)
    {
        if (menu->GetMenuItemCount() != 0)
            menu->AppendSeparator();

        wxMenu *sub = new wxMenu(_T(""));
        sub->AppendCheckItem(idCBViewInheritance, _("Show inherited members"));
        sub->AppendSeparator();
        sub->AppendRadioItem(idCBViewModeFlat, _("Flat"));
        sub->AppendRadioItem(idCBViewModeStructured, _("Structured"));

        menu->Append(wxNewId(), _("&View options"), sub);
        menu->Append(idMenuRefreshTree, _("&Refresh tree"));

        if (id == m_Tree->GetRootItem())
        {
            menu->AppendSeparator();
            menu->Append(idMenuForceReparse, _("Re-parse now"));
        }

        menu->Check(idCBViewInheritance, m_pParser ? m_pParser->ClassBrowserOptions().showInheritance : false);
        sub->Check(idCBViewModeFlat, m_pParser ? m_pParser->ClassBrowserOptions().viewFlat : false);
        sub->Check(idCBViewModeStructured, m_pParser ? !m_pParser->ClassBrowserOptions().viewFlat : false);
    }

    PopupMenu(menu, tree->GetParent()->ScreenToClient(tree->ClientToScreen(pt)));
    delete menu; // Prevents memory leak
#endif // wxUSE_MENUS
}

// events

void ClassBrowser::OnTreeItemRightClick(wxTreeEvent& event)
{
    wxTreeCtrl* tree = m_Tree;
	tree->SelectItem(event.GetItem());
    ShowMenu(tree, event.GetItem(), event.GetPoint());// + tree->GetPosition());
}

void ClassBrowser::OnListItemRightClick(wxTreeEvent& event)
{
    wxTreeCtrl* tree = m_List;
	tree->SelectItem(event.GetItem());
    ShowMenu(tree, event.GetItem(), event.GetPoint());
}

void ClassBrowser::OnTreeItemClick(wxTreeEvent& event)
{
	wxTreeItemId id = event.GetItem();
	m_List->Freeze();
	m_List->DeleteAllItems();
	if (!id.IsOk())
	{
	    m_List->Thaw();
	    return;
	}
	m_List->SetImageList(m_Tree->GetImageList());
    ClassTreeData* old = static_cast<ClassTreeData*>(m_Tree->GetItemData(id));
    ClassTreeData* cdt = old ? new ClassTreeData(old->GetToken()) : 0;
	wxTreeItemId root = m_List->AddRoot(m_Tree->GetItemText(id), m_Tree->GetItemImage(id), m_Tree->GetItemImage(id), cdt);

    wxTreeItemIdValue cookie;
	wxTreeItemId child = m_Tree->GetFirstChild(id, cookie);
	while (child.IsOk())
	{
	    old = static_cast<ClassTreeData*>(m_Tree->GetItemData(child));
	    cdt = old ? new ClassTreeData(old->GetToken()) : 0;
        m_List->AppendItem(root, m_Tree->GetItemText(child), m_Tree->GetItemImage(child), m_Tree->GetItemImage(child), cdt);
        child = m_Tree->GetNextChild(id, cookie);
	}
    m_List->Expand(root);
    m_List->Thaw();
}

void ClassBrowser::OnJumpTo(wxCommandEvent& event)
{
    wxTreeCtrl* tree = m_TreeForPopupMenu;
	wxTreeItemId id = tree->GetSelection();
	ClassTreeData* ctd = (ClassTreeData*)tree->GetItemData(id);
    if (ctd)
    {
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (prj)
        {
            wxString base = prj->GetBasePath();
            wxFileName fname;
            if (event.GetId() == idMenuJumpToImplementation)
                fname.Assign(ctd->GetToken()->m_ImplFilename);
            else
                fname.Assign(ctd->GetToken()->m_Filename);
            fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
        	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetFullPath());
			if (ed)
			{
                int line;
                if (event.GetId() == idMenuJumpToImplementation)
                    line = ctd->GetToken()->m_ImplLine - 1;
                else
                    line = ctd->GetToken()->m_Line - 1;
				ed->GotoLine(line);
			}
        }
    }
}

void ClassBrowser::OnTreeItemDoubleClick(wxTreeEvent& event)
{
    wxTreeCtrl* tree = (wxTreeCtrl*)event.GetEventObject();
	wxTreeItemId id = event.GetItem();
	ClassTreeData* ctd = (ClassTreeData*)tree->GetItemData(id);
    if (ctd)
    {
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (prj)
        {
			bool toImp = false;
			switch (ctd->GetToken()->m_TokenKind)
			{
			case tkConstructor:
            case tkDestructor:
            case tkFunction:
                if (ctd->GetToken()->m_ImplLine != 0 && !ctd->GetToken()->m_ImplFilename.IsEmpty())
                    toImp = true;
				break;
			default:
				break;
			}

            wxString base = prj->GetBasePath();
            wxFileName fname;
            if (toImp)
                fname.Assign(ctd->GetToken()->m_ImplFilename);
            else
                fname.Assign(ctd->GetToken()->m_Filename);
            fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
        	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetFullPath());
			if (ed)
			{
				int line;
                if (toImp)
                    line = ctd->GetToken()->m_ImplLine - 1;
                else
                    line = ctd->GetToken()->m_Line - 1;
				ed->GotoLine(line);

				wxFocusEvent ev(wxEVT_SET_FOCUS);
				ev.SetWindow(this);
				ed->GetControl()->AddPendingEvent(ev);
			}
        }
    }
}

void ClassBrowser::OnRefreshTree(wxCommandEvent& event)
{
	Update();
}

void ClassBrowser::OnForceReparse(wxCommandEvent& event)
{
    if (m_NativeParser)
        m_NativeParser->ForceReparseActiveProject();
}

void ClassBrowser::OnCBViewMode(wxCommandEvent& event)
{
	if (!m_pParser)
		return;

	if (event.GetId() == idCBViewInheritance)
		m_pParser->ClassBrowserOptions().showInheritance = event.IsChecked();
	else if (event.GetId() == idCBViewModeFlat)
		m_pParser->ClassBrowserOptions().viewFlat = event.IsChecked();
	else if (event.GetId() == idCBViewModeStructured)
		m_pParser->ClassBrowserOptions().viewFlat = !event.IsChecked();
	else
		return;

	m_pParser->WriteOptions();
	Update();
}

void ClassBrowser::OnViewScope(wxCommandEvent& event)
{
	if (m_pParser)
	{
		m_pParser->ClassBrowserOptions().showAllSymbols = event.GetSelection() == 1;
		m_pParser->WriteOptions();
        Update();
	}
	else
	{
	    // we have no parser; just write the setting in the configuration
        Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/show_all_symbols"), event.GetSelection() == 1);
	}
}

bool ClassBrowser::RecursiveSearch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& parent, wxTreeItemId& result)
{
    wxTreeItemIdValue cookie;
	wxTreeItemId child = tree->GetFirstChild(parent, cookie);
	while (child.IsOk())
	{
        ClassTreeData* ctd = static_cast<ClassTreeData*>(tree->GetItemData(child));
        if (ctd && ctd->GetToken())
        {
            Token* token = ctd->GetToken();
            if (token->m_Name.Lower().StartsWith(search))
            {
                result = child;
                return true;
            }
        }
        if (tree->ItemHasChildren(child))
        {
            if (RecursiveSearch(search, tree, child, result))
                return true;
        }
        child = tree->GetNextChild(parent, cookie);
	}
	return false;
}

void ClassBrowser::OnSearch(wxCommandEvent& event)
{
    if (m_Search->GetValue().IsEmpty())
        return;
    wxTreeItemId result;
    if (RecursiveSearch(m_Search->GetValue().Lower(), m_Tree, m_Tree->GetRootItem(), result))
    {
        m_Tree->SelectItem(result);
        m_Tree->EnsureVisible(result);
        if (RecursiveSearch(m_Search->GetValue().Lower(), m_List, m_List->GetRootItem(), result))
        {
            m_List->SelectItem(result);
            m_List->EnsureVisible(result);
        }
    }
}
