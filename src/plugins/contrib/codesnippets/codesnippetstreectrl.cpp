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
// RCS-ID: $Id: codesnippetstreectrl.cpp 105 2007-11-16 19:50:44Z Pecan $

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

#if defined(BUILDING_PLUGIN)
    #include "sdk.h"
    #ifndef CB_PRECOMP
        #include "manager.h"
        #include "logmanager.h"
        #include "globals.h"
    #endif
#else
#endif

#include <tinyxml/tinyxml.h>
#include "snippetitemdata.h"
#include "codesnippetstreectrl.h"
#include "snippetsconfig.h"
#include "messagebox.h"
#include "menuidentifiers.h"
#include "editsnippetframe.h"
#if defined(__WXGTK__)
    #include "wx/gtk/win_gtk.h"
    #include <gdk/gdkx.h>
#endif

IMPLEMENT_DYNAMIC_CLASS(CodeSnippetsTreeCtrl, wxTreeCtrl)

BEGIN_EVENT_TABLE(CodeSnippetsTreeCtrl, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(idSnippetsTreeCtrl, CodeSnippetsTreeCtrl::OnBeginTreeItemDrag)
	EVT_TREE_END_DRAG(idSnippetsTreeCtrl,   CodeSnippetsTreeCtrl::OnEndTreeItemDrag)
	EVT_LEAVE_WINDOW(                       CodeSnippetsTreeCtrl::OnLeaveWindow)
	EVT_ENTER_WINDOW(                       CodeSnippetsTreeCtrl::OnEnterWindow)
	EVT_MOTION(                             CodeSnippetsTreeCtrl::OnMouseEvent)
	EVT_TREE_SEL_CHANGED(idSnippetsTreeCtrl,CodeSnippetsTreeCtrl::OnItemSelected)
	EVT_TREE_ITEM_RIGHT_CLICK(idSnippetsTreeCtrl, CodeSnippetsTreeCtrl::OnItemRightSelected)
	//-EVT_IDLE(                               CodeSnippetsTreeCtrl::OnIdle)
	//- EVT_IDLE replaced by call from plugin|appframe OnIdle routine
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsTreeCtrl::CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id,
                                const wxPoint& pos, const wxSize& size, long style)
// ----------------------------------------------------------------------------
	//-: wxTreeCtrl(parent, id, pos, size, style)
	: wxTreeCtrl(parent, id, pos, size, style, wxDefaultValidator, wxT("csTreeCtrl"))
{
    m_fileChanged = false;
    m_bMouseLeftWindow = false;
    m_pPropertiesDialog = 0;
    m_bShutDown = false;
    m_mimeDatabase = 0;

    m_pSnippetsTreeCtrl = this;
    GetConfig()->SetSnippetsTreeCtrl(this);

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
    #if defined(BUILDING_PLUGIN)
        Manager::Get()->GetMacrosManager()->ReplaceMacros(fileName);
        //-LOGIT( _T("$macros name[%s]"),fileName.c_str() );
    #endif
    if ( not ::wxFileExists( fileName) ) return false;
    return true;
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

    //CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();
    wxTreeItemId itemId = event.GetItem();

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
    // The selection is still on he previous item.
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
wxTreeItemId CodeSnippetsTreeCtrl::FindItemByLabel(const wxString& searchTerms, const wxTreeItemId& node, int requestType)
// ----------------------------------------------------------------------------
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetSnippetsTreeCtrl()->GetFirstChild(node, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(item)))
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
				wxString label = GetSnippetsTreeCtrl()->GetItemText(item);

				if(0 == label.Cmp(searchTerms))
				{
					return item;
				}
			}

			if(GetSnippetsTreeCtrl()->ItemHasChildren(item))
			{
				wxTreeItemId search = FindItemByLabel(searchTerms, item, requestType);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetSnippetsTreeCtrl()->GetNextChild(node, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem = (void*)(0);
   return dummyItem;
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FindItemById(const wxTreeItemId& itemToFind, const wxTreeItemId& startNode, int itemToFindType)
// ----------------------------------------------------------------------------
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetSnippetsTreeCtrl()->GetFirstChild(startNode, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(item)))
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
				wxString label = GetSnippetsTreeCtrl()->GetItemText(item);

				if( itemToFind == item)
				{
					return item;
				}
			}

			if(GetSnippetsTreeCtrl()->ItemHasChildren(item))
			{
				wxTreeItemId search = FindItemById(itemToFind, item, itemToFindType);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetSnippetsTreeCtrl()->GetNextChild(startNode, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem = (void*)(0);
   return dummyItem;
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveItemsToXmlNode(TiXmlNode* node, const wxTreeItemId& parentID)
// ----------------------------------------------------------------------------
{
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
	for (; node; node = node->NextSiblingElement())
	{
		// Check if the node has attributes
		const wxString itemName(csC2U(node->Attribute("name")));
		const wxString itemType(csC2U(node->Attribute("type")));

		// Check the item type
		if (itemType == _T("category"))
		{
			// Add new category
			wxTreeItemId newCategoryId = AddCategory(parentID, itemName, false);

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
						AddCodeSnippet(parentID, itemName, csC2U(snippetElementText->Value()), false);
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
				messageBox(_T("CodeSnippets: Error loading XML file; element \"snippet\" cannot be found."));
			}
		}
		else
		{
		    messageBox(_T("CodeSnippets: Error loading XML file; attribute \"type\" is \"") + itemType + _T("\" which is invalid item type."));
			return;
		}
	} // end for
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveItemsToFile(const wxString& fileName)
// ----------------------------------------------------------------------------
{
	TiXmlDocument doc;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	doc.InsertEndChild(header);

	TiXmlComment comment;
	comment.SetValue("Trust me. I know what I'm doing.");
	doc.InsertEndChild(comment);

	TiXmlElement snippetsElement("snippets");
	SaveItemsToXmlNode(&snippetsElement, GetRootItem());

	doc.InsertEndChild(snippetsElement);

	doc.SaveFile(fileName.mb_str());
	SetFileChanged(false);
	SaveFileModificationTime();

    #ifdef LOGGING
     LOGIT( _T("File saved:[%s]"),fileName.c_str() );
    #endif //LOGGING
}

// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::LoadItemsFromFile(const wxString& fileName, bool bAppendItems)
// ----------------------------------------------------------------------------
{
	if (!bAppendItems)
        DeleteChildren( GetRootItem() );

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
           #if defined(BUILDING_PLUGIN)
			Manager::Get()->GetLogManager()->DebugLog(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + csC2U(doc.ErrorDesc()));
			Manager::Get()->GetLogManager()->DebugLog(_T("CodeSnippets: Backup of the failed file has been created."));
		   #else
            //-wxMessageBox(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + csC2U(doc.ErrorDesc()));
            messageBox(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + csC2U(doc.ErrorDesc()));
   			//-wxMessageBox(_T("CodeSnippets: Backup of the failed file has been created."));
   			messageBox(_T("CodeSnippets: Backup of the failed file has been created."));
		   #endif
		}
	}
	// Show the first level of items
	if (  GetRootItem() && GetRootItem().IsOk())
        Expand( GetRootItem() );

    // show filename window banner
    wxString nameOnly;
    wxFileName::SplitPath( fileName, 0, &nameOnly, 0);
    // Edit the root node's title so that the user sees file name
    GetSnippetsTreeCtrl()->SetItemText(GetSnippetsTreeCtrl()->GetRootItem(), wxString::Format(_("%s"), nameOnly.GetData()));

    SetFileChanged(false);
    SaveFileModificationTime();
    return retcode;

}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::AddCodeSnippet(const wxTreeItemId& parent,
                            wxString title, wxString codeSnippet, bool editNow)
// ----------------------------------------------------------------------------
{
	wxTreeItemId newItemID = InsertItem(parent, GetLastChild(parent), title, 2, -1,
                new SnippetItemData(SnippetItemData::TYPE_SNIPPET, codeSnippet));

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
wxTreeItemId CodeSnippetsTreeCtrl::AddCategory(const wxTreeItemId& parent, wxString title, bool editNow)
// ----------------------------------------------------------------------------
{
	wxTreeItemId newCategoryID = InsertItem(parent, GetLastChild(parent), title, 1, -1, new SnippetItemData(SnippetItemData::TYPE_CATEGORY));

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
    if ( itemText.IsSameAs(wxT("New category")) || itemText.IsSameAs(wxT("New snippet")) )
        shiftKeyIsDown = true;
    bool trashItem = false;

    // if shift key is up, copy item to .trash category
    if (not shiftKeyIsDown)
    {
        // put deleted items in .trash category
        wxTreeItemId trashId = FindItemByLabel(wxT(".trash"), GetRootItem(), CodeSnippetsConfig::SCOPE_CATEGORIES);
        if ( trashId==(void*)0 )
            trashId = AddCategory(GetRootItem(), wxT(".trash"), false);

        // if item is NOT already in the trash, copy item to .trash category
        if (not ( FindItemById( itemId, trashId, pItemData->GetType()) ))
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
    if (trashItem || shiftKeyIsDown)
    {
        // if FileLink, memorize the filename
        wxString filename = wxEmptyString;
        if ( IsFileSnippet(itemId) )
           filename = GetSnippetFileLink(itemId);

        // if this was a FileLink, ask if user wants to delete file
        if ( not filename.IsEmpty() ) {
            int answer = messageBox( wxT("Delete physical file?\n\n")+filename,
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
void CodeSnippetsTreeCtrl::SaveFileModificationTime(wxDateTime savedTime)
// ----------------------------------------------------------------------------
{
    if (savedTime != time_t(0))
    {   m_LastXmlModifiedTime = savedTime;
        return;
    }
    if (::wxFileExists(GetConfig()->SettingsSnippetsXmlFullPath) )
    {   wxFileName fname( GetConfig()->SettingsSnippetsXmlFullPath );
        m_LastXmlModifiedTime = fname.GetModificationTime();
    }
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnBeginTreeItemDrag(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // TREE_BEGIN_DRAG
    // -----------------------
    CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();

    //#ifdef LOGGING
    //	 LOGIT( wxT("ScrapList::OnTreeCtrlEvent %p"), pTree );
    //#endif //LOGGING

    #ifdef LOGGING
     LOGIT( _T("TREE_CTRL_BEGIN_DRAG %p"), pTree );
    #endif //LOGGING
    // On MSW the current selection may not be the same as the current itemId
    // If the user just clicks and drags, the two are different
    m_pEvtTreeCtrlBeginDrag = pTree;
    m_TreeItemId        = event.GetItem();
        // At this point we could solve the above problem with
        // pTree->SelectItem(m_TreeItemId) ; But for now, we'll just
        // record the actual current itemId.
    m_MnuAssociatedItemID = m_TreeItemId;
    m_TreeMousePosn       = ::wxGetMousePosition();
    m_TreeText            = pTree->GetSnippet(m_TreeItemId);
    if ( IsCategory(m_TreeItemId) )
        m_TreeText = GetSnippetLabel(m_TreeItemId);
    if (m_TreeText.IsEmpty())
        m_pEvtTreeCtrlBeginDrag = 0;
     //LOGIT( _T("TREE_CTRL_BEGIN_DRAG TreeText[%s]"), m_TreeText.GetData() );
    event.Allow();

    // -----------------------------------------
    // Do *not* event.Skip() or GTK will break
    //event.Skip();
    // -----------------------------------------

    return;
}//OnBeginTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnEndTreeItemDrag(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // TREE_END_DRAG
    // -----------------------


    #ifdef LOGGING
     wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();
     LOGIT( _T("TREE_CTRL_END_DRAG %p"), pTree );
    #endif //LOGGING

    wxTreeItemId targetItem = (wxTreeItemId)event.GetItem();
    wxTreeItemId sourceItem = m_MnuAssociatedItemID;
    if ( not sourceItem.IsOk() ){return;}
    if ( not targetItem.IsOk() ){return;}
    if (not m_pEvtTreeCtrlBeginDrag)
    {
        event.Skip(); return;
    }

    // veto the drag if mouse has moved out of the Tree window
        // Note: Even if mouse is dragged out of the tree window,
        //      FindFocus() is returning the Tree window. So the
        //      following test does not work.
        //if (pTree == wxWindow::FindFocus() )
        //    event.Allow();
        //else return;

    // If user dragged item out of the window, it'll be dropped by the
    // target application. So just clear the status and return.
    if (m_bMouseLeftWindow)
    {   // user dragged item out of the window
        m_bMouseLeftWindow = false;
        return;
    }

    event.Allow();
    // if source and target are snippets, create a new category and enclose both.
    if ( IsSnippet(targetItem) )
    {
        if ( targetItem = ConvertSnippetToCategory(targetItem)) {;}//ok
        else return;
        // now targetItem is a category
    }


    // Save the source item node to an Xml Document
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
    if (not m_MouseCtrlKeyDown){
            RemoveItem( sourceItem );
    }

    delete pDoc; pDoc = 0;

    // -----------------------------------------
    // Do *not* event.Skip() or GTK will break
    //event.Skip();
    // -----------------------------------------

    return;

}//OnEndTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnEnterWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // a wxAUI window is not enabling the Tree ctrl so when leaving
    // the disabled TreeCtrl, the scintilla editor doesnt show the cursor.
    wxWindow* pw = (wxWindow*)event.GetEventObject();
    pw->Enable();
    pw->SetFocus();
    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnLeaveWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // LEAVE_WINDOW
    // -----------------------

    // User has dragged mouse out of source window.
    // if EVT_TREE_BEGIN_DRAG is pending, create a drag source to be used
    // in the destination window.

    #ifdef LOGGING
     //LOGIT( _T("MOUSE EVT_LEAVE_WINDOW") );
    #endif //LOGGING

    // Left mouse key must be down (dragging)
    if (not event.LeftIsDown() ) {event.Skip();return;}
    // check if data is available
    if ( m_TreeText.IsEmpty()) {event.Skip();return;}
    if (not m_pEvtTreeCtrlBeginDrag) {event.Skip(); return;}

    #ifdef LOGGING
     LOGIT( _T("LEAVE_WINDOW %p"), event.GetEventObject() );
    #endif //LOGGING

    // when user drags an item out of the window, this routine is called
    // before EVT_END_DRAG, who will clear this flag
    m_bMouseLeftWindow = true;

    // we now have data, create both a simple text and filename drop source
    wxTextDataObject* textData = new wxTextDataObject();
    wxFileDataObject* fileData = new wxFileDataObject();
        // fill text and file sources with snippet
    wxString textStr = GetSnippet(m_MnuAssociatedItemID) ;
    #if defined(BUILDING_PLUGIN)
        // substitute any $(macro) text
        Manager::Get()->GetMacrosManager()->ReplaceMacros(textStr);
        //-LOGIT( _T("SnippetsTreeCtrl OnLeaveWindow $macros text[%s]"),textStr.c_str() );
    #endif
    wxDropSource textSource( *textData, (wxWindow*)event.GetEventObject() );
    textData->SetText( textStr );

    wxDropSource fileSource( *fileData, (wxWindow*)event.GetEventObject() );
    wxString fileName = GetSnippetFileLink(m_MnuAssociatedItemID);
    if (not ::wxFileExists(fileName) ) fileName = wxEmptyString;
    fileData->AddFile( (fileName.Len() > 128) ? wxString(wxEmptyString) : fileName );
        // set composite data object to contain both text and file data
    wxDataObjectComposite *data = new wxDataObjectComposite();
    data->Add( (wxDataObjectSimple*)textData );
    data->Add( (wxDataObjectSimple*)fileData, true ); // set file data as preferred
        // create the drop source containing both data types
    wxDropSource source( *data, (wxWindow*)event.GetEventObject()  );

    #ifdef LOGGING
     LOGIT( _T("DropSource Text[%s],File[%s]"),
                textData->GetText().GetData(),
                fileData->GetFilenames().Item(0).GetData() );
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
            case wxDragCancel:  pc = _T("Cancelled"); break;
            default:            pc = _T("Huh?");      break;
        }
        LOGIT( wxT("ScrapList::OnLeftDown DoDragDrop returned[%s]"),pc.GetData() );
    #else
        wxUnusedVar(result);
    #endif

    // ---WORKAROUNG --------------------------------------------------
    // Since we dragged outside the tree control with an EVT_TREE_DRAG_BEGIN
    // pending, WX will not send the EVT_TREE_DRAG_END from a
    // mouse_key_up event unless the user re-clicks inside the tree control.
    // The mouse is still captured and the tree exibits very bad behavior.
    // Hack:
    // To solve this, send an mouse_key_up to the tree control so it
    // releases the mouse and receives an EVT_TREE_DRAG_END event.

    if ( m_pEvtTreeCtrlBeginDrag )
    {
        //send Mouse LeftKeyUp to Tree Control window
        #ifdef LOGGING
         //LOGIT( _T("Sending Mouse LeftKeyUp") );
        #endif //LOGGING

        // move mouse into the Tree control
        wxPoint CurrentMousePosn = ::wxGetMousePosition();

      #if defined(__WXMSW__)
        MSW_MouseMove( m_TreeMousePosn.x, m_TreeMousePosn.y );
        // send mouse LeftKeyUp
        INPUT    Input={0};
        Input.type          = INPUT_MOUSE;
        Input.mi.dwFlags    = MOUSEEVENTF_LEFTUP;
        ::SendInput(1,&Input,sizeof(INPUT));
        // put mouse back in pre-moved "dropped" position
        MSW_MouseMove( CurrentMousePosn.x, CurrentMousePosn.y );
      #endif //(__WXMSW__)

      #if defined(__WXGTK__)
        // move cursor to source window and send a left button up event
        XWarpPointer (GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()),
                None,              /* not source window -> move from anywhere */
                GDK_WINDOW_XID(GDK_ROOT_PARENT()),  /* dest window */
                0, 0, 0, 0,        /* not source window -> move from anywhere */
                m_TreeMousePosn.x, m_TreeMousePosn.y );
        // send LeftMouseRelease key
        m_pEvtTreeCtrlBeginDrag->SetFocus();
        GdkDisplay* display = gdk_display_get_default ();
        int xx=0,yy=0;
        GdkWindow* pGdkWindow = gdk_display_get_window_at_pointer( display, &xx, &yy);
        // LOGIT(wxT("Tree[%p][%d %d]"), m_pEvtTreeCtrlBeginDrag,m_TreeMousePosn.x, m_TreeMousePosn.y);
        // LOGIT(wxT("gdk [%p][%d %d]"), pWindow, xx, yy);
        GdkEventButton evb;
        memset(&evb, 0, sizeof(evb));
        evb.type = GDK_BUTTON_RELEASE;
        evb.window = pGdkWindow;
        evb.x = xx;
        evb.y = yy;
        evb.state = GDK_BUTTON1_MASK;
        evb.button = 1;
        // gdk display put event, namely mouse release
        gdk_display_put_event( display, (GdkEvent*)&evb);
        // put mouse back in pre-moved "dropped" position
        XWarpPointer (GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()),
                None,              /* not source window -> move from anywhere */
                GDK_WINDOW_XID(GDK_ROOT_PARENT()),  /* dest window */
                0, 0, 0, 0,        /* not source window -> move from anywhere */
                CurrentMousePosn.x, CurrentMousePosn.y );
      #endif//(__WXGTK__)

    }//if

    delete textData; //wxTextDataObject
    delete fileData; //wxFileDataObject
    m_pEvtTreeCtrlBeginDrag = 0;
    m_TreeText = wxEmptyString;

    event.Skip();
    return;
}//OnLeaveWindow
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnMouseEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    //remember event window pointer
    //wxObject* m_pEvtObject = event.GetEventObject();

    // memorize position of the mouse ctrl key as copy/delete flag
    m_MouseCtrlKeyDown = event.ControlDown();
    #ifdef LOGGING
     //LOGIT(wxT("MouseCtrlKeyDown[%s]"), m_MouseCtrlKeyDown?wxT("Down"):wxT("UP") );
    #endif
    event.Skip();

}
// ----------------------------------------------------------------------------
#if defined(__WXMSW__)
void CodeSnippetsTreeCtrl::MSW_MouseMove(int x, int y )
// ----------------------------------------------------------------------------
{
    // Move mouse uses a very strange coordinate system.
    // It uses screen positions with a range of 0 to 65535

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
}
#endif
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
                GetItemText(oldItemId), false );

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
	//wxTreeItemId item = GetSnippetsTreeCtrl->GetFirstChild(parentID, cookie );
    wxTreeItemId item = itemID;

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		const SnippetItemData* data = (SnippetItemData*)GetItemData(item);

		if (!data)
		{
			return;
		}

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
    LOGIT( _T("PgmName[%s]"),pgmName.c_str() );

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
        messageBox( msg );
    }

    // let user edit the snippet text
    // write text to temp file
    // invoke the editor
    // read text back into snippet

    wxFileName tmpFileName = wxFileName::CreateTempFileName(wxEmptyString);

    wxFile tmpFile( tmpFileName.GetFullPath(), wxFile::write);
    if (not tmpFile.IsOpened() )
    {
        messageBox(wxT("Open failed for:")+tmpFileName.GetFullPath());
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
    {   messageBox(wxT("Abort.Error reading temp data file."));
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
        messageBox(wxT("Error reading temp file"));
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
        answer = messageBox(
            wxT("Item is already a file link named:\n")+fileName
                + wxT(" \n\nAre you sure you want to rewrite the file?\n"),
            wxT("Warning"),wxYES|wxNO); //, GetMainFrame(), mousePosn.x, mousePosn.y);
        if ( wxYES == answer)
        {   // read data from old file
            wxFile oldFile( fileName, wxFile::read);
            if (not oldFile.IsOpened() )
            {   messageBox(wxT("Abort.Error reading data file."));
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
    #if defined(BUILDING_PLUGIN)
        // substitute any $(macro) text
        Manager::Get()->GetMacrosManager()->ReplaceMacros(newFileName);
        //-LOGIT( _T("$macros substitute[%s]"),newFileName.c_str() );
    #endif

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
                 wxSAVE | wxOVERWRITE_PROMPT);          //style
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
        messageBox(wxT("Open failed for:")+newFileName);
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
    pdlg->Destroy();
    return (result = (result==wxID_OK));
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippet(SnippetItemData* pSnippetItemData, wxString fileName)
// ----------------------------------------------------------------------------
{
    // just focus any already open snippet items
    int knt = m_aDlgRetcodes.GetCount();
    for (int i = 0; i<knt ; ++i )
    {   EditSnippetFrame* pesf = (EditSnippetFrame*)m_aDlgPtrs.Item(i);
        if (not pesf) continue;
        if ( pesf->GetSnippetId() == GetAssociatedItemID() )
    	{
    	    m_aDlgPtrs.Item(i)->Iconize(false);
    	    m_aDlgPtrs.Item(i)->SetFocus();
            return;
    	}
    }//for
    // Create editor for snippet item
    if (SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID() ))) 	{
        wxString snippetText = itemData->GetSnippet();
        m_aDlgRetcodes.Add(0);
        int* pRetcode = &m_aDlgRetcodes.Last();

        EditSnippetFrame* pdlg = new EditSnippetFrame( GetAssociatedItemID(), pRetcode );
        // cascade multiple editors
        int knt = m_aDlgPtrs.GetCount();
        if (pdlg && (knt > 0) ){
            int x,y;
            pdlg->GetPosition(&x, &y );
             if (0 == x){
                pdlg->ClientToScreen(&x, &y );
             }
            knt = knt<<5;
            pdlg->Move(x+knt,y+knt);
        }

        if ( pdlg->Show() )
        {
            m_aDlgPtrs.Add((wxDialog*)pdlg);
        }
        else
            m_aDlgRetcodes.RemoveAt(m_aDlgRetcodes.GetCount());

	}//if
}

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

    // MIME search fails on a url. Do it brute force
    if ( fileName.StartsWith(wxT("http://")) )
    {   wxLaunchDefaultBrowser( fileName);
        return;
    }

    if ( not ::wxFileExists(fileName) ) return;

    wxString fileNameExt;
    ::wxSplitPath( fileName, /*path*/0, /*name*/0, &fileNameExt);
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
            if ( open )
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
int CodeSnippetsTreeCtrl::ExecuteDialog(wxDialog* pdlg, wxSemaphore& waitSem)
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
//-void CodeSnippetsTreeCtrl::OnIdle(wxIdleEvent& event)
void CodeSnippetsTreeCtrl::OnIdle()
// ----------------------------------------------------------------------------
{
    // check to see if an editor has been posted & finish.

    for (size_t i = 0; i < this->m_aDlgRetcodes.GetCount(); ++i )
    {
        // if we have a return code, this editor is done
        if ( m_aDlgRetcodes.Item(i) == 0)
            continue;

        // else an edit frame is done, save any changed data
        int retcode = m_aDlgRetcodes.Item(i);
        EditSnippetFrame* pdlg = (EditSnippetFrame*)m_aDlgPtrs.Item(i);
        pdlg->MakeModal(false);
		if (retcode == wxID_OK)
		{
            // If XML text snippet, just save back to XML file
            if (pdlg->GetFileName().IsEmpty())
            {
                SnippetItemData* pSnippetItemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(pdlg->GetSnippetId()));
                pSnippetItemData->SetSnippet(pdlg->GetText());
                GetSnippetsTreeCtrl()->SetItemText(pdlg->GetSnippetId(), pdlg->GetName());
            }
            else //This was an external file
            {
                ;// Modified external files already saved by dialog
            }
            // if text item type changed to link, set corrected icon
            if ( pdlg->GetSnippetId().IsOk() )
                SetSnippetImage(pdlg->GetSnippetId());
			SetFileChanged(true);
		}//if
		if (pdlg && (not m_bShutDown) )
        {
            pdlg->Destroy();
        }

        // retcode set by return frame
        //-retcode = pdlg->GetReturnCode();
        m_aDlgRetcodes.Item(i) = 0;
        m_aDlgPtrs.Item(i) = 0;
    }//for

    // when all editors terminate, free array storage
    size_t editorsOpen = 0 ;
    size_t knt = m_aDlgPtrs.GetCount();
    for (size_t i = 0; i < knt; ++i )
    	editorsOpen |= (size_t)m_aDlgPtrs.Item(i);
    if ( knt && (not editorsOpen) )
    {   m_aDlgRetcodes.Clear();
        m_aDlgPtrs.Clear();
    }

    if ( GetConfig()->IsPlugin() )
        GetConfig()->GetMenuBar()->Enable(idViewSnippets, true);

    // if search text is empty, show filename as root item
    // Edit the root node's title so that the user sees file name
    if ( GetConfig()->GetSnippetsSearchCtrl()
        && GetConfig()->GetSnippetsSearchCtrl()->GetValue().IsEmpty() )
    {   wxString nameOnly;
        wxFileName::SplitPath( GetConfig()->SettingsSnippetsXmlFullPath, 0, &nameOnly, 0);
        // avoid excessive refresh
        wxString currentValue = GetItemText(GetSnippetsTreeCtrl()->GetRootItem());
        if (currentValue != nameOnly)
            GetSnippetsTreeCtrl()->SetItemText(GetSnippetsTreeCtrl()->GetRootItem(), wxString::Format(_("%s"), nameOnly.GetData()));
    }

    ////event.Skip();
    return;
}//OnIdle

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnShutdown(wxCloseEvent& event)
// ----------------------------------------------------------------------------
{
////    // Here because our Connect() intercepted wxTheApp EVT_CLOSE
////    // Blink this modeless dialog just like it was a modal dialog
////        //    wxWindow* oldTop = wxTheApp->GetTopWindow();
////        //    wxDialog* pdlg = this->m_pTopDialog;
////        //    wxTheApp->SetTopWindow( pdlg );
////        //    pdlg->RequestUserAttention();
////        //    wxTheApp->SetTopWindow(oldTop);
////        //    event.Veto();
////        //    //event.Skip(); causes app to crash
////
////    // This bool prevents crash when CodeBlocks is shutdown;
////    this->m_bShutDown = true;
////    for (size_t i = 0; i < this->m_aDlgPtrs.GetCount(); ++i )
////    {
////        wxDialog* pdlg = this->m_aDlgPtrs.Item(i);
////        if (pdlg) pdlg->ProcessEvent(event);
////    }
////    #if defined(BUILDING_PLUGIN)
////      // Enable the plugin View menu item
////        asm("int3");
////        Manager::Get()->GetAppWindow()->GetMenuBar()->Enable(idViewSnippets, true);
////    #endif
////
////    event.Skip();
////    return;
}
// ----------------------------------------------------------------------------
