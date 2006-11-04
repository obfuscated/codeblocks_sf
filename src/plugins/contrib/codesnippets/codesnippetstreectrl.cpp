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
	#include "manager.h"
	#include "messagemanager.h"
	#include "globals.h"
#endif

#include <tinyxml/tinyxml.h>

#include "snippetitemdata.h"
#include "codesnippetstreectrl.h"

IMPLEMENT_DYNAMIC_CLASS(CodeSnippetsTreeCtrl, wxTreeCtrl)

CodeSnippetsTreeCtrl::CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	//ctor
}

CodeSnippetsTreeCtrl::~CodeSnippetsTreeCtrl()
{
	//dtor
}

int CodeSnippetsTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
	// Get the items' data first
	const SnippetItemData* data1 = (SnippetItemData*)(GetItemData(item1));
	const SnippetItemData* data2 = (SnippetItemData*)(GetItemData(item2));


	if (data1 && data2)
	{
		int compareIndex1 = 0;
		switch (data1->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				compareIndex1 = 0;
			break;

			case SnippetItemData::TYPE_CATEGORY:
				compareIndex1 = 1;
			break;

			case SnippetItemData::TYPE_SNIPPET:
				compareIndex1 = 2;
			break;
		}
		int compareIndex2 = 0;
		switch (data2->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				compareIndex2 = 0;
			break;

			case SnippetItemData::TYPE_CATEGORY:
				compareIndex2 = 1;
			break;

			case SnippetItemData::TYPE_SNIPPET:
				compareIndex2 = 2;
			break;
		}

		// Compare the types
		if (compareIndex1 == compareIndex2)
		{
			// If they are both the same type, just use normal compare
			return wxTreeCtrl::OnCompareItems(item1, item2);
		}
		else if (compareIndex1 > compareIndex2)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

void CodeSnippetsTreeCtrl::SaveItemsToXmlNode(TiXmlNode* node, const wxTreeItemId& parentID)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(parentID, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		const SnippetItemData* data = (SnippetItemData*)(GetItemData(item));

		if (!data)
		{
			return;
		}

		// Begin item element
		TiXmlElement element("item");

		// Write the item's name
		element.SetAttribute("name", cbU2C(GetItemText(item)));

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
			TiXmlText snippetElementText(cbU2C(data->GetSnippet()));

            snippetElement.InsertEndChild(snippetElementText);

			element.InsertEndChild(snippetElement);
		}

		// Check if this item has children
		if(ItemHasChildren(item))
		{
			// If it has, check those too
			SaveItemsToXmlNode(&element, item);
		}

		// Insert the item we created as parent node's child
		node->InsertEndChild(element);

		// Check the next child
		item = GetNextChild(parentID, cookie);
	}
} // end of SaveSnippets

void CodeSnippetsTreeCtrl::LoadItemsFromXmlNode(const TiXmlElement* node, const wxTreeItemId& parentID)
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
				LoadItemsFromXmlNode(node->FirstChildElement(), newItemId);
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

void CodeSnippetsTreeCtrl::SaveItemsToFile(const wxString& fileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	doc.InsertEndChild(header);

	TiXmlElement snippetsElement("snippets");
	SaveItemsToXmlNode(&snippetsElement, GetRootItem());

	doc.InsertEndChild(snippetsElement);

	doc.SaveFile(fileName.mb_str());
} // end of SaveSnippetsToFile

void CodeSnippetsTreeCtrl::LoadItemsFromFile(const wxString& fileName)
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
					LoadItemsFromXmlNode(firstChild, GetRootItem());
				}
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

// NOTE : nobody uses the return vlaue -> could remove it
wxTreeItemId CodeSnippetsTreeCtrl::AddCodeSnippet(const wxTreeItemId& parent, wxString title, wxString codeSnippet, bool editNow)
{
	wxTreeItemId newItemID = InsertItem(parent, GetLastChild(parent), title, 2, -1, new SnippetItemData(SnippetItemData::TYPE_SNIPPET, codeSnippet));

	// Sort 'em
	SortChildren(parent);

	if (editNow)
	{
		// Let the user to edit the item
		EnsureVisible(newItemID);
		EditLabel(newItemID);
	}

	return newItemID;
}

wxTreeItemId CodeSnippetsTreeCtrl::AddCategory(const wxTreeItemId& parent, wxString title, bool editNow)
{
	wxTreeItemId newCategoryID = InsertItem(parent, GetLastChild(parent), title, 1, -1, new SnippetItemData(SnippetItemData::TYPE_CATEGORY));

	// Sort 'em
	SortChildren(parent);

	if (editNow)
	{
		// Let the user to edit the category
		EnsureVisible(newCategoryID);
		EditLabel(newCategoryID);
	}

	return newCategoryID;
}
