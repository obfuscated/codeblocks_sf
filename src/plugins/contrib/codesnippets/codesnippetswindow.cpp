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
#define TREE_IMAGE_ALL_SNIPPETS 0
#define TREE_IMAGE_CATEGORY 1
#define TREE_IMAGE_SNIPPET 2

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
int idMnuImportSnippets = wxNewId();
int idMnuExportSnippets = wxNewId();

BEGIN_EVENT_TABLE(CodeSnippetsWindow, wxPanel)
	// Menu events
	EVT_MENU(idMnuRemove, CodeSnippetsWindow::OnMnuRemove)
	EVT_MENU(idMnuAddSubCategory, CodeSnippetsWindow::OnMnuAddSubCategory)
	EVT_MENU(idMnuAddSnippet, CodeSnippetsWindow::OnMnuAddSnippet)
	EVT_MENU(idMnuApplySnippet, CodeSnippetsWindow::OnMnuApplySnippet)
	EVT_MENU(idMnuImportSnippets, CodeSnippetsWindow::OnMnuImportSnippets)
	EVT_MENU(idMnuExportSnippets, CodeSnippetsWindow::OnMnuExportSnippets)
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
	LoadSnippetsFromFile(ConfigManager::GetFolder(sdConfig) + wxFILE_SEP_PATH + _T("codesnippets.xml"));
}

CodeSnippetsWindow::~CodeSnippetsWindow()
{
	// Save the snippets
	SaveSnippetsToFile(ConfigManager::GetFolder(sdConfig) + wxFILE_SEP_PATH + _T("codesnippets.xml"));

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

	m_SnippetsTreeCtrl->SetDropTarget(new SnippetsDropTarget(this));

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
	m_RootID = m_SnippetsTreeCtrl->AddRoot(_("All snippets"), TREE_IMAGE_ALL_SNIPPETS, -1, rootData);
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
		m_SnippetsTreeCtrl->SetItemText(m_RootID, _("All snippets"));
	}
	else
	{
		m_ClearSearchBtn->Enable(true);

		// Edit the root node's title so that the user knows we are
		// searching for specific item
		m_SnippetsTreeCtrl->SetItemText(m_RootID, _T("Search \"") + m_SearchSnippetCtrl->GetValue() + _T("\""));

		// Search it!
		wxTreeItemId foundID = SearchSnippet(m_SearchSnippetCtrl->GetValue(), m_RootID);

		if (foundID.IsOk())
		{
			// Highlight the item
			m_SnippetsTreeCtrl->EnsureVisible(foundID);
			m_SnippetsTreeCtrl->SelectItem(foundID);
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
				snippetsTreeMenu->Append(idMnuImportSnippets, _("Import from file..."));
				snippetsTreeMenu->Append(idMnuExportSnippets, _("Export to XML..."));
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
	AddCategory(m_MnuAssociatedItemID, _("New category"), true);
}

// NOTE : nobody uses the reurn vlaue -> could remove it
wxTreeItemId CodeSnippetsWindow::AddCodeSnippet(const wxTreeItemId& parent, wxString title, wxString codeSnippet, bool editNow)
{
	wxTreeItemId newItemID = m_SnippetsTreeCtrl->InsertItem(parent, m_SnippetsTreeCtrl->GetLastChild(parent), title, TREE_IMAGE_SNIPPET, -1, new SnippetItemData(SnippetItemData::TYPE_SNIPPET, codeSnippet));

	// Sort 'em
	m_SnippetsTreeCtrl->SortChildren(parent);

	if (editNow)
	{
		// Let the user to edit the item
		m_SnippetsTreeCtrl->EnsureVisible(newItemID);
		m_SnippetsTreeCtrl->EditLabel(newItemID);
	}

	return newItemID;
}

wxTreeItemId CodeSnippetsWindow::AddCategory(const wxTreeItemId& parent, wxString title, bool editNow)
{
	wxTreeItemId newCategoryID = m_SnippetsTreeCtrl->InsertItem(parent, m_SnippetsTreeCtrl->GetLastChild(parent), title, TREE_IMAGE_CATEGORY, -1, new SnippetItemData(SnippetItemData::TYPE_CATEGORY));

	// Sort 'em
	m_SnippetsTreeCtrl->SortChildren(parent);

	if (editNow)
	{
		// Let the user to edit the category
		m_SnippetsTreeCtrl->EnsureVisible(newCategoryID);
		m_SnippetsTreeCtrl->EditLabel(newCategoryID);
	}

	return newCategoryID;
}

void CodeSnippetsWindow::OnMnuRemove(wxCommandEvent& /*event*/)
{
	// Get the associated item id
	wxTreeItemId itemID = m_MnuAssociatedItemID;

	// Sanity check
	if (itemID != m_RootID)
	{
		// No questions asked
		m_SnippetsTreeCtrl->DeleteChildren(itemID);
		m_SnippetsTreeCtrl->Delete(itemID);
	}
}

void CodeSnippetsWindow::OnMnuAddSnippet(wxCommandEvent& /*event*/)
{
	// Add new snippet using the associated item ID
	AddCodeSnippet(m_MnuAssociatedItemID, _("New snippet"), wxEmptyString, true);
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
	if (event.GetItem() == m_RootID)
	{
		event.Veto();
	}
}

void CodeSnippetsWindow::SaveSnippets(TiXmlNode* parentNode, const wxTreeItemId& parentID)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_SnippetsTreeCtrl->GetFirstChild(parentID, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		const SnippetItemData* data = (SnippetItemData*)(m_SnippetsTreeCtrl->GetItemData(item));

		if (!data)
		{
			return;
		}

		// Begin item element
		TiXmlElement element("item");

		// Write the item's name
		element.SetAttribute("name", m_SnippetsTreeCtrl->GetItemText(item).mb_str());

		// Write the type of the item
		switch (data->GetType())
		{
			case SnippetItemData::TYPE_CATEGORY:
				element.SetAttribute("type", "category");
			break;

			case SnippetItemData::TYPE_SNIPPET:
				element.SetAttribute("type", "snippet");
			break;

			default:
			break;
		}

		// And the snippet
		if (data->GetType() == SnippetItemData::TYPE_SNIPPET)
		{
			TiXmlElement snippetElement("snippet");
			TiXmlText snippetElementText(data->GetSnippet().mb_str());

            snippetElement.InsertEndChild(snippetElementText);

			element.InsertEndChild(snippetElement);
		}

		// Check if this item has children
		if(m_SnippetsTreeCtrl->ItemHasChildren(item))
		{
			// If it has, check those too
			SaveSnippets(&element, item);
		}

		// Insert the item we created as parent node's child
		parentNode->InsertEndChild(element);

		// Check the next child
		item = m_SnippetsTreeCtrl->GetNextChild(parentID, cookie);
	}
} // end of SaveSnippets

void CodeSnippetsWindow::LoadSnippets(const TiXmlElement* node, const wxTreeItemId& parentID)
{
	for (; node; node = node->NextSiblingElement())
	{
		// Check if the node has attributes
		const char* name = node->Attribute("name");
		if (!name)
		{
			Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Error loading XML file; attribute \"name\" cannot be found."));
			return;
		}

		const char* type = node->Attribute("type");
		if (!type)
		{
			Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Error loading XML file; attribute \"type\" cannot be found."));
			return;
		}

		const wxString itemName(cbC2U(name));
		const wxString itemType(cbC2U(type));

		// Check the item type
		if (itemType == _T("category"))
		{
			// Add new category
			wxTreeItemId newItemId = AddCategory(parentID, itemName, false);

			// Load the child items
			if (!node->NoChildren())
			{
				LoadSnippets(node->FirstChildElement(), newItemId);
			}
		}
		else if (itemType == _T("snippet"))
		{
			// Get the snippet
			if (const TiXmlElement* snippetElement = node->FirstChildElement("snippet"))
			{
				if (const TiXmlNode* snippetElementText = snippetElement->FirstChild())
				{
					if (snippetElementText->ToText())
					{
						AddCodeSnippet(parentID, itemName, cbC2U(snippetElementText->Value()), false);
					}
				}
				else
				{
					// Create a new snippet with no code in it
					AddCodeSnippet(parentID, itemName, wxEmptyString, false);
				}
			}
			else
			{
				Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Error loading XML file; element \"snippet\" cannot be found."));
			}
		}
		else
		{
			// Unknown
			Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Error loading XML file; attribute \"type\" returned \"") + itemType + _T("\" which is invalid item type."));
			return;
		}
	} // end for
} // end of LoadSnippets

void CodeSnippetsWindow::OnMnuImportSnippets(wxCommandEvent& /*event*/)
{
	wxFileDialog dlg(this, _T("Import from file"), _T(""), _T(""), _T("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxOPEN|wxFILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
		LoadSnippetsFromFile(dlg.GetPath());
	}
} // end of OnMnuImportSnippets

void CodeSnippetsWindow::OnMnuExportSnippets(wxCommandEvent& /*event*/)
{
	wxFileDialog dlg(this, _T("Export to XML"), _T(""), _T("codesnippets.xml"), _T("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		SaveSnippetsToFile(dlg.GetPath());
	}
} // end of OnMnuExportSnippets

void CodeSnippetsWindow::OnEndLabelEdit(wxTreeEvent& event)
{
	// Sort all the parent item's children
	m_SnippetsTreeCtrl->SortChildren(m_SnippetsTreeCtrl->GetItemParent(event.GetItem()));
} // end of OnEndLabelEdit

void CodeSnippetsWindow::SaveSnippetsToFile(const wxString& fileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	doc.InsertEndChild(header);

	TiXmlElement snippetsElement("snippets");
	SaveSnippets(&snippetsElement, m_RootID);

	doc.InsertEndChild(snippetsElement);

	doc.SaveFile(fileName.mb_str());
} // end of SaveSnippetsToFile

void CodeSnippetsWindow::LoadSnippetsFromFile(const wxString& fileName)
{
	if (wxFileExists(fileName))
	{
		TiXmlDocument doc;

		if (doc.LoadFile(fileName.mb_str()))
		{
			TiXmlElement* root = doc.RootElement();
			if (root)
			{
				// Get the first element
				TiXmlElement* firstChild = root->FirstChildElement("item");
				if (firstChild)
				{
					LoadSnippets(firstChild, m_RootID);
				}
				else
				{
					Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\" because first child element cannot be found (malformed XML?)."));
				}
			}
			else
			{
				Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\" because root element cannot be found (malformed XML?)."));
			}
		}
		else
		{
			wxString errorMsg = _T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ");
			errorMsg << cbC2U(doc.ErrorDesc());
			Manager::Get()->GetMessageManager()->DebugLog(errorMsg);
		}
	}
} // end of LoadSnippetsFromFile

bool SnippetsDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
	// Set focus to the Code snippets window
	m_Window->SetFocus();

	wxPoint p(x, y);
	int hitTestFlags = 0;

	// Find out if there's a tree item under the coordinates
	wxTreeItemId itemID = m_Window->m_SnippetsTreeCtrl->HitTest(p, hitTestFlags);

	if (hitTestFlags & wxTREE_HITTEST_ONITEMBUTTON|wxTREE_HITTEST_ONITEMICON|wxTREE_HITTEST_ONITEMLABEL)
	{
		// Find out the item type
		if (SnippetItemData* item = (SnippetItemData*)(m_Window->m_SnippetsTreeCtrl->GetItemData(itemID)))
		{
			switch (item->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					// Add new code snippet to the root
					m_Window->AddCodeSnippet(m_Window->m_RootID, _("New snippet"), data, true);
				break;

				case SnippetItemData::TYPE_CATEGORY:
					// Add new code snippet to the category
					m_Window->AddCodeSnippet(item->GetId(), _("New snippet"), data, true);
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
