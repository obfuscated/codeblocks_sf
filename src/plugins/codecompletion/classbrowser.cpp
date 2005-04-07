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

#include "classbrowser.h" // class's header file
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <configmanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <globals.h>

int ID_ClassBrowser = wxNewId();
int idMenuJumpToDeclaration = wxNewId();
int idMenuJumpToImplementation = wxNewId();
int idMenuRefreshTree = wxNewId();
int idCBViewInheritance = wxNewId();
int idCBViewModeFlat = wxNewId();
int idCBViewModeStructured = wxNewId();

BEGIN_EVENT_TABLE(ClassBrowser, wxPanel)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_ClassBrowser, ClassBrowser::OnTreeItemRightClick)
    EVT_MENU(idMenuJumpToDeclaration, ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuJumpToImplementation, ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuRefreshTree, ClassBrowser::OnRefreshTree)
    EVT_MENU(idCBViewInheritance, ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBViewModeFlat, ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBViewModeStructured, ClassBrowser::OnCBViewMode)
END_EVENT_TABLE()

// class constructor
ClassBrowser::ClassBrowser(wxNotebook* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN),
    m_Parent(parent),
	m_pParser(0L)
{
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
	m_Tree = new wxTreeCtrl(this, ID_ClassBrowser);
    bs->Add(m_Tree, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

    m_Parent->AddPage(this, _("Symbols"));
    m_PageIndex = m_Parent->GetPageCount() - 1;
}

// class destructor
ClassBrowser::~ClassBrowser()
{
	m_Parent->RemovePage(m_PageIndex);
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
	}
	else
		m_Tree->DeleteAllItems();
}

void ClassBrowser::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
// NOTE: local variables are tricky! If you build two local menus
// and attach menu B to menu A, on function exit both menu A and menu B
// will be destroyed. But when destroying menu A, menu B will be destroyed
// again. Its already-freed memory will be accessed, generating a segfault.

// A safer approach is to make all menus heap-based, and delete the topmost
// on exit.

    if ( !id.IsOk() )
        return;

#if wxUSE_MENUS
	wxString caption;
    wxMenu *menu=new wxMenu(wxEmptyString);

	ClassTreeData* ctd = (ClassTreeData*)m_Tree->GetItemData(id);
    if (ctd)
    {
       menu->Append(idMenuJumpToDeclaration, _("Jump to &declaration"));
    }

    // ask any plugins to add items in this menu
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtClassBrowser, menu, m_Tree->GetItemText(id));

    if (menu->GetMenuItemCount() != 0)
		menu->AppendSeparator();

	wxMenu *sub = new wxMenu("");
    sub->AppendCheckItem(idCBViewInheritance, _("Show inherited members"));
    sub->AppendSeparator();
    sub->AppendRadioItem(idCBViewModeFlat, _("Flat"));
    sub->AppendRadioItem(idCBViewModeStructured, _("Structured"));

	menu->Append(wxNewId(), _("&View options"), sub);
    menu->Append(idMenuRefreshTree, _("&Refresh tree"));

	menu->Check(idCBViewInheritance, m_pParser ? m_pParser->ClassBrowserOptions().showInheritance : false);
	sub->Check(idCBViewModeFlat, m_pParser ? m_pParser->ClassBrowserOptions().viewFlat : false);
	sub->Check(idCBViewModeStructured, m_pParser ? !m_pParser->ClassBrowserOptions().viewFlat : false);

    PopupMenu(menu, pt);
    delete menu; // Prevents memory leak
#endif // wxUSE_MENUS
}

// events

void ClassBrowser::OnTreeItemRightClick(wxTreeEvent& event)
{
	m_Tree->SelectItem(event.GetItem());
    ShowMenu(event.GetItem(), event.GetPoint());
}

void ClassBrowser::OnJumpTo(wxCommandEvent& event)
{
	int id = m_Tree->GetSelection();
	ClassTreeData* ctd = (ClassTreeData*)m_Tree->GetItemData(id);
    if (ctd)
    {
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (prj)
        {
            wxString base = prj->GetBasePath();
            wxFileName fname(ctd->GetToken()->m_Filename);
            fname.Normalize(wxPATH_NORM_ALL, base);
        	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetFullPath());
			if (ed)
			{
				int pos = ed->GetControl()->PositionFromLine(ctd->GetToken()->m_Line - 1);
				ed->GetControl()->GotoPos(pos);
			}
        }
    }
}

void ClassBrowser::OnRefreshTree(wxCommandEvent& event)
{
	Update();
}

void ClassBrowser::OnCBViewMode(wxCommandEvent& event)
{
	if (!m_pParser)
		return;
		
	if (event.GetId() == idCBViewInheritance)
		m_pParser->ClassBrowserOptions().showInheritance = !event.IsChecked();
	else if (event.GetId() == idCBViewModeFlat)
		m_pParser->ClassBrowserOptions().viewFlat = !event.IsChecked();
	else if (event.GetId() == idCBViewModeStructured)
		m_pParser->ClassBrowserOptions().viewFlat = event.IsChecked();
	else
		return;

	m_pParser->WriteOptions();
	Update();
}
