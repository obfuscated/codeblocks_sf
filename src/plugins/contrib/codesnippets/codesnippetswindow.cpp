/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "sdk.h"
#ifndef CB_PRECOMP
	#include <wx/button.h>
	#include <wx/imaglist.h>
	#include <wx/intl.h>
	#include <wx/sizer.h>
	#include <wx/textctrl.h>
	#include "manager.h"
	#include "messagemanager.h"
	#include "configmanager.h"
	#include "editormanager.h"
	#include "cbeditor.h"
	#include "globals.h"
#endif
#include <wx/colour.h>
#include <wx/filedlg.h>
#include <wx/tokenzr.h>

#include "codesnippetswindow.h"
#include "snippetitemdata.h"
#include <tinyxml/tinyxml.h>

#define SNIPPETS_TREE_IMAGE_COUNT 3

const wxString snippetsTreeImageFileNames[] = {
	_T("allsnippets.png"),
	_T("category.png"),
	_T("snippet.png")
};

int idSearchSnippetCtrl = wxNewId();
int idClearSearchBtn = wxNewId();
int idSnippetsTreeCtrl = wxNewId();

// Menu items
int idMnuAddSubCategory = wxNewId();
int idMnuRemove = wxNewId();
int idMnuAddSnippet = wxNewId();
int idMnuApplySnippet = wxNewId();
int idMnuLoadSnippetsFromFile = wxNewId();
int idMnuSaveSnippetsToFile = wxNewId();
int idMnuRemoveAll = wxNewId();

BEGIN_EVENT_TABLE(CodeSnippetsWindow, wxPanel)
	// Menu events
	EVT_MENU(idMnuRemove, CodeSnippetsWindow::OnMnuRemove)
	EVT_MENU(idMnuAddSubCategory, CodeSnippetsWindow::OnMnuAddSubCategory)
	EVT_MENU(idMnuAddSnippet, CodeSnippetsWindow::OnMnuAddSnippet)
	EVT_MENU(idMnuApplySnippet, CodeSnippetsWindow::OnMnuApplySnippet)
	EVT_MENU(idMnuLoadSnippetsFromFile, CodeSnippetsWindow::OnMnuLoadSnippetsFromFile)
	EVT_MENU(idMnuSaveSnippetsToFile, CodeSnippetsWindow::OnMnuSaveSnippetsToFile)
	EVT_MENU(idMnuRemoveAll, CodeSnippetsWindow::OnMnuRemoveAll)
	// ---
	EVT_BUTTON(idClearSearchBtn, CodeSnippetsWindow::OnClearSearch)
	EVT_TEXT(idSearchSnippetCtrl, CodeSnippetsWindow::OnSearch)
	EVT_TREE_ITEM_ACTIVATED(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemActivated)
	EVT_TREE_ITEM_MENU(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemMenu)
	EVT_TREE_BEGIN_DRAG(idSnippetsTreeCtrl, CodeSnippetsWindow::OnBeginDrag)
	EVT_TREE_END_DRAG(idSnippetsTreeCtrl, CodeSnippetsWindow::OnEndDrag)
	EVT_TREE_BEGIN_LABEL_EDIT(idSnippetsTreeCtrl, CodeSnippetsWindow::OnBeginLabelEdit)
	EVT_TREE_END_LABEL_EDIT(idSnippetsTreeCtrl, CodeSnippetsWindow::OnEndLabelEdit)
END_EVENT_TABLE()

CodeSnippetsWindow::CodeSnippetsWindow()
	: wxPanel(Manager::Get()->GetAppWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
	// Initialize the dialog
	InitDialog();

	// Load the snippets
	m_SnippetsTreeCtrl->LoadItemsFromFile(ConfigManager::GetFolder(sdConfig) + wxFILE_SEP_PATH + _T("codesnippets.xml"));
}

CodeSnippetsWindow::~CodeSnippetsWindow()
{
	// Save the snippets
	m_SnippetsTreeCtrl->SaveItemsToFile(ConfigManager::GetFolder(sdConfig) + wxFILE_SEP_PATH + _T("codesnippets.xml"));

	// Release tree images
	delete m_SnippetsTreeImageList;
}

void CodeSnippetsWindow::InitDialog()
{
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* searchCtrlSizer;
	searchCtrlSizer = new wxBoxSizer(wxHORIZONTAL);

	m_SearchSnippetCtrl = new wxTextCtrl(this, idSearchSnippetCtrl, _T(""), wxDefaultPosition, wxDefaultSize, 0);
	searchCtrlSizer->Add(m_SearchSnippetCtrl, 1, wxALL, 5);

	m_ClearSearchBtn = new wxButton(this, idClearSearchBtn, _("Clear"), wxDefaultPosition, wxDefaultSize, 0);
	m_ClearSearchBtn->Enable(false);
	searchCtrlSizer->Add(m_ClearSearchBtn, 0, wxALL, 5);

	panelSizer->Add(searchCtrlSizer, 0, wxEXPAND, 5);

	wxBoxSizer* treeCtrlSizer;
	treeCtrlSizer = new wxBoxSizer(wxVERTICAL);

	m_SnippetsTreeCtrl = new CodeSnippetsTreeCtrl(this, idSnippetsTreeCtrl, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS);
	treeCtrlSizer->Add(m_SnippetsTreeCtrl, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

	panelSizer->Add(treeCtrlSizer, 1, wxEXPAND, 5);

	SetSizer(panelSizer);
	Layout();

	m_SnippetsTreeCtrl->SetDropTarget(new SnippetsDropTarget(m_SnippetsTreeCtrl));

	wxString dataFolder = ConfigManager::GetFolder(sdDataGlobal) + wxFILE_SEP_PATH + _T("images");
	wxColor maskColor(255, 0, 255);
	m_SnippetsTreeImageList = new wxImageList(16, 16, true, 3);

	for (int i = 0; i < SNIPPETS_TREE_IMAGE_COUNT; ++i)
	{
		wxBitmap imageFile = cbLoadBitmap(dataFolder + wxFILE_SEP_PATH + _T("codesnippets") + wxFILE_SEP_PATH + snippetsTreeImageFileNames[i], wxBITMAP_TYPE_PNG);
		m_SnippetsTreeImageList->Add(imageFile, maskColor);
	}

	m_SnippetsTreeCtrl->SetImageList(m_SnippetsTreeImageList);

	// Add root item
	SnippetItemData* rootData = new SnippetItemData(SnippetItemData::TYPE_ROOT);
	m_SnippetsTreeCtrl->AddRoot(_("All snippets"), 0, -1, rootData);

	m_AppendItemsFromFile = false;
}

void CodeSnippetsWindow::OnClearSearch(wxCommandEvent& /*event*/)
{
	m_SearchSnippetCtrl->Clear();
}

void CodeSnippetsWindow::OnSearch(wxCommandEvent& /*event*/)
{
	if (m_SearchSnippetCtrl->GetValue() == wxEmptyString)
	{
		m_ClearSearchBtn->Enable(false);

		// Reset the root node's title
		m_SnippetsTreeCtrl->SetItemText(m_SnippetsTreeCtrl->GetRootItem(), _("All snippets"));

		// Reset the searchbox's background color
		m_SearchSnippetCtrl->SetBackgroundColour(wxNullColour);
		m_SearchSnippetCtrl->Refresh();
	}
	else
	{
		m_ClearSearchBtn->Enable(true);

		// Edit the root node's title so that the user knows we are
		// searching for specific item
		m_SnippetsTreeCtrl->SetItemText(m_SnippetsTreeCtrl->GetRootItem(), _T("Search \"") + m_SearchSnippetCtrl->GetValue() + _T("\""));

		// Search it!
		wxTreeItemId foundID = SearchSnippet(m_SearchSnippetCtrl->GetValue(), m_SnippetsTreeCtrl->GetRootItem());

		if (foundID.IsOk())
		{
			// Highlight the item
			m_SnippetsTreeCtrl->EnsureVisible(foundID);
			m_SnippetsTreeCtrl->SelectItem(foundID);

			// Reset the searchbox's backgroud color
			m_SearchSnippetCtrl->SetBackgroundColour(wxNullColour);
			m_SearchSnippetCtrl->Refresh();
		}
		else
		{
			// Select the root item so user doesn't think we found something
			m_SnippetsTreeCtrl->EnsureVisible(m_SnippetsTreeCtrl->GetRootItem());
			m_SnippetsTreeCtrl->SelectItem(m_SnippetsTreeCtrl->GetRootItem());

			// Add visual feedback: paint the searchbox's background with
			// light red color
			m_SearchSnippetCtrl->SetBackgroundColour(wxColor(244, 168, 168));
			m_SearchSnippetCtrl->Refresh();
		}
	}
} // end of OnSearch

void CodeSnippetsWindow::OnItemActivated(wxTreeEvent& event)
{
	ApplySnippet(event.GetItem());
}

void CodeSnippetsWindow::OnItemMenu(wxTreeEvent& event)
{
	// Get the item associated with the event
	if (const SnippetItemData* eventItem = (SnippetItemData*)(m_SnippetsTreeCtrl->GetItemData(event.GetItem())))
	{
		wxMenu* snippetsTreeMenu = new wxMenu();

		// Check the type of the item and add the menu items
		switch (eventItem->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				snippetsTreeMenu->Append(idMnuAddSnippet, _("Add code snippet"));
				snippetsTreeMenu->Append(idMnuAddSubCategory, _("Add subcategory"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuRemoveAll, _("Remove all items"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuSaveSnippetsToFile, _("Save to file..."));

				// Check if Shift key is pressed
				if (::wxGetKeyState(WXK_SHIFT))
				{
					// Add append from file entry
					snippetsTreeMenu->Append(idMnuLoadSnippetsFromFile, _T("Load from file (append)..."));
					m_AppendItemsFromFile = true;
				}
				else
				{
					// Use the normal load from file entry
					snippetsTreeMenu->Append(idMnuLoadSnippetsFromFile, _("Load from file..."));
					m_AppendItemsFromFile = false;
				}
			break;

			case SnippetItemData::TYPE_CATEGORY:
				snippetsTreeMenu->Append(idMnuAddSnippet, _("Add code snippet"));
				snippetsTreeMenu->Append(idMnuAddSubCategory, _("Add subcategory"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuRemove, _("Remove"));
			break;

			case SnippetItemData::TYPE_SNIPPET:
				snippetsTreeMenu->Append(idMnuApplySnippet, _("Apply"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuRemove, _("Remove"));
			break;
		}

		// Save the item ID for later use
		m_MnuAssociatedItemID = eventItem->GetId();

		PopupMenu(snippetsTreeMenu);

		delete snippetsTreeMenu;
	}
}

void CodeSnippetsWindow::OnBeginDrag(wxTreeEvent& /*event*/)
{
}

void CodeSnippetsWindow::OnEndDrag(wxTreeEvent& /*event*/)
{
}

void CodeSnippetsWindow::OnMnuAddSubCategory(wxCommandEvent& /*event*/)
{
	// Add new category using the associated item ID
	m_SnippetsTreeCtrl->AddCategory(m_MnuAssociatedItemID, _("New category"), true);
}

void CodeSnippetsWindow::OnMnuRemove(wxCommandEvent& /*event*/)
{
	// Get the associated item id
	wxTreeItemId itemID = m_MnuAssociatedItemID;

	// Sanity check
	if (itemID != m_SnippetsTreeCtrl->GetRootItem())
	{
		// No questions asked
		m_SnippetsTreeCtrl->DeleteChildren(itemID);
		m_SnippetsTreeCtrl->Delete(itemID);
	}
}

void CodeSnippetsWindow::OnMnuAddSnippet(wxCommandEvent& /*event*/)
{
	// Add new snippet using the associated item ID
	m_SnippetsTreeCtrl->AddCodeSnippet(m_MnuAssociatedItemID, _("New snippet"), wxEmptyString, true);
}

void CodeSnippetsWindow::ApplySnippet(const wxTreeItemId& itemID)
{
	// Get the item
	if (const SnippetItemData* item = (SnippetItemData*)(m_SnippetsTreeCtrl->GetItemData(itemID)))
	{
		// Check that we're using the correct item type
		if (item->GetType() != SnippetItemData::TYPE_SNIPPET)
		{
			return;
		}

		// Check that editor is open
		EditorManager* editorMan = Manager::Get()->GetEditorManager();
		if(!editorMan)
		{
			return;
		}

		cbEditor* editor = editorMan->GetBuiltinActiveEditor();
		if(!editor)
		{
			return;
		}

		cbStyledTextCtrl* ctrl = editor->GetControl();
		if(ctrl)
		{
			// Apply the snippet
			ctrl->AddText(item->GetSnippet());
		}
	}
} // end of ApplySnippet

void CodeSnippetsWindow::OnMnuApplySnippet(wxCommandEvent& /*event*/)
{
	// Apply the snippet using the associated item id
	ApplySnippet(m_MnuAssociatedItemID);
}

wxTreeItemId CodeSnippetsWindow::SearchSnippet(const wxString& searchTerms, const wxTreeItemId& node)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_SnippetsTreeCtrl->GetFirstChild(node, cookie );
	// Loop through all items
	while(item.IsOk())
	{
		const wxString label = m_SnippetsTreeCtrl->GetItemText(item);

		if(label.Contains(searchTerms))
		{
			return item;
		}

		if(m_SnippetsTreeCtrl->ItemHasChildren(item))
		{
			wxTreeItemId search = SearchSnippet(searchTerms, item);
			if(search.IsOk())
			{
				return search;
			}
		}
		item = m_SnippetsTreeCtrl->GetNextChild(node, cookie);
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem;
   return dummyItem;
} // end of SearchSnippet

void CodeSnippetsWindow::OnBeginLabelEdit(wxTreeEvent& event)
{
	// Deny editing of the root item
	if (event.GetItem() == m_SnippetsTreeCtrl->GetRootItem())
	{
		event.Veto();
	}
}

void CodeSnippetsWindow::OnMnuLoadSnippetsFromFile(wxCommandEvent& event)
{
	wxFileDialog dlg(this, _T("Load from file"), _T(""), _T(""), _T("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxOPEN|wxFILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
		if (!m_AppendItemsFromFile)
		{
			m_SnippetsTreeCtrl->DeleteChildren(m_SnippetsTreeCtrl->GetRootItem());
		}

		m_SnippetsTreeCtrl->LoadItemsFromFile(dlg.GetPath());
	}
} // end of OnMnuImportSnippets

void CodeSnippetsWindow::OnMnuSaveSnippetsToFile(wxCommandEvent& /*event*/)
{
	wxFileDialog dlg(this, _T("Save to file"), _T(""), _T("codesnippets.xml"), _T("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_SnippetsTreeCtrl->SaveItemsToFile(dlg.GetPath());
	}
} // end of OnMnuExportSnippets

void CodeSnippetsWindow::OnEndLabelEdit(wxTreeEvent& event)
{
	// Sort all the parent item's children
	m_SnippetsTreeCtrl->SortChildren(m_SnippetsTreeCtrl->GetItemParent(event.GetItem()));
} // end of OnEndLabelEdit

void CodeSnippetsWindow::OnMnuRemoveAll(wxCommandEvent& /*event*/)
{
	// Remove all items
	m_SnippetsTreeCtrl->DeleteChildren(m_SnippetsTreeCtrl->GetRootItem());
} // end of OnMnuRemoveAll

bool SnippetsDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
	// Set focus to the Code snippets window
	m_TreeCtrl->SetFocus();

	wxPoint p(x, y);
	int hitTestFlags = 0;

	// Find out if there's a tree item under the coordinates
	wxTreeItemId itemID = m_TreeCtrl->HitTest(p, hitTestFlags);

	if (hitTestFlags & wxTREE_HITTEST_ONITEMBUTTON|wxTREE_HITTEST_ONITEMICON|wxTREE_HITTEST_ONITEMLABEL)
	{
		// Find out the item type
		if (SnippetItemData* item = (SnippetItemData*)(m_TreeCtrl->GetItemData(itemID)))
		{
			switch (item->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					// Add new code snippet to the root
					m_TreeCtrl->AddCodeSnippet(m_TreeCtrl->GetRootItem(), _("New snippet"), data, true);
				break;

				case SnippetItemData::TYPE_CATEGORY:
					// Add new code snippet to the category
					m_TreeCtrl->AddCodeSnippet(item->GetId(), _("New snippet"), data, true);
				break;

				case SnippetItemData::TYPE_SNIPPET:
					// Set the snippet's code to match the dropped data
					item->SetSnippet(data);
				break;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
} // end of OnDropText
