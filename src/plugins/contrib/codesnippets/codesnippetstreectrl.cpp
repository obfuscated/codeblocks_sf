/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

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
// RCS-ID: $Id$

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
    #include <wx/msgdlg.h>
#endif
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/dataobj.h>
    #include <wx/dnd.h>
    #include <wx/filename.h>
    #include "wx/mimetype.h"

//-#if defined(BUILDING_PLUGIN)
    #include "sdk.h"
    #ifndef CB_PRECOMP
        #include "manager.h"
        #include "logmanager.h"
        #include "globals.h"
    #endif
//-#else
//-#endif

#include <tinyxml/tinyxml.h>
#include "snippetitemdata.h"
#include "codesnippetstreectrl.h"
#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "GenericMessageBox.h"
#include "menuidentifiers.h"
#include "editsnippetframe.h"
#include "codesnippetsevent.h"
#include "snippetsconfig.h"
#include "dragscrollevent.h"
#include "FileImport.h"
#include "csutils.h"
#include "version.h"

IMPLEMENT_DYNAMIC_CLASS(CodeSnippetsTreeCtrl, wxTreeCtrl)

BEGIN_EVENT_TABLE(CodeSnippetsTreeCtrl, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(idSnippetsTreeCtrl, CodeSnippetsTreeCtrl::OnBeginTreeItemDrag)
	EVT_TREE_END_DRAG(idSnippetsTreeCtrl,   CodeSnippetsTreeCtrl::OnEndTreeItemDrag)

	EVT_LEAVE_WINDOW( CodeSnippetsTreeCtrl::OnLeaveWindow)
	EVT_ENTER_WINDOW( CodeSnippetsTreeCtrl::OnEnterWindow)
	//-EVT_MOTION(       CodeSnippetsTreeCtrl::OnMouseMotionEvent)
	//-EVT_MOUSEWHEEL(   CodeSnippetsTreeCtrl::OnMouseWheelEvent)
	//-EVT_LEFT_UP(      CodeSnippetsTreeCtrl::OnMouseLeftUpEvent)
	//-EVT_LEFT_DOWN(    CodeSnippetsTreeCtrl::OnMouseLeftDownEvent)

	EVT_TREE_SEL_CHANGED(idSnippetsTreeCtrl,CodeSnippetsTreeCtrl::OnItemSelected)
	EVT_TREE_ITEM_RIGHT_CLICK(idSnippetsTreeCtrl, CodeSnippetsTreeCtrl::OnItemRightSelected)
	//-EVT_IDLE(                               CodeSnippetsTreeCtrl::OnIdle)
	//- EVT_IDLE replaced by call from plugin|appframe OnIdle routine
    // --
    EVT_CODESNIPPETS_SELECT(wxID_ANY,    CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_Select)
    EVT_CODESNIPPETS_EDIT  (wxID_ANY,    CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_Edit)
    EVT_CODESNIPPETS_GETFILELINKS(wxID_ANY,CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_GetFileLinks)

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsTreeCtrl::CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id,
                                const wxPoint& pos, const wxSize& size, long style)
// ----------------------------------------------------------------------------
	//-: wxTreeCtrl(parent, id, pos, size, style)
	: wxTreeCtrl(parent, id, pos, size, style, wxDefaultValidator, wxT("csTreeCtrl"))
{
    m_fileChanged = false;
    m_bMouseExitedWindow = false;
    m_pPropertiesDialog = 0;
    m_bShutDown = false;
    m_mimeDatabase = 0;
    m_bBeginInternalDrag = false;
    m_LastXmlModifiedTime = time_t(0);            //2009/03/15
    #if !wxCHECK_VERSION(2, 8, 12)
    m_itemAtKeyUp = m_itemAtKeyDown = 0;
    #endif


    m_pSnippetsTreeCtrl = this;
    GetConfig()->SetSnippetsTreeCtrl(this);

    m_pDragCursor = new wxCursor(wxCURSOR_HAND);
    m_bDragCursorOn = false;
    m_oldCursor = GetCursor();

}
// ----------------------------------------------------------------------------
CodeSnippetsTreeCtrl::~CodeSnippetsTreeCtrl()
// ----------------------------------------------------------------------------
{
    //dtor
    // Tell all that TreeCtrl is gone;
    GetConfig()->SetSnippetsTreeCtrl(0);

}
// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::IsFileSnippet (wxTreeItemId treeItemId  )
// ----------------------------------------------------------------------------
{
    wxTreeItemId itemId = treeItemId;
    if ( itemId == (void*)0) itemId = GetSelection();
    if (not itemId.IsOk()) return false;
    if (not IsSnippet(itemId) ) return false;
    wxString fileName = GetSnippet(itemId).BeforeFirst('\r');
    fileName = fileName.BeforeFirst('\n');
    // substitute $macros with actual text
    //-#if defined(BUILDING_PLUGIN)
        static const wxString delim(_T("$%["));
        if( fileName.find_first_of(delim) != wxString::npos )
            Manager::Get()->GetMacrosManager()->ReplaceMacros(fileName);
        //-LOGIT( _T("$macros name[%s]"),fileName.c_str() );
    //-#endif

    if ( not ::wxFileExists( fileName) ) return false;
    return true;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::IsFileLinkSnippet (wxTreeItemId treeItemId  )
// ----------------------------------------------------------------------------
{
    wxTreeItemId itemId = treeItemId;
    if ( itemId == (void*)0) itemId = GetSelection();
    if (not itemId.IsOk()) return false;
    if (not IsSnippet(itemId) ) return false;
    wxString fileName = GetSnippet(itemId).BeforeFirst('\r');
    fileName = fileName.BeforeFirst('\n');
    // substitute $macros with actual text
    //-#if defined(BUILDING_PLUGIN)
        static const wxString delim(_T("$%["));
        if( fileName.find_first_of(delim) != wxString::npos )
            Manager::Get()->GetMacrosManager()->ReplaceMacros(fileName);
        //-LOGIT( _T("$macros name[%s]"),fileName.c_str() );
    //-#endif
    if (fileName.Length() > 128)
    {   // if text is > 128 characters, not a filelink.
        return false;
    }
    if ( not ::wxFileExists( fileName) ) return false;
    return true;

}//IsFileLinkSnippet
// ----------------------------------------------------------------------------
wxString CodeSnippetsTreeCtrl::GetFileLinkExt (wxTreeItemId treeItemId  )
// ----------------------------------------------------------------------------
{
    //return filename extension

    if ( not IsFileLinkSnippet(treeItemId) ) return wxT("");
    wxTreeItemId itemId = treeItemId;
    if ( itemId == (void*)0) itemId = GetSelection();
    if (not itemId.IsOk()) return wxT("");
    if (not IsSnippet(itemId) ) return wxT("");
    wxString fileName = GetSnippet(itemId).BeforeFirst('\r');
    fileName = fileName.BeforeFirst('\n');
    // substitute $macros with actual text
    //-#if defined(BUILDING_PLUGIN)
        static const wxString delim(_T("$%["));
        if( fileName.find_first_of(delim) != wxString::npos )
            Manager::Get()->GetMacrosManager()->ReplaceMacros(fileName);
        //-LOGIT( _T("$macros name[%s]"),fileName.c_str() );
    //-#endif
    if ( not ::wxFileExists( fileName) ) return wxT("");
    wxFileName filename(fileName);
    return filename.GetExt();
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnItemSelectChanging(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // UNUSED
    //-if (m_pTopDialog) event.Veto();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnItemSelected(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{                                                           //(pecan 2006/9/12)
    // User selected a tree item. Memorize the selection for
    // other routines.

    //CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();
    //wxTreeItemId itemId = event.GetItem();

    //-SetStatusText(wxEmptyString);

    // Get the item associated with the event
	if (const SnippetItemData* eventItem =
        (SnippetItemData*)GetItemData(event.GetItem()))
	{
        wxString itemDescription;
		// Check the type of the item and add the menu items
		switch (eventItem->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
			break;

			case SnippetItemData::TYPE_CATEGORY:
			break;

			case SnippetItemData::TYPE_SNIPPET:
			//-SetStatusText( GetSnippetDescription(itemId) );
			break;
		}

		// Save the item ID for later use
		m_MnuAssociatedItemID = eventItem->GetId();

	}

}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnItemRightSelected(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // on wx2.8.3, Right click does not select the item.
    // The selection is still on the previous item.
    // So we'll select it ourself.

    SelectItem(event.GetItem());                                                         //(pecan 2006/9/12)
}
// ----------------------------------------------------------------------------
int CodeSnippetsTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FindTreeItemByLabel(const wxString& searchTerms, const wxTreeItemId& node, int requestType)
// ----------------------------------------------------------------------------
{

    // Return a tree item id matching string

	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(node, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetItemData(item)))
		{
			bool ignoreThisType = false;

			switch (itemData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisType = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
					if (requestType == CodeSnippetsConfig::SCOPE_CATEGORIES)
					{
						ignoreThisType = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if (requestType == CodeSnippetsConfig::SCOPE_SNIPPETS)
					{
						ignoreThisType = true;
					}
				break;
			}

			if (!ignoreThisType)
			{
				wxString label = GetItemText(item);

				if(0 == label.Cmp(searchTerms))
				{
					return item;
				}
			}

			if(ItemHasChildren(item))
			{
				wxTreeItemId search = FindTreeItemByLabel(searchTerms, item, requestType);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetNextChild(node, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem = (void*)(0);
   return dummyItem;
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FindTreeItemByTreeId(const wxTreeItemId& itemToFind, const wxTreeItemId& startNode, int itemToFindType)
// ----------------------------------------------------------------------------
{
    // Return a tree item matching item reference

	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(startNode, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetItemData(item)))
		{
			bool ignoreThisType = false;

			switch (itemData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisType = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
					if (itemToFindType == CodeSnippetsConfig::SCOPE_CATEGORIES)
					{
						ignoreThisType = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if (itemToFindType == CodeSnippetsConfig::SCOPE_SNIPPETS)
					{
						ignoreThisType = true;
					}
				break;
			}

			if (!ignoreThisType)
			{
				wxString label = GetItemText(item);

				if( itemToFind == item)
				{
					return item;
				}
			}

			if(ItemHasChildren(item))
			{
				wxTreeItemId search = FindTreeItemByTreeId(itemToFind, item, itemToFindType);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetNextChild(startNode, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem = (void*)(0);
   return dummyItem;
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FindTreeItemBySnippetId(const SnippetItemID& IDToFind, const wxTreeItemId& startNode)
// ----------------------------------------------------------------------------
{
    // Return a tree item matching item reference

    static wxTreeItemId dummyItem = (void*)(0);
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(startNode, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetItemData(item)))
		{
			bool ignoreThisItem = false;

            #if defined(LOGGING)
            //LOGIT( _T("itemToFind[%d] ItemID[%d],Tree[%s],Snippet[%s]"),
            //    IDToFind, itemData->GetID(), GetItemText(item).c_str(), itemData->GetSnippet().Left(16).c_str());
            #endif

			switch (itemData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisItem = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
					if ( IDToFind not_eq itemData->GetID() )
					{
						ignoreThisItem = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if ( IDToFind not_eq itemData->GetID() )
					{
						ignoreThisItem = true;
					}
				break;
			}

			if (!ignoreThisItem)
			{
				wxString label = GetItemText(item);

				if( IDToFind == itemData->GetID() )
				{
					return item;
				}
			}

			if ( ItemHasChildren(item) )
			{
				wxTreeItemId search = FindTreeItemBySnippetId( IDToFind, item );
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetNextChild(startNode, cookie);

            //if ( (item = GetNextSibling( item ) ))
            //    continue;
		}//if snippet item
	}//while ok tree item

   // Return dummy item if search string was not found
   return dummyItem;
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveItemsToXmlNode(TiXmlNode* node, const wxTreeItemId& parentID)
// ----------------------------------------------------------------------------
{
    // Translate tree items to xml data

	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(parentID, cookie);

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		if (const SnippetItemData* data = (SnippetItemData*)(GetItemData(item)))
		{
			// Begin item element
			TiXmlElement element("item");

			// Write the item's name
			element.SetAttribute("name", csU2C(GetItemText(item)));

			if (data->GetType() == SnippetItemData::TYPE_CATEGORY)
			{
				// Category
				element.SetAttribute("type", "category");
				element.SetAttribute("ID", csU2C(data->GetSnippetIDStr()) );

				// Check if this category has children
				if(ItemHasChildren(item))
				{
					// If it has, check those too
					SaveItemsToXmlNode(&element, item);
				}
			}
			else
			{
				// Snippet
				element.SetAttribute("type", "snippet");
				element.SetAttribute("ID", csU2C(data->GetSnippetIDStr()) );

				TiXmlElement snippetElement("snippet");
				TiXmlText snippetElementText(csU2C(data->GetSnippet()));

				snippetElement.InsertEndChild(snippetElementText);

				element.InsertEndChild(snippetElement);
			}

			// Insert the item we created as parent node's child
			node->InsertEndChild(element);

			// Check the next child
			item = GetNextChild(parentID, cookie);
		}
		else
		{
			return;
		}
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::LoadItemsFromXmlNode(const TiXmlElement* node, const wxTreeItemId& parentID)
// ----------------------------------------------------------------------------
{
    // Translate xml data to tree data

	for (; node; node = node->NextSiblingElement())
	{
		// Check if the node has attributes
		const wxString itemName(csC2U(node->Attribute("name")));
		const wxString itemType(csC2U(node->Attribute("type")));
		const wxString itemIDstr(csC2U(node->Attribute("ID")));
		long itemID;
		itemIDstr.ToLong(&itemID);

		// Check the item type
		if (itemType == _T("category"))
		{
			// Add new category
			wxTreeItemId newCategoryId = AddCategory(parentID, itemName, itemID, /*editNow*/false);

			// Load the child items
			if (!node->NoChildren())
			{
				LoadItemsFromXmlNode(node->FirstChildElement(), newCategoryId);
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
						AddCodeSnippet(parentID, itemName, csC2U(snippetElementText->Value()), itemID,  /*editNow*/false);
					}
				}
				else
				{
					// Create a new snippet with no code in it
					AddCodeSnippet(parentID, itemName, wxEmptyString, itemID, /*editNow*/false);
				}
			}
			else
			{
                GenericMessageBox(_T("CodeSnippets: Error loading XML file; element \"snippet\" cannot be found."));
			}
		}
		else
		{
		    GenericMessageBox(_T("CodeSnippets: Error loading XML file; attribute \"type\" is \"") + itemType + _T("\" which is invalid item type."));
			return;
		}
	} // end for
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveItemsToFile(const wxString& fileName)
// ----------------------------------------------------------------------------
{
    // This routine also called from codesnippets.cpp::OnRelease()

    // verify all dir levels exist
    CreateDirLevels(fileName);

    SnippetItemData::SetHighestSnippetID( 0 );
    ResetSnippetsIDs( GetRootItem() );

	TiXmlDocument doc;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	doc.InsertEndChild(header);

	TiXmlComment comment;
	comment.SetValue("Trust me. I know what I'm doing.");
	doc.InsertEndChild(comment);

	TiXmlElement snippetsElement("snippets");
	SaveItemsToXmlNode(&snippetsElement, GetRootItem());

	doc.InsertEndChild(snippetsElement);

	int rc = doc.SaveFile(fileName.mb_str());
    if(not rc)
    {   wxString msg = wxString::Format(_T("ERROR: Failed to save %s"), fileName.c_str());
        wxMessageBox(msg, _T("ERROR"));
    }
	SetFileChanged(false);
	SnippetItemData::SetSnippetsItemsChangedCount(0);
	FetchFileModificationTime();

    // Give Snippets Search the .xml index filename
    // indicating possible content changes
    CodeSnippetsEvent evt( wxEVT_CODESNIPPETS_NEW_INDEX, 0);
    evt.SetSnippetString( GetConfig()->SettingsSnippetsXmlPath );
    evt.PostCodeSnippetsEvent(evt);

    #ifdef LOGGING
     if (rc) LOGIT( _T("File saved:[%s]"),fileName.c_str() );
    #endif //LOGGING
}

// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::LoadItemsFromFile(const wxString& fileName, bool bAppendItems)
// ----------------------------------------------------------------------------
{
	if (!bAppendItems)
    {
        DeleteChildren( GetRootItem() );
        SnippetItemData::SetHighestSnippetID( 0 );
        SnippetItemData::SetSnippetsItemsChangedCount(0);
    }

    bool retcode = true;
	if (wxFileExists(fileName))
	{
		// Don't remove any whitespace
		TiXmlBase::SetCondenseWhiteSpace(false);

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
		    retcode = false;
			// Create a backup copy of the offending file
			wxString backupFile = fileName;
			backupFile.Append(_T(".bak"));

			// Overwrite it
			wxCopyFile(fileName, backupFile, true);
           //-#if defined(BUILDING_PLUGIN)
           if ( GetConfig()->IsPlugin() )
           {
                Manager::Get()->GetLogManager()->DebugLog(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + csC2U(doc.ErrorDesc()));
                Manager::Get()->GetLogManager()->DebugLog(_T("CodeSnippets: Backup of the failed file has been created."));
           }
		   //-#else
		   else //IsApplication
		   {
                //-wxMessageBox(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + csC2U(doc.ErrorDesc()));
                GenericMessageBox(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + csC2U(doc.ErrorDesc()));
                //-wxMessageBox(_T("CodeSnippets: Backup of the failed file has been created."));
                GenericMessageBox(_T("CodeSnippets: Backup of the failed file has been created."));
		   }
		   //-#endif
		}
	}
	// Show the first level of items
	if (  GetRootItem() && GetRootItem().IsOk())
        Expand( GetRootItem() );

    // show filename window banner
    wxString nameOnly;
    wxFileName::SplitPath( fileName, 0, &nameOnly, 0);
    // Edit the root node's title so that the user sees file name
    SetItemText( GetRootItem(), wxString::Format(_("%s"), nameOnly.GetData()));

    if (not SnippetItemData::GetSnippetsItemsChangedCount() )
        SetFileChanged(false);
    FetchFileModificationTime();

    // Issue event for new snippets index file
    CodeSnippetsEvent evt( wxEVT_CODESNIPPETS_NEW_INDEX, 0);
    evt.SetSnippetString( fileName );
    evt.PostCodeSnippetsEvent(evt);

    return retcode;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::AddCodeSnippet(const wxTreeItemId& parent,
                            wxString title, wxString codeSnippet, long ID, bool editNow)
// ----------------------------------------------------------------------------
{
    SnippetItemData* pSnippetDataItem = new SnippetItemData(SnippetItemData::TYPE_SNIPPET, codeSnippet, ID);
	wxTreeItemId newItemID = InsertItem(parent, GetLastChild(parent), title, 2, -1,
                pSnippetDataItem );

	// Sort 'em
	SortChildren(parent);

    bool ok = false;
    if ( newItemID && newItemID.IsOk() ) ok = true;
	if (editNow)
	{
		// Let the user edit the item
		EnsureVisible(newItemID);
		////-EditLabel(newItemID);
		ok = EditSnippetProperties( newItemID );
		if (not ok) RemoveItem(newItemID);
		else SelectItem( newItemID );
	}

	if ( ok && newItemID.IsOk() )
    {   SetSnippetImage(newItemID);
        SetFileChanged(true);
    }

}

// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::AddCategory(const wxTreeItemId& parent, wxString title, long ID, bool editNow)
// ----------------------------------------------------------------------------
{
	wxTreeItemId newCategoryID = InsertItem(parent, GetLastChild(parent), title, 1, -1,
            new SnippetItemData(SnippetItemData::TYPE_CATEGORY, ID));

	// Sort 'em
	SortChildren(parent);

	if (editNow)
	{
		// Let the user edit the category
		EnsureVisible(newCategoryID);
		EditLabel(newCategoryID);
        SetFileChanged(true);
	}

	return newCategoryID;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::RemoveItem(const wxTreeItemId RemoveItemId)
// ----------------------------------------------------------------------------
{
	// Get the associated item id
	wxTreeItemId itemId = RemoveItemId;
	// Sanity checks
	if (not itemId.IsOk()) return false;
	if (itemId == GetRootItem() ) return false;

    SnippetItemData* pItemData = (SnippetItemData*)(GetItemData(itemId));
    if (not pItemData) return false;

    bool shiftKeyIsDown = ::wxGetKeyState(WXK_SHIFT);
    wxString itemText = GetItemText(RemoveItemId);

    // delete unused items directly (don't ".trash" them)
    bool canceledItem = false;
    if ( itemText.IsSameAs(wxT("New category")) || itemText.IsSameAs(wxT("New snippet")) )
    {
        canceledItem = true;
        shiftKeyIsDown = true;
    }
    bool trashItem = false;

    // if shift key is up, copy item to .trash category
    if (not shiftKeyIsDown)
    {
        // put deleted items in .trash category
        wxTreeItemId trashId = FindTreeItemByLabel(wxT(".trash"), GetRootItem(), CodeSnippetsConfig::SCOPE_CATEGORIES);
        if ( trashId==(void*)0 )
            trashId = AddCategory(GetRootItem(), wxT(".trash"), /*itemID*/0, /*editNow*/false);

        // if item is NOT already in the trash, copy item to .trash category
        if (not ( FindTreeItemByTreeId( itemId, trashId, pItemData->GetType()) ))
        {
            TiXmlDocument* pDoc =  CopyTreeNodeToXmlDoc( itemId);
            CopyXmlDocToTreeNode(pDoc, trashId);
            delete pDoc;
        }
        else // item already in .trash, delete it
        {
            trashItem = true;
        }
    }//if not shiftKeyIsDown

    // when in trash already, or immediate delete requested, check for fileLink delete too
    if ( not canceledItem )    //not "New category", not "New snippet"
    if (trashItem || shiftKeyIsDown )
    {
        // if FileLink, memorize the filename
        wxString filename = wxEmptyString;
        if ( IsFileSnippet(itemId) )
           filename = GetSnippetFileLink(itemId);

        // if this was a FileLink, ask if user wants to delete file
        if ( not filename.IsEmpty() ) {
            int answer = GenericMessageBox( wxT("Delete physical file?\n\n")+filename,
                                                    wxT("Delete"),wxYES_NO );
            if ( answer == wxYES)
                /*int done =*/ ::wxRemoveFile(filename);
        }
    }//if trashItem

    // Delete the original Snippet Tree Item
    DeleteChildren(itemId);
    Delete(itemId);
    SetFileChanged(true);

	return true;
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SetSnippetImage(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
    // set the item tree image
    if ( IsFileSnippet(itemId) )
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_FILE);
    else if (IsUrlSnippet(itemId))
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_URL);
    else
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_TEXT);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::FetchFileModificationTime(wxDateTime savedTime)
// ----------------------------------------------------------------------------
{
    if (savedTime != time_t(0))
    {   m_LastXmlModifiedTime = savedTime;
        return;
    }
    if (::wxFileExists(GetConfig()->SettingsSnippetsXmlPath) )
    {   wxFileName fname( GetConfig()->SettingsSnippetsXmlPath );
        m_LastXmlModifiedTime = fname.GetModificationTime();
    }
}
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::BeginInternalTreeItemDrag()
//// ----------------------------------------------------------------------------
//{
//    CodeSnippetsTreeCtrl* pTree = this;
//
//    if (not m_itemAtKeyDown)
//        return;
//    #ifdef LOGGING
//     LOGIT( _T("TREE_BEGIN_DRAG %p"), pTree );
//    #endif //LOGGING
//
//    m_bBeginInternalDrag = true;
//    m_TreeItemId = m_itemAtKeyDown;
//    m_MnuAssociatedItemID = m_TreeItemId;
//    m_TreeMousePosn = wxPoint(m_MouseUpX, m_MouseUpY);
//    m_TreeText = pTree->GetSnippet(m_TreeItemId);
//    if ( IsCategory(m_TreeItemId) )
//        m_TreeText = GetSnippetLabel(m_TreeItemId);
//    if (m_TreeText.IsEmpty())
//        m_bBeginInternalDrag = 0;
//
//    return;
//}//OnBeginInternalTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnBeginTreeItemDrag(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    CodeSnippetsTreeCtrl* pTree = this;

    #ifdef LOGGING
     LOGIT( _T("TREE_BEGIN_DRAG %p"), pTree );
    #endif //LOGGING

    m_bBeginInternalDrag = true;
    m_TreeItemId = event.GetItem();
    m_itemAtKeyDown = m_TreeItemId;
    m_MnuAssociatedItemID = m_TreeItemId;
    m_TreeMousePosn = event.GetPoint();
    m_TreeText = pTree->GetSnippet(m_TreeItemId);
    if ( IsCategory(m_TreeItemId) )
        m_TreeText = GetSnippetLabel(m_TreeItemId);
    if (m_TreeText.IsEmpty())
        m_bBeginInternalDrag = 0;

    event.Allow();

    return;
}//OnBeginTreeItemDrag
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::EndInternalTreeItemDrag()
//// ----------------------------------------------------------------------------
//{
//    // -----------------------
//    // TREE_END_DRAG
//    // -----------------------
//
//    if (not m_itemAtKeyUp)
//        return;
//
//    #ifdef LOGGING
//     wxTreeCtrl* pTree = this;
//     LOGIT( _T("TREE_END_DRAG %p"), pTree );
//    #endif //LOGGING
//
//    wxTreeItemId targetItem = m_itemAtKeyUp;
//    wxTreeItemId sourceItem = m_itemAtKeyDown;
//    if ( not sourceItem.IsOk() ){return;}
//    if ( not targetItem.IsOk() ){return;}
//    if (not m_bBeginInternalDrag)
//        return;
//
//    // If user dragged item out of the window, FinishExternlDrag()
//    // will handle it. So just clear the status and return.
//    if (m_bMouseExitedWindow)
//        return;
//
//    // if source and target are snippets, create a new category and enclose both.
//    if ( IsSnippet(targetItem) )
//    {
//        if ( (targetItem = ConvertSnippetToCategory(targetItem)) )
//        {;}//ok
//        else return;
//        // now targetItem is a category
//    }
//
//
//    // Save the source item node to a new Xml Document
//    // Load it into the target item node
//    // delete the source item node
//
//    // create Xml document from source(dragged) tree item
//    TiXmlDocument* pDoc = CopyTreeNodeToXmlDoc( sourceItem );
//    if (not pDoc) return;
//
//    TiXmlElement* root = pDoc->RootElement();
//    if (root)
//    {
//        // Get the source xml element
//        TiXmlElement* firstChild = root->FirstChildElement("item");
//        if (firstChild)
//        {   // insert into target Tree items from source xml document
//            LoadItemsFromXmlNode( firstChild, targetItem);
//        }
//    }
//    // remove the old tree item
//    if (not m_bMouseCtrlKeyDown){
//            RemoveItem( sourceItem );
//    }
//
//    delete pDoc; pDoc = 0;
//    m_itemAtKeyDown = 0;
//    m_itemAtKeyUp = 0;
//
//    return;
//
//}//OnEndTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EndInternalTreeItemDrag()
// ----------------------------------------------------------------------------
{
    // -----------------------
    // TREE_END_DRAG
    // -----------------------

    if (not m_itemAtKeyUp)
        return;

    #ifdef LOGGING
     wxTreeCtrl* pTree = this;
     LOGIT( _T("TREE_END_DRAG %p"), pTree );
    #endif //LOGGING

    wxTreeItemId targetItem = m_itemAtKeyUp;
    wxTreeItemId sourceItem = m_itemAtKeyDown;
    if ( not sourceItem.IsOk() ){return;}
    if ( not targetItem.IsOk() ){return;}
    if (not m_bBeginInternalDrag)
        return;

    // If user dragged item out of the window, FinishExternlDrag()
    // will handle it. So just clear the status and return.
    if (m_bMouseExitedWindow)
        return;

    // if source and target are snippets, create a new category and enclose both.
    if ( IsSnippet(targetItem) )
    {
        if ( (targetItem = ConvertSnippetToCategory(targetItem)) )
        {;}//ok
        else return;
        // now targetItem is a category
    }


    // Save the source item node to a new Xml Document
    // Load it into the target item node
    // delete the source item node

    // create Xml document from source(dragged) tree item
    TiXmlDocument* pDoc = CopyTreeNodeToXmlDoc( sourceItem );
    if (not pDoc) return;

    TiXmlElement* root = pDoc->RootElement();
    if (root)
    {
        // Get the source xml element
        TiXmlElement* firstChild = root->FirstChildElement("item");
        if (firstChild)
        {   // insert into target Tree items from source xml document
            LoadItemsFromXmlNode( firstChild, targetItem);
        }
    }
    // remove the old tree item
    if (not m_bMouseCtrlKeyDown){
            RemoveItem( sourceItem );
    }

    delete pDoc; pDoc = 0;
    #if !wxCHECK_VERSION(2, 8, 12)
    m_itemAtKeyDown = 0;
    m_itemAtKeyUp = 0;
    #endif

    return;

}//OnEndTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnEnterWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    //*WARNING* child windows of wxAUI windows do not get common events.
    // unless the parent wxAUI window has/had focus.

    // a wxAUI window is not enabling the Tree ctrl unless the user specifically
    // clicks on a tree item. Its disabled otherwise, so when leaving
    // the disabled TreeCtrl, the CodeSnippets child Edit (scintilla editor)
    // gets no EVT_FOCUS_WINDOW and therefore shows no caret/cursor.
    // So here, we force the tree ctrl to focus so that the editor
    // gets a focus event also. Note also, that other common events are not
    // propagated when a wxAUI window is floating.

    #if defined(LOGGING)
    //LOGIT( _T("CodeSnippetsTreeCtrl::OnEnterWindow"));
    #endif

    // If the user is editing a label, don't refocus the control
    if ( not GetConfig()->GetSnippetsWindow()->IsEditingLabel() )
    if ( GetConfig()->IsFloatingWindow())
    {
        #if defined(LOGGING)
        //LOGIT( _T("CodeSnippetsCtrl IsFloatingWindow[%s]"), _T("TRUE"));
        #endif
        wxWindow* pw = (wxWindow*)event.GetEventObject();
        // only if forground is our window, set focus
        if ( pw == ::wxGetActiveWindow() )
        {   pw->Enable();
            pw->SetFocus();
        }
    }

    event.Skip();
}
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::OnLeaveWindow(wxMouseEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // -----------------------
//    // LEAVE_WINDOW
//    // -----------------------
//
//    // User has dragged mouse out of source window.
//    // if us is dragging mouse, save the source item for later use
//    // in FinishDrag()
//
//    #ifdef LOGGING
//     //LOGIT( _T("CodeSnippetsTreeCtrl::OnLeaveWindow") );
//    #endif //LOGGING
//
//  #if defined(__WXMSW__)
//    m_bBeginInternalDrag = false; //This is not an internal drag
//    // Left mouse key must be down (dragging)
//    if (not m_bMouseLeftKeyDown ) {event.Skip();return;}
//    if (not m_bMouseIsDragging ) {event.Skip();return;}
//    // check if data is available
//    if (not m_itemAtKeyDown) {event.Skip();return;}
//
//    CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();
//    m_TreeText = pTree->GetSnippet(m_itemAtKeyDown);
//    if ( IsCategory(m_itemAtKeyDown) )
//        m_TreeText = GetSnippetLabel(m_itemAtKeyDown);
//    if ( m_TreeText.IsEmpty()) {event.Skip();return;}
//
//    #ifdef LOGGING
//     LOGIT( _T("TreeCtrl LEAVE_WINDOW %p"), event.GetEventObject() );
//    #endif //LOGGING
//
//    // when user drags an item out of the window, this routine is called
//    // OnMouseKeyUpEvent will clear this flag
//    m_bMouseExitedWindow = true;
//    CaptureMouse();
//  #endif //__WXMSW__
//
//  #if defined(__WXGTK__)
//    if (m_bDragCursorOn)
//    {
//        SetCursor(m_oldCursor);
//        m_bDragCursorOn = false;
//    }
//  #endif //__WXGTK__
//
//    event.Skip();
//    return;
//}//OnLeaveWindow
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnLeaveWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // LEAVE_WINDOW
    // -----------------------

    // User has dragged mouse out of source window.
    // if us is dragging mouse, save the source item for later use
    // in FinishDrag()

    #ifdef LOGGING
     //LOGIT( _T("CodeSnippetsTreeCtrl::OnLeaveWindow") );
    #endif //LOGGING

    m_bBeginInternalDrag = false; //This is not an internal drag

    // Left mouse key must be down (dragging)
    // check if data is available
    if ( m_TreeText.IsEmpty()) return;

    //-CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();
    //-m_TreeText = pTree->GetSnippet(m_itemAtKeyDown);
    //-if ( IsCategory(m_itemAtKeyDown) )
    //-    m_TreeText = GetSnippetLabel(m_itemAtKeyDown);
    //-if ( m_TreeText.IsEmpty()) {event.Skip();return;}

    #ifdef LOGGING
     LOGIT( _T("TreeCtrl LEAVE_WINDOW %p"), event.GetEventObject() );
    #endif //LOGGING

    // when user drags an item out of the window, this routine is called
    // OnMouseKeyUpEvent will clear this flag
    m_bMouseExitedWindow = true;

    #if defined(__WXMSW__)
    if (m_bMouseExitedWindow
        and (not  m_TreeText.IsEmpty()))
    {
        FinishExternalDrag();
        SendMouseLeftUp(this, m_TreeMousePosn.x, m_TreeMousePosn.y);
    }
    #endif //__WXMSW__

    return;
}//OnLeaveWindow
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::FinishExternalDrag()
// ----------------------------------------------------------------------------
{//asm("int3"); /*trap*/
    m_bMouseExitedWindow = false;

    if ( m_TreeText.IsEmpty())
        return;

    // we now have data; create both a simple text and filename drop source
    wxTextDataObject* textData = new wxTextDataObject();
    wxFileDataObject* fileData = new wxFileDataObject();
        // fill text and file sources with snippet
    wxString textStr = GetSnippet(m_itemAtKeyDown) ;
    //-#if defined(BUILDING_PLUGIN)
        // substitute any $(macro) text
        static const wxString delim(_T("$%["));
        if( textStr.find_first_of(delim) != wxString::npos )
            Manager::Get()->GetMacrosManager()->ReplaceMacros(textStr);
        //-LOGIT( _T("SnippetsTreeCtrl OnLeaveWindow $macros text[%s]"),textStr.c_str() );
    //-#endif

    wxDropSource textSource( *textData, this );
    textData->SetText( textStr );

    wxDropSource fileSource( *fileData, this );
    wxString fileName = GetSnippetFileLink(m_itemAtKeyDown);
    if (not ::wxFileExists(fileName) ) fileName = wxEmptyString;
    // If no filename, but text is URL/URI, pass it as a file (esp. for browsers)
    if ( fileName.IsEmpty())
    {   if (textStr.StartsWith(_T("http://")))
            fileName = textStr;
        if (textStr.StartsWith(_T("file://")))
            fileName = textStr;
        // Remove anything pass the first \n or \r {v1.3.92}
        fileName = fileName.BeforeFirst('\n');
        fileName = fileName.BeforeFirst('\r');
        if (not fileName.IsEmpty())
            textData->SetText( fileName );
    }
    fileData->AddFile( (fileName.Len() > 128) ? wxString(wxEmptyString) : fileName );

        // set composite data object to contain both text and file data
    wxDataObjectComposite *data = new wxDataObjectComposite();
    data->Add( (wxDataObjectSimple*)textData );
    data->Add( (wxDataObjectSimple*)fileData, true ); // set file data as preferred
        // create the drop source containing both data types
    wxDropSource source( *data, this  );

    #ifdef LOGGING
     LOGIT( _T("DropSource Text[%s], File[%s]"),
                textData->GetText().c_str(),
                fileData->GetFilenames().Item(0).c_str() );
    #endif //LOGGING
        // allow both copy and move
    int flags = 0;
    flags |= wxDrag_AllowMove;
    // do the dragNdrop
    wxDragResult result = source.DoDragDrop(flags);

    // report the results
    #if LOGGING
        wxString pc;
        switch ( result )
        {
            case wxDragError:   pc = _T("Error!");    break;
            case wxDragNone:    pc = _T("Nothing");   break;
            case wxDragCopy:    pc = _T("Copied");    break;
            case wxDragMove:    pc = _T("Moved");     break;
            case wxDragLink:    pc = _T("Linked");    break;
            case wxDragCancel:  pc = _T("Cancelled"); break;
            default:            pc = _T("Huh?");      break;
        }
        LOGIT( wxT("DoDragDrop returned[%s]"),pc.GetData() );
    #else
        wxUnusedVar(result);
    #endif

    delete textData; //wxTextDataObject
    delete fileData; //wxFileDataObject
    m_TreeText = wxEmptyString;
    #if !wxCHECK_VERSION(2, 8, 12)
    m_itemAtKeyDown = 0;
    m_itemAtKeyUp = 0;
    #endif

}
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::OnMouseMotionEvent(wxMouseEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // memorize position of the mouse ctrl key as copy/delete flag
//    m_bMouseCtrlKeyDown = event.ControlDown();
//    m_bMouseLeftKeyDown = event.LeftIsDown();
//    m_bMouseIsDragging  = event.Dragging();
//
//    // If dragging an item, show drag cursor
//    if (m_bMouseIsDragging and m_bMouseLeftKeyDown
//        and (not m_bDragCursorOn) and m_itemAtKeyDown)
//    {
//        m_oldCursor = GetCursor();
//        SetCursor(*m_pDragCursor);
//        m_bDragCursorOn = true;
//    }
//    else    //restore regular cursor
//    if (m_bDragCursorOn)
//    {
//        SetCursor(m_oldCursor);
//        m_bDragCursorOn = false;
//    }
//
//    #ifdef LOGGING
//     //LOGIT(wxT("MouseCtrlKeyDown[%s]"), m_bMouseCtrlKeyDown?wxT("Down"):wxT("UP") );
//    #endif
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::OnMouseLeftDownEvent(wxMouseEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // memorize position of the mouse
//    m_bMouseLeftKeyDown = true;
//    m_MouseDownX = event.GetX();
//    m_MouseDownY = event.GetY();
//
//    m_itemAtKeyDown = 0;
//    m_itemAtKeyUp = 0;
//    int hitFlags = 0;
//    wxTreeItemId id = HitTest(wxPoint(m_MouseDownX, m_MouseDownY), hitFlags);
//    if (id.IsOk() and (hitFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL )))
//        m_itemAtKeyDown = id;
//
//    #ifdef LOGGING
//     //LOGIT(wxT("MouseCtrlKeyDown[%s]"), m_bMouseCtrlKeyDown?wxT("Down"):wxT("UP") );
//    #endif
//    event.Skip();
//}
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::OnMouseLeftUpEvent(wxMouseEvent& event)
//// ----------------------------------------------------------------------------
//{
//    event.Skip();
//
//    // memorize position of the mouse
//    m_bMouseLeftKeyDown = false;
//    m_MouseUpX = event.GetX();
//    m_MouseUpY = event.GetY();
//
//    m_itemAtKeyUp = 0;
//    int hitFlags = 0;
//    wxTreeItemId id = HitTest(wxPoint(m_MouseUpX, m_MouseUpY), hitFlags);
//    if (id.IsOk() and (hitFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL )))
//        m_itemAtKeyUp = id;
//
//    #ifdef LOGGING
//     //LOGIT(wxT("OnMouseLeftUp id[%d]"), id.IsOk() );
//    #endif
//
//    if (m_bMouseExitedWindow and m_bMouseIsDragging and m_itemAtKeyDown
//        and (not  m_TreeText.IsEmpty()))
//    {
//        FinishExternalDrag();
//    }
//    else
//    if ( (not m_bMouseExitedWindow) and m_bMouseIsDragging
//         and m_itemAtKeyDown and m_itemAtKeyUp
//         and (m_itemAtKeyDown not_eq m_itemAtKeyUp ))
//    {
//        BeginInternalTreeItemDrag();
//        EndInternalTreeItemDrag();
//    }
//    // Do not release the mouse until after the drag has finished
//    // else the drag will not complete.
//    if (HasCapture() and m_bMouseExitedWindow)
//        ReleaseMouse();
//
//    m_bMouseExitedWindow = false;
//    m_bMouseIsDragging = false;
//
//    if (m_bDragCursorOn)
//    {
//        SetCursor(m_oldCursor);
//        m_bDragCursorOn = false;
//    }
//
//}//OnMouseLeftUpEvent
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnEndTreeItemDrag(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{//asm("int3"); /*trap*/

    // memorize position of the mouse
    m_MouseUpX = event.GetPoint().x;
    m_MouseUpY = event.GetPoint().y;

    #if !wxCHECK_VERSION(2, 8, 12)
    m_itemAtKeyUp = 0;
    #endif
    int hitFlags = 0;
    wxTreeItemId id = HitTest(wxPoint(m_MouseUpX, m_MouseUpY), hitFlags);
    if (id.IsOk() and (hitFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL )))
        m_itemAtKeyUp = id;

    #ifdef LOGGING
     //LOGIT(wxT("OnMouseLeftUp id[%d]"), id.IsOk() );
    #endif

    if (m_bMouseExitedWindow
        and (not  m_TreeText.IsEmpty()))
    {
        #if defined(__WXMSW__)
        //-FinishExternalDrag(); Hangs when called fron here
        #endif
    }
    else
    if ( (not m_bMouseExitedWindow)
         and m_itemAtKeyDown and m_itemAtKeyUp
         and (m_itemAtKeyDown not_eq m_itemAtKeyUp ))
    {
        //-BeginInternalTreeItemDrag();
        EndInternalTreeItemDrag();
    }

    m_bMouseExitedWindow = false;
    m_bMouseIsDragging = false;

}//OnMouseLeftUpEvent
//// ----------------------------------------------------------------------------
//void CodeSnippetsTreeCtrl::OnMouseLeftUpEvent(wxMouseEvent& event)
//// ----------------------------------------------------------------------------
//{
//    event.Skip();
//
//    // memorize position of the mouse
//    m_bMouseLeftKeyDown = false;
//    m_MouseUpX = event.GetX();
//    m_MouseUpY = event.GetY();
//
//    m_itemAtKeyUp = 0;
//    int hitFlags = 0;
//    wxTreeItemId id = HitTest(wxPoint(m_MouseUpX, m_MouseUpY), hitFlags);
//    if (id.IsOk() and (hitFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL )))
//        m_itemAtKeyUp = id;
//
//    #ifdef LOGGING
//     //LOGIT(wxT("OnMouseLeftUp id[%d]"), id.IsOk() );
//    #endif
//
//    if (m_bMouseExitedWindow and m_bMouseIsDragging and m_itemAtKeyDown
//        and (not  m_TreeText.IsEmpty()))
//    {
//        FinishExternalDrag();
//    }
//    else
//    if ( (not m_bMouseExitedWindow) and m_bMouseIsDragging
//         and m_itemAtKeyDown and m_itemAtKeyUp
//         and (m_itemAtKeyDown not_eq m_itemAtKeyUp ))
//    {
//        BeginInternalTreeItemDrag();
//        EndInternalTreeItemDrag();
//    }
//    // Do not release the mouse until after the drag has finished
//    // else the drag will not complete.
//    if (HasCapture() and m_bMouseExitedWindow)
//        ReleaseMouse();
//
//    m_bMouseExitedWindow = false;
//    m_bMouseIsDragging = false;
//
//    if (m_bDragCursorOn)
//    {
//        SetCursor(m_oldCursor);
//        m_bDragCursorOn = false;
//    }
//
//}//OnMouseLeftUpEvent
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnMouseWheelEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // Ctrl-MouseWheel rotation changes treeCtrl font

    m_bMouseCtrlKeyDown = event.ControlDown();
    #ifdef LOGGING
     //LOGIT(wxT("treeCtrl:OnMouseWheel[%s]"), m_bMouseCtrlKeyDown?wxT("Down"):wxT("UP") );
    #endif
    if (not m_bMouseCtrlKeyDown) {event.Skip(); return;}

    int nRotation = event.GetWheelRotation();
    wxFont ctrlFont = GetFont();

    if ( nRotation > 0)
        ctrlFont.SetPointSize( ctrlFont.GetPointSize()-1);
    else
        ctrlFont.SetPointSize( ctrlFont.GetPointSize()+1);

    SetFont(ctrlFont);
    return;
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::ConvertSnippetToCategory(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
    // Convert Snippet to Category, then insert the old snippet
    wxTreeItemId badItemId = (void*)0;

    if ( not IsSnippet(itemId)) return badItemId;
        // memorize id of this snippet
    wxTreeItemId oldItemId = itemId;
    if ( not oldItemId.IsOk() ) return badItemId;
        // memorize parent id of this snippet
    wxTreeItemId itemParent = GetItemParent(oldItemId);

        // create Xml document from selected tree item
    TiXmlDocument* pDoc = CopyTreeNodeToXmlDoc( oldItemId );
    if (not pDoc) return badItemId;

    // Create new Category
    wxTreeItemId newCategoryId = AddCategory( itemParent,
                GetItemText(oldItemId), GetSnippetID(oldItemId), false );

    // Insert old Snippet Item under new Category
    TiXmlElement* root = pDoc->RootElement();
    if (root)
    {
            // Get the first xml element
        TiXmlElement* firstChild = root->FirstChildElement("item");
        if (firstChild)
        {   // insert Tree items from xml document
            //-LoadSnippets(firstChild, newCategoryId);
            LoadItemsFromXmlNode( firstChild, newCategoryId);

        }
    }
        // remove the old tree item
    RemoveItem( oldItemId );
    delete pDoc; pDoc = 0;
    return newCategoryId;
}
// ----------------------------------------------------------------------------
TiXmlDocument* CodeSnippetsTreeCtrl::CopyTreeNodeToXmlDoc(wxTreeItemId TreeItemId  )
// ----------------------------------------------------------------------------
{
    // Copy a Tree node to an Xml Document

    wxTreeItemId itemId = TreeItemId;
    if (not itemId.IsOk() )
    {
    	itemId = GetSelection();
    }
    if (not itemId.IsOk()) return NULL;

	TiXmlDocument* pDoc = new TiXmlDocument;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	pDoc->InsertEndChild(header);

	TiXmlElement snippetsElement("snippets");
	CopySnippetsToXmlDoc(&snippetsElement, itemId);

	pDoc->InsertEndChild(snippetsElement);

	return pDoc;

} // end of SaveSnippetsToFile
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::CopySnippetsToXmlDoc(TiXmlNode* Node, const wxTreeItemId& itemID)
// ----------------------------------------------------------------------------
{
    // Recursively copy Tree item and all its children to an Xml document node

	wxTreeItemIdValue cookie;
	//wxTreeItemId item = GetFirstChild(parentID, cookie );
    wxTreeItemId item = itemID;

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		const SnippetItemData* data = (SnippetItemData*)GetItemData(item);

		if (!data)
			return;

		// Begin item element
		TiXmlElement element("item");

		// Write the item's name
		element.SetAttribute("name", GetItemText(item).mb_str());

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
			//#ifdef LOGGING
			// LOGIT( _T("Snippet[%s]"), data->GetSnippet().GetData() );
			//#endif //LOGGING
		}

		// Check if this item has children
		if(ItemHasChildren(item))
		{
			// If it has, check those too
			//-SaveSnippets(&element, item);
			SaveItemsToXmlNode(&element, item);
		}

		// Insert the item we created as parent node's child
		//parentNode->InsertEndChild(element);
		Node->InsertEndChild(element);

		// Check the next child
		item = GetNextChild( itemID, cookie);
	}
}//CopySnippetsToXmlDoc
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::CopyXmlDocToTreeNode(TiXmlDocument* pTiXmlDoc, wxTreeItemId targetItem  )
// ----------------------------------------------------------------------------
{
    TiXmlElement* root = pTiXmlDoc->RootElement();
    if (root)
    {
        // Get the source xml element
        TiXmlElement* firstChild = root->FirstChildElement("item");
        if (firstChild)
        {   // insert into target Tree items from source xml document
            LoadItemsFromXmlNode( firstChild, targetItem);
        }
    }

}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippetAsFileLink()
// ----------------------------------------------------------------------------
{
    // Open snippet text as a file name. Ie, the text should contain a filename.
    // Else just open a temp file with the snippet text as data.

    #if defined(LOGGING)
    LOGIT( _T("EditSnippetAsFileLink[%s]"),wxT("") );
    #endif

    if (not IsSnippet() ) return;

	// If snippet is file, open it
	wxTreeItemId itemId = GetAssociatedItemID();
	SnippetItemData* pSnippetItemData = (SnippetItemData*)GetItemData(GetAssociatedItemID());
	wxString FileName = GetSnippetFileLink( itemId );
    LOGIT( _T("EditSnippetsAsFileLlink()FileName[%s]"),FileName.c_str() );

    if (FileName.Length() > 128)
    {   // if text is > 128 characters, open a temp file with snippet text as data.
        EditSnippetAsText();
        return;
    }


    // no filename, edit text file
    if ( (FileName.IsEmpty())
        || (not ::wxFileExists( FileName)) )
    {   // if, non-existent file, open snippet text as data
        EditSnippetAsText();
        return;
    }

    // we have an actual file link, not just text.
    // use user specified editor, else hard coded pgms.
    wxString pgmName = GetConfig()->SettingsExternalEditor;
    #if defined(LOGGING)
    LOGIT( _T("PgmName[%s]"),pgmName.c_str() );
    #endif
    // Do: if external pgm name is blank, or file link doesn't exists
    // must be text only
        if ( pgmName.IsEmpty() || ( not ::wxFileExists(pgmName)) )
        {
            EditSnippet( pSnippetItemData, FileName);
            return;
        }

    // edit file link with user settings external program.
    // file name must be surrounded with quotes when using wxExecute
    if ( ::wxFileExists(pgmName) )
    {   wxString execString = pgmName + wxT(" \"") + FileName + wxT("\"");
        #ifdef LOGGING
        LOGIT( _T("OpenAsFileLink[%s]"), execString.GetData() );
        #endif //LOGGING
        ::wxExecute( execString);
    }

    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OpenSnippetAsFileLink()
// ----------------------------------------------------------------------------
{
    // Open snippet text as a file name. Ie, the first text line should contain a filename.

    if (not IsSnippet() ) return;

	// If snippet is file, open it
	wxTreeItemId itemId = GetAssociatedItemID();
	wxString FileName = GetSnippetFileLink( itemId );
    LOGIT( _T("OpenSnippetsAsFileLlink()FileName[%s]"),FileName.c_str() );

    if (FileName.Length() > 128)
    {   // if text is > 128 characters, open a temp file with snippet text as data.
        EditSnippetAsText();
        return;
    }

    // user requested "MIME" type to open file
    EditSnippetWithMIME();

    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippetAsText()
// ----------------------------------------------------------------------------
{

    #if defined(LOGGING)
    LOGIT( _T("EditSnippetAsText[%s]"),wxT("") );
    #endif
	SnippetItemData* pSnippetItemData = (SnippetItemData*)GetItemData(GetAssociatedItemID());

    // if no user specified editor, use default editor
    wxString editorName = GetConfig()->SettingsExternalEditor ;
    if ( editorName.IsEmpty() || (not ::wxFileExists(editorName)) )
    {
        EditSnippet( pSnippetItemData );
        return;
    }

    if ( editorName.IsEmpty() || (not ::wxFileExists(editorName)) )
    {
        #if defined(__WXMSW__)
                editorName = wxT("notepad");
        #elif defined(__UNIX__)
                editorName = wxT("vi");
        #endif
        wxString msg(wxT("Using default editor: ")+editorName+wxT("\n"));
        if (GetConfig()->IsApplication() ) msg = msg + wxT("Use Menu->");
        else msg = msg + wxT("Right click Root item. Use ");
        msg = msg + wxT("Settings to set a better editor.\n");
        GenericMessageBox( msg );
    }

    // let user edit the snippet text
    // write text to temp file
    // invoke the editor
    // read text back into snippet

    wxFileName tmpFileName = wxFileName::CreateTempFileName(wxEmptyString);

    wxFile tmpFile( tmpFileName.GetFullPath(), wxFile::write);
    if (not tmpFile.IsOpened() )
    {
        GenericMessageBox(wxT("Open failed for:")+tmpFileName.GetFullPath());
        return ;
    }
    wxString snippetData( GetSnippet() );
    tmpFile.Write( csU2C(snippetData), snippetData.Length());
    tmpFile.Close();
        // Invoke the external editor on the temporary file
        // file name must be surrounded with quotes when using wxExecute
    wxString execString = editorName + wxT(" \"") + tmpFileName.GetFullPath() + wxT("\"");

    #ifdef LOGGING
     LOGIT( _T("Properties wxExecute[%s]"), execString.GetData() );
    #endif //LOGGING

        // Invoke the external editor and wait for its termination
    ::wxExecute( execString, wxEXEC_SYNC);
        // Read the edited data back into the snippet text
    tmpFile.Open(tmpFileName.GetFullPath(), wxFile::read);
    if (not tmpFile.IsOpened() )
    {   GenericMessageBox(wxT("Abort.Error reading temp data file."));
        return;
    }
    unsigned long fileSize = tmpFile.Length();

    #ifdef LOGGING
     LOGIT( _T("New file size[%d]"),fileSize );
    #endif //LOGGING

    // check the data for validity
    char pBuf[fileSize+1];
    size_t nResult = tmpFile.Read( pBuf, fileSize );
    if ( wxInvalidOffset == (int)nResult )
        GenericMessageBox(wxT("Error reading temp file"));
    pBuf[fileSize] = 0;
    tmpFile.Close();

    #ifdef LOGGING
      //LOGIT( _T("pBuf[%s]"), pBuf );
    #endif //LOGGING

        // convert data back to internal format
    snippetData = csC2U( pBuf );

     #ifdef LOGGING
      //LOGIT( _T("snippetData[%s]"), snippetData.GetData() );
     #endif //LOGGING

        // delete the temporary file
    ::wxRemoveFile( tmpFileName.GetFullPath() );

        // update Tree item
    SetSnippet( snippetData );

    return;
}//EditSnippetAsText
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveSnippetAsFileLink()
// ----------------------------------------------------------------------------
{
    wxTreeItemId itemId = GetAssociatedItemID();
    if (not itemId.IsOk()) return;

    if ( not IsSnippet()) return;

    // Dump the snippet text to file, then place the filename in the snippet text area

    // Dump Snippet field into a temporary file
    wxString snippetLabel = GetSnippetLabel();
    wxString snippetData = GetSnippet();
    wxString fileName = GetSnippetFileLink();

    int answer = wxYES;

    // if file already exists preserve the old data
    if ( ::wxFileExists( fileName ) )
    {   // item snippet is already a filename
        answer = GenericMessageBox(
            wxT("Item is already a file link named:\n")+fileName
                + wxT(" \n\nAre you sure you want to rewrite the file?\n"),
            wxT("Warning"),wxYES|wxNO); //, GetMainFrame(), mousePosn.x, mousePosn.y);
        if ( wxYES == answer)
        {   // read data from old file
            wxFile oldFile( fileName, wxFile::read);
            if (not oldFile.IsOpened() )
            {   GenericMessageBox(wxT("Abort.Error reading data file."));
                return;
            }
            unsigned long fileSize = oldFile.Length();
            char* pBuf = new char[fileSize+1];
            oldFile.Read( pBuf, fileSize );
            pBuf[fileSize] = 0;
            snippetData = csC2U(  pBuf );
            oldFile.Close();
            delete [] pBuf;
        }
    }
    if ( wxNO == answer ) return;

    // filter filename, removing all illegal filename characters
    wxString newFileName = snippetLabel+wxT(".txt");
    wxFileName snippetFileName( newFileName) ;
    //-#if defined(BUILDING_PLUGIN)
        // substitute any $(macro) text
        static const wxString delim(_T("$%["));
        if( newFileName.find_first_of(delim) != wxString::npos )
            Manager::Get()->GetMacrosManager()->ReplaceMacros(newFileName);
        //-LOGIT( _T("$macros substitute[%s]"),newFileName.c_str() );
    //-#endif

    //newFileName = snippetFileName.GetFullName();
    wxString forbidden = snippetFileName.GetForbiddenChars();
    for (size_t i=0; i < forbidden.Length(); ++i)
        newFileName.Replace( wxString(forbidden[i]), wxT(""),true);
        //#ifdef LOGGING
        // LOGIT( _T("forbidden[%s],filename[%s]"), forbidden.GetData(),newFileName.GetData());
        //#endif //LOGGING

    // Ask user for filename
    wxFileDialog dlg(this,                              //parent  window
                 _("Save as text file"),                //message
                 GetConfig()->SettingsSnippetsFolder,   //default directory
                 newFileName,                           //default file
                 wxT("*.*"),                            //wildcards
                 wxFD_SAVE | wxFD_OVERWRITE_PROMPT);    //style
    // move dialog into the parents frame space
    wxPoint mousePosn = ::wxGetMousePosition();
    (&dlg)->Move(mousePosn.x, mousePosn.y);
    if (dlg.ShowModal() != wxID_OK) return;

    newFileName = dlg.GetPath();

    #ifdef LOGGING
     LOGIT( _T("New filename[%s]"), newFileName.GetData() );
    #endif //LOGGING;

    if ( newFileName.IsEmpty() ) return;
    // Verify filename, or create
    wxFile newFile( newFileName, wxFile::write);
    if (not newFile.IsOpened() )
    {
        GenericMessageBox(wxT("Open failed for:")+newFileName);
        return ;
    }
    newFile.Write( csU2C(snippetData), snippetData.Length());
    newFile.Close();
    // update Tree item
    SetSnippet( newFileName );

    // verify the item tree image
    if ( IsFileSnippet() )
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_FILE);
    else
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_TEXT);

    return;
}//OnMnuSaveAsFile
// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::EditSnippetProperties(wxTreeItemId& itemId)
// ----------------------------------------------------------------------------
{
    if ( not IsSnippet(itemId) ) return false;

    int result = 0;
    wxSemaphore waitSem;
    SnippetProperty* pdlg = new SnippetProperty(GetSnippetsTreeCtrl(), itemId, &waitSem);

	// Add Properties edit window to DragScroll managed windows
    DragScrollEvent dsevt(wxEVT_DRAGSCROLL_EVENT , idDragScrollAddWindow);
    dsevt.SetEventObject((wxObject*)pdlg->m_SnippetEditCtrl);
    GetConfig()->GetDragScrollEvtHandler()->AddPendingEvent( dsevt );

    result = ExecuteDialog(pdlg, waitSem);
    // Save any changed data
	if ( result == wxID_OK )
	{
        // verify the item tree image
        SetSnippetImage(itemId);
	}
    if (result == wxID_OK) {
        SetFileChanged(true);
    }

 	// Remove Properties edit window from DragScroll managed windows
    dsevt.SetId(idDragScrollRemoveWindow);
    dsevt.SetEventObject((wxObject*)pdlg->m_SnippetEditCtrl);
    GetConfig()->GetDragScrollEvtHandler()->AddPendingEvent( dsevt );

    pdlg->Destroy();
    return (result = (result==wxID_OK));
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippet(SnippetItemData* pSnippetItemData, wxString fileName)
// ----------------------------------------------------------------------------
{
    // just focus any already open snippet items

    Utils utils;
    int knt = aEdFrameRetcodes.GetCount();
    for (int i = 0; i<knt ; ++i )
    {   EditSnippetFrame* pesf = (EditSnippetFrame*)m_aEdFramePtrs.Item(i);
        if (not pesf) continue;
        if (not utils.WinExists(pesf))
            continue;

        if ( pesf->GetSnippetId() == GetAssociatedItemID() )
    	{
    	    // if return code has been set, this frame is set to terminate
    	    if ( (int)aEdFrameRetcodes.GetCount() < i)
                continue;
            if ( aEdFrameRetcodes.Item(i) not_eq 0)
                continue;
    	    m_aEdFramePtrs.Item(i)->Iconize(false);
    	    m_aEdFramePtrs.Item(i)->SetFocus();
            return;
    	}
    }//for

    // Create editor for snippet item
    if (SnippetItemData* itemData = (SnippetItemData*)( GetItemData(GetAssociatedItemID() )))
    {
        wxString snippetText = itemData->GetSnippet();
        aEdFrameRetcodes.Add(0);          // new slot for return code
        int* pRetcode = &aEdFrameRetcodes.Last();

        EditSnippetFrame* pEdFrame = new EditSnippetFrame( GetAssociatedItemID(), pRetcode );

        // cascade multiple editors
        int knt = m_aEdFramePtrs.GetCount();
        if (pEdFrame && (knt > 0) ){
            int x,y;
            pEdFrame->GetPosition(&x, &y );
             if (0 == x){
                pEdFrame->ClientToScreen(&x, &y );
             }
            knt = knt<<5;
            pEdFrame->Move(x+knt,y+knt);
        }

        if ( pEdFrame->Show() )
        {
            m_aEdFramePtrs.Add((wxScrollingDialog*)pEdFrame);
        }
        else
            aEdFrameRetcodes.RemoveAt(aEdFrameRetcodes.GetCount());

	}//if
}//EditSnippet

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippetWithMIME()
// ----------------------------------------------------------------------------
{
    wxTreeItemId itemId = GetAssociatedItemID();
    if (not itemId.IsOk()) return;
    if ( not IsSnippet()) return;

    wxString snippetLabel = GetSnippetLabel();
    wxString snippetData = GetSnippet();
    wxString fileName = GetSnippetFileLink();
    LOGIT( _T("EditSnippetWithMime[%s]"), fileName.c_str() );
    if ( fileName.IsEmpty() ) return;

    wxFileName file(fileName);
    wxString fileExt = file.GetExt();

    // MIME search fails on a url. Do it brute force
    if (   ( fileName.StartsWith(wxT("http://")) )
        || ( fileName.StartsWith(wxT("file://")) )
        || ( fileName.StartsWith(wxT("ftp://")) )
        || ( fileExt == wxT("html") )
        || ( fileExt == wxT("htm") )
       )
    {   wxLaunchDefaultBrowser( fileName);
        return;
    }

    if ( not ::wxFileExists(fileName) ) return;

    wxString fileNameExt;
    #if wxCHECK_VERSION(2, 9, 0)
    wxFileName::SplitPath( fileName, /*volume*/0, /*path*/0, /*name*/0, &fileNameExt);
    #else
    ::wxSplitPath( fileName, /*path*/0, /*name*/0, &fileNameExt);
    #endif
    if ( fileNameExt.IsEmpty() ) return;

    wxString s_defaultExt = _T("xyz");
    wxString msg;

    if ( !!fileNameExt )
    {
        s_defaultExt = fileNameExt;

        // init MIME database if not done yet
        if ( !m_mimeDatabase )
        {
            ////m_mimeDatabase = new wxMimeTypesManager;
            // use global pointer instead of above instantiation
            m_mimeDatabase = wxTheMimeTypesManager;

            ////static const wxFileTypeInfo fallbacks[] =
            ////{
            ////    wxFileTypeInfo(_T("application/xyz"),
            ////                   _T("XyZ %s"),
            ////                   _T("XyZ -p %s"),
            ////                   _T("The one and only XYZ format file"),
            ////                   _T("xyz"), _T("123"), NULL),
            ////    wxFileTypeInfo(_T("text/html"),
            ////                   _T("lynx %s"),
            ////                   _T("lynx -dump %s | lpr"),
            ////                   _T("HTML document (from fallback)"),
            ////                   _T("htm"), _T("html"), NULL),
            ////
            ////    // must terminate the table with this!
            ////    wxFileTypeInfo()
            ////};
            ////
            ////m_mimeDatabase->AddFallbacks(fallbacks);
        }//if

        wxFileType *filetype = m_mimeDatabase->GetFileTypeFromExtension(fileNameExt);
        if ( !filetype )
        {
            msg << _T("Unknown extension '") << fileNameExt << _T("'\n");
        }
        else
        {
            wxString type, desc, open;
            filetype->GetMimeType(&type);
            filetype->GetDescription(&desc);

            //wxString filename = _T("filename");
            wxString filename = fileName;
            //filename << _T(".") << fileNameExt;
            wxFileType::MessageParameters params(filename, type);
            filetype->GetOpenCommand(&open, params);

           #if LOGGING
            msg << _T("MIME information about extension '") << fileNameExt << _T('\n')
                     << _T("\tMIME type: ") << ( !type ? wxT("unknown")
                                                   : type.c_str() ) << _T('\n')
                     << _T("\tDescription: ") << ( !desc ? wxEmptyString : desc.c_str() )
                        << _T('\n')
                     << _T("\tCommand to open: ") << ( !open ? wxT("no") : open.c_str() )
                        << _T('\n');
           #endif

            delete filetype;
            #if wxCHECK_VERSION(2, 9, 0)
            if ( !open.IsEmpty() )
            #else
            if ( open )
            #endif
                ::wxExecute( open, wxEXEC_ASYNC);
        }
    }
    #ifdef LOGGING
        LOGIT( _T("EditSnippetWithMIME()[%s]"),msg.c_str() );
    #endif //LOGGING

    // Do not use these when using global wxTheMimeTypesManager pointer
    ////if ( m_mimeDatabase ) delete m_mimeDatabase;
    ////m_mimeDatabase = 0;

    return;
}
// ----------------------------------------------------------------------------
int CodeSnippetsTreeCtrl::ExecuteDialog(wxScrollingDialog* pdlg, wxSemaphore& waitSem)
// ----------------------------------------------------------------------------
{
    if (m_pPropertiesDialog) return 0;

    m_pPropertiesDialog = pdlg;
    int retcode = 0;

    wxWindow* pw = this;
    if (pw && pw->GetParent()) //
    {   pw = pw->GetParent();
    }
    if (pw && pw->GetParent())  //This is the SnippetWindow parent
    {   pw = pw->GetParent();
    }

    // Grab main apps close function so dlg isn't orphaned|crashed on close
    GetConfig()->GetMainFrame()->Connect( wxEVT_CLOSE_WINDOW,
        (wxObjectEventFunction)(wxEventFunction)
        (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown,NULL,this);

    // Grab parents close function so dlg isn't orphaned|crashed on close)
    pw->Connect( wxEVT_CLOSE_WINDOW,
        (wxObjectEventFunction)(wxEventFunction)
        (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown,NULL,this);

        // The following works fine on windows, but does not disable the menu item on linux.
        // *and*, I no longer care.
        if ( GetConfig()->IsPlugin() )
            GetConfig()->GetMenuBar()->Enable(idViewSnippets, false);

        if ( pdlg->Show() )
        {
            // Just check to see if the semaphore has been posted.
            // Don't do a real wait, else the edit dialog will freeze
            while( wxSEMA_BUSY == waitSem.TryWait())
            {   waitSem.WaitTimeout(20);
                wxYield();
            }
            retcode = pdlg->GetReturnCode();
        }
        // Release main apps closeWindow function
        GetConfig()->GetMainFrame()->Disconnect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown);

        // Release parents closeWindow function
        pw->Disconnect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown);

        if ( GetConfig()->IsPlugin() )
            GetConfig()->GetMenuBar()->Enable(idViewSnippets, true);

        m_pPropertiesDialog = 0;

        return retcode;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveEditorsXmlData(EditSnippetFrame* pEdFrame)
// ----------------------------------------------------------------------------
{
    SnippetItemData* pSnippetItemData = (SnippetItemData*)(GetItemData(pEdFrame->GetSnippetId()));
    pSnippetItemData->SetSnippet(pEdFrame->GetText());
    SetItemText(pEdFrame->GetSnippetId(), pEdFrame->GetName());
    SetFileChanged(true);
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveDataAndCloseEditorFrame(EditSnippetFrame* pEdFrame)
// ----------------------------------------------------------------------------
{
    // This routine is invoked from EditSnippetFrame::OnCloseFrameOrWindow()
    // check to see if an editor has been posted & finish.

    // if parm is valid, set it's return code = 0
    if (pEdFrame )
    {
        int index = m_aEdFramePtrs.Index((wxScrollingDialog*)pEdFrame);
        if (index not_eq wxNOT_FOUND)
            aEdFrameRetcodes[index] = wxID_OK;
    }

    for (size_t i = 0; i < this->aEdFrameRetcodes.GetCount(); ++i )
    {
        // if we have a return code, this editor is done
        if ( aEdFrameRetcodes.Item(i) == 0)
            continue;

        // else an edit frame is done, save any changed data
        int retcode = aEdFrameRetcodes.Item(i);
        EditSnippetFrame* pEdFrame = (EditSnippetFrame*)m_aEdFramePtrs.Item(i);
#if !wxCHECK_VERSION(2,9,0)
        pEdFrame->MakeModal(false);
#endif
		if (retcode == wxID_OK)
		{
            // If XML text snippet, just save back to XML file
            if (pEdFrame->GetFileName().IsEmpty())
            {
                SaveEditorsXmlData(pEdFrame);
            }
            else //This was an external file
            {
                ;// Modified external files already saved by editor
            }
            // if text item type changed to link, set corrected icon
            if ( pEdFrame->GetSnippetId().IsOk() )
            {
                SetSnippetImage(pEdFrame->GetSnippetId());
                #if defined(LOGGING)
                LOGIT( _T("CodeSnippetsTreeCtrl saved XML"));
                #endif
            }
			//-SetFileChanged(true);
			// Save the XML file
			SaveItemsToFile(GetConfig()->SettingsSnippetsXmlPath);
		}//if wxID_OK

		if (pEdFrame && (not m_bShutDown) )
        {
            // If a pgm is started after CodeBlocks, it'll get the focus
            // when we destroy this editor frame *and* the frame was initiated by
            // a context menu item. So, here, we raise and focus CodeBlocks
            // if this is the last editor frame.
            if ( 1 == this->aEdFrameRetcodes.GetCount() )
            {   wxWindow* pWin = (wxWindow*)(GetConfig()->GetMainFrame());
                pWin->Raise();
                pWin->SetFocus();
            }
            pEdFrame->Destroy();
        }

        // retcode set by return frame
        //-retcode = pEdFrame->GetReturnCode();
        aEdFrameRetcodes.Item(i) = 0;
        m_aEdFramePtrs.Item(i) = 0;
    }//for

    // when all editors terminate, free array storage
    size_t editorsOpen = 0 ;
    size_t knt = m_aEdFramePtrs.GetCount();
    for (size_t i = 0; i < knt; ++i )
    	editorsOpen += (size_t)m_aEdFramePtrs.Item(i)?1:0;
    if ( knt && (not editorsOpen) )
    {   aEdFrameRetcodes.Clear();
        m_aEdFramePtrs.Clear();
    }

}//SaveAndCloseFrame
// ----------------------------------------------------------------------------
//-void CodeSnippetsTreeCtrl::OnIdle(wxIdleEvent& event)
// This routine is invoked from codesnippets.cpp and codesnippetsapp.cpp
void CodeSnippetsTreeCtrl::OnIdle()
// ----------------------------------------------------------------------------
{
    // check to enable menu items and update StatusBar

    if ( GetConfig()->IsPlugin() )
        GetConfig()->GetMenuBar()->Enable(idViewSnippets, true);

    // if search text is empty, show filename as root item
    // Edit the root node's title so that the user sees file name
    if ( GetConfig()->GetSnippetsSearchCtrl()
        && GetConfig()->GetSnippetsSearchCtrl()->GetValue().IsEmpty() )
    {
        wxString nameOnly;
        wxFileName::SplitPath( GetConfig()->SettingsSnippetsXmlPath, 0, &nameOnly, 0);
        // avoid excessive refresh
        wxString currentValue = GetItemText(GetRootItem());
        if (currentValue != nameOnly)
            SetItemText( GetRootItem(), wxString::Format(_("%s"), nameOnly.GetData()));
    }

    ////event.Skip(); this routine is called from another OnIdle which does the
    // event.Skip() itself.
    return;
}//OnIdle

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnShutdown(wxCloseEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip(); return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_Select(CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    // CodeSnippetsEvent (usually issued from external ThreadSearch frame)
    // to focus/select a tree item via a snippet id

    event.Skip();

    wxString xmlString = event.GetSnippetString();
    #if defined(LOGGING)
    int snippetID = event.GetSnippetID();
    LOGIT( _T("CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_Select[%d]String[%s]"), snippetID, xmlString.c_str());
    #endif

    xmlString.Trim(false);  // remove prefix whitespace
    // fetch type="category" or type="snippet"
    int type = 0;
    long id  = 0;
    wxString idString(wxEmptyString);
    if ( xmlString.Contains(_T("type=\"category\"")))
        type = SnippetItemData::TYPE_CATEGORY;
    if ( xmlString.Contains(_T("type=\"snippet\"")))
        type = SnippetItemData::TYPE_SNIPPET;
    if ( type )
    {
        int s = xmlString.Find(_T(" ID=\""));
        if (wxNOT_FOUND == s) return;
        // parse out numeric id
        idString = xmlString.Mid( s+5 );
        idString = idString.Mid( 0 ,idString.Find('\"') );
        idString.ToLong(&id);
    }
    if ( id )
    {
        wxTreeItemId rootID = GetRootItem();
        wxTreeItemId treeItemID = FindTreeItemBySnippetId( id, rootID );
        if (treeItemID)
        {
            EnsureVisible( treeItemID );
            SelectItem( treeItemID );
            // Let edit event(doubleClick) raise and focus CodeSnippets window
            //-wxWindow* pWin = (wxWindow*)(GetConfig()->GetMainFrame());
            //-pWin->Raise();
            //-pWin->SetFocus();
        }
    }//if id

}//OnCodeSnippetsEvent_Select
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_Edit(CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    // CodeSnippetsEvent (usually issued from external ThreadSearch frame)
    // to edit a tree item via a snippet id

    event.Skip();

    wxString xmlString = event.GetSnippetString();
    #if defined(LOGGING)
    LOGIT( _T("CodeSnippetsTreeCtrl::OnCodeSnippesEvent_Edit[%d][%s]"), event.GetSnippetID(), xmlString.c_str());
    #endif

    xmlString.Trim(false);  // remove prefix whitespace
    // fetch type="category" or type="snippet"
    int type = 0;
    long id  = 0;
    wxString idString(wxEmptyString);
    if ( xmlString.Contains(_T("type=\"category\"")))
        type = SnippetItemData::TYPE_CATEGORY;
    if ( xmlString.Contains(_T("type=\"snippet\"")))
        type = SnippetItemData::TYPE_SNIPPET;
    if ( type )
    {
        int s = xmlString.Find(_T(" ID=\""));
        if (wxNOT_FOUND == s) return;
        // parse out numeric id
        idString = xmlString.Mid( s+5 );
        idString = idString.Mid( 0 ,idString.Find('\"') );
        idString.ToLong(&id);
    }
    if ( id )
    {
        wxTreeItemId rootID = GetRootItem();
        wxTreeItemId treeItemID = FindTreeItemBySnippetId( id, rootID );
        if (treeItemID)
        {
            EnsureVisible( treeItemID );
            SelectItem( treeItemID );
            if ( type == SnippetItemData::TYPE_CATEGORY )
            {
                wxWindow* pWin = (wxWindow*)(GetConfig()->GetMainFrame());
                pWin->Raise();
                pWin->SetFocus();
            }
            if ( type == SnippetItemData::TYPE_SNIPPET )
            {
                SetAssociatedItemID( treeItemID );
                wxCommandEvent editEvt( wxEVT_COMMAND_MENU_SELECTED , idMnuEditSnippet);
                #if wxCHECK_VERSION(2, 9, 0)
                GetConfig()->GetSnippetsWindow()->GetEventHandler()->AddPendingEvent(editEvt);
                #else
                GetConfig()->GetSnippetsWindow()->AddPendingEvent( editEvt);
                #endif
            }
        }
    }//if id

}//OnCodeSnippetsEvent_Edit
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnCodeSnippetsEvent_GetFileLinks(CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    // CodeSnippetsEvent usually issued from external frame
    // to fill file map with file link names and snippet ids

    event.Skip();

    #if defined(LOGGING)
    LOGIT( _T("CodeSnippetsTreeCtrl::OnCodeSnippesEvent_GetFileLinks") );
    #endif

    FileLinksMapArray& fileIDMap = GetConfig()->GetFileLinksMapArray();
    fileIDMap.clear();

    FillFileLinksMapArray( GetRootItem(), fileIDMap );
    return;

}//OnCodeSnippetsEvent_GetFileLinks
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::ResetSnippetsIDs(const wxTreeItemId& startNode)
// ----------------------------------------------------------------------------
{
    // Re-number Snippet Id's in Tree. Used for save and data inserts

    static wxTreeItemId dummyItem = (void*)(0);
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(startNode, cookie );

	// Loop through all items and reset their ID number
	while(item.IsOk())
	{
		if ( SnippetItemData* itemData = (SnippetItemData*)(GetItemData(item)))
		{
			bool ignoreThisType = false;

			switch (itemData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisType = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
				break;

				case SnippetItemData::TYPE_CATEGORY:
				break;
			}

			if (!ignoreThisType)
			{
				//-wxString label = GetItemText(item);
                itemData->SetID( itemData->GetNewID() );
			}

			if(ItemHasChildren(item))
			{
				wxTreeItemId search = ResetSnippetsIDs( item );
				if(search.IsOk())
					return search;
			}
			item = GetNextChild(startNode, cookie);
		}
	}

   // Return dummy item if search string was not found
   return dummyItem;
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FillFileLinksMapArray(const wxTreeItemId& startNode, FileLinksMapArray& fileLinksMapArray)
// ----------------------------------------------------------------------------
{
    // Place Snippet item file links into array along with their snippet ID's

    static wxTreeItemId dummyItem = (void*)(0);
	wxTreeItemIdValue cookie;

	wxTreeItemId treeItem = GetFirstChild(startNode, cookie );

	// Loop through all items record their file links
	while(treeItem.IsOk())
	{
		if ( SnippetItemData* snippetData = (SnippetItemData*)(GetItemData(treeItem)))
		{
			bool ignoreThisType = false;

			switch (snippetData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisType = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
				break;

				case SnippetItemData::TYPE_CATEGORY:
                    ignoreThisType = true;
				break;
			}

			if (!ignoreThisType)
			{
			    wxString fileName = wxEmptyString;
				if ( wxEmptyString not_eq (fileName = snippetData->GetSnippetFileLink() ))
                {
                    long itemId = snippetData->GetID();
                    fileLinksMapArray[fileName] = itemId;
                }
			}

			if(ItemHasChildren(treeItem))
			{
				wxTreeItemId search = FillFileLinksMapArray( treeItem, fileLinksMapArray );
				if(search.IsOk())
					return search;
			}
			treeItem = GetNextChild(startNode, cookie);
		}
	}

   // Return dummy item if search string was not found
   return dummyItem;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::CreateDirLevels(const wxString& pathNameIn)
// ----------------------------------------------------------------------------
{
    // FileImport Traverser will create any missing directories
    FileImportTraverser fit(_T("dummy"), pathNameIn);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SendMouseLeftUp(const wxWindow* pWin, const int mouseX, const int mouseY)
// ----------------------------------------------------------------------------
{
    // ---MSW WORKAROUNG --------------------------------------------------
    // Since we dragged outside the tree control with a mouse_left_down,
    // Wx will *not* send us a mouse_key_up event until the user explcitly
    // re-clicks inside the tree control. The tree exibits very bad behavior.

    // Send an mouse_key_up to the tree control so it releases the
    // mouse and behaves correctly.
  #if defined(__WXMSW__)
    if ( pWin )
    {
        //send Mouse LeftKeyUp to Tree Control window
        #ifdef LOGGING
         LOGIT( _T("Sending Mouse LeftKeyUp"));
        #endif //LOGGING
        // Remember current mouse position
        wxPoint CurrentMousePosn = ::wxGetMousePosition();
        // Get absolute location of mouse x and y
        wxPoint fullScreen = pWin->ClientToScreen(wxPoint(mouseX,mouseY));
        // move mouse into the window
        MSW_MouseMove( fullScreen.x, fullScreen.y );
        // send mouse LeftKeyUp
        INPUT Input         = {0};
        Input.type          = INPUT_MOUSE;
        Input.mi.dwFlags    = MOUSEEVENTF_LEFTUP;
        ::SendInput(1,&Input,sizeof(INPUT));
        // put mouse back in drag-end position
        MSW_MouseMove( CurrentMousePosn.x, CurrentMousePosn.y );
    }
  #endif //(__WXMSW__)
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::MSW_MouseMove(int x, int y )
// ----------------------------------------------------------------------------
{
   #if defined(__WXMSW__)
    // Move the MSW mouse to absolute screen coords x,y
      double fScreenWidth   = ::GetSystemMetrics( SM_CXSCREEN )-1;
      double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1;
      double fx = x*(65535.0f/fScreenWidth);
      double fy = y*(65535.0f/fScreenHeight);
      INPUT  Input={0};
      Input.type      = INPUT_MOUSE;
      Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
      Input.mi.dx = (LONG)fx;
      Input.mi.dy = (LONG)fy;
      ::SendInput(1,&Input,sizeof(INPUT));
   #endif
}
