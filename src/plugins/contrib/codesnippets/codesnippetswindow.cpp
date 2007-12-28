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
// RCS-ID: $Id: codesnippetswindow.cpp 109 2007-12-06 15:04:10Z Pecan $

#ifdef WX_PRECOMP //
    #include "wx_pch.h"
#else
    #include <wx/button.h>
    #include <wx/imaglist.h>
    #include <wx/intl.h>
    #include <wx/sizer.h>
    #include <wx/textctrl.h>
    #include <wx/menu.h>
    #include <wx/textdlg.h>
    #include <wx/colour.h>
    #include <wx/filedlg.h>
    #include <wx/tokenzr.h>
#endif
    #include <wx/filename.h>
    #include <wx/toplevel.h>

// wxWidget headers not include in wx_pch.h
    #include <wx/clipbrd.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#if defined(BUILDING_PLUGIN)
    #include "sdk.h"
    #ifndef CB_PRECOMP
        #include "manager.h"
        #include "logmanager.h"
        #include "macrosmanager.h"
        #include "configmanager.h"
        #include "editormanager.h"
        #include "wxscintilla/include/wx/wxscintilla.h"
        #include "cbeditor.h"
        #include "globals.h"
    #endif
	#include "cbstyledtextctrl.h"
#endif

// project headers
#include "version.h"
#include "codesnippetswindow.h"
#include "snippetitemdata.h"
#include <tinyxml/tinyxml.h>
#include "snippetsconfig.h"
#include "snippetsimages.h"
#include "codesnippetstreectrl.h"
#include "messagebox.h"
#include "settingsdlg.h"
#include "menuidentifiers.h"


//-#define SNIPPETS_TREE_IMAGE_COUNT 3
// above redefined in snipimages.h to 6

//const wxString snippetsTreeImageFileNames[] = {
//	_T("allsnippets.png"),
//	_T("category.png"),
//	_T("snippet.png")
//};

int idSearchSnippetCtrl         = wxNewId();
int idSearchCfgBtn              = wxNewId();
int idSnippetsTreeCtrl          = wxNewId();

    // Context Menu items
int idMnuAddSubCategory         = wxNewId();
int idMnuRemove                 = wxNewId();
int idMnuRename                 = wxNewId();
int idMnuCopy                   = wxNewId();
int idMnuPaste                  = wxNewId();
int idMnuConvertToCategory      = wxNewId();
int idMnuAddSnippet             = wxNewId();
int idMnuApplySnippet           = wxNewId();
int idMnuLoadSnippetsFromFile   = wxNewId();
int idMnuSaveSnippets           = wxNewId();
int idMnuSaveSnippetsToFile     = wxNewId();
int idMnuFileBackup             = wxNewId();
int idMnuRemoveAll              = wxNewId();
int idMnuCopyToClipboard        = wxNewId();
int idMnuEditSnippet            = wxNewId();
int idMnuOpenFileLink           = wxNewId();
int idMnuConvertToFileLink      = wxNewId();
int idMnuProperties             = wxNewId();
int idMnuSettings               = wxNewId();
int idMnuAbout                  = wxNewId();

// Search config menu items
int idMnuCaseSensitive          = wxNewId();
int idMnuClear                  = wxNewId();
int idMnuScope                  = wxNewId();
int idMnuScopeSnippets          = wxNewId();
int idMnuScopeCategories        = wxNewId();
int idMnuScopeBoth              = wxNewId();

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CodeSnippetsWindow, wxPanel)
    // ---
	// Tree menu events
	EVT_MENU(idMnuRemove,           CodeSnippetsWindow::OnMnuRemove)
	EVT_MENU(idMnuRename,           CodeSnippetsWindow::OnMnuRename)
	EVT_MENU(idMnuCopy,             CodeSnippetsWindow::OnMnuCopy)
	EVT_MENU(idMnuPaste,            CodeSnippetsWindow::OnMnuPaste)
	EVT_MENU(idMnuConvertToCategory,CodeSnippetsWindow::OnMnuConvertToCategory)
	EVT_MENU(idMnuAddSubCategory,   CodeSnippetsWindow::OnMnuAddSubCategory)
	EVT_MENU(idMnuAddSnippet,       CodeSnippetsWindow::OnMnuAddSnippet)
	EVT_MENU(idMnuApplySnippet,     CodeSnippetsWindow::OnMnuApplySnippet)
	EVT_MENU(idMnuLoadSnippetsFromFile, CodeSnippetsWindow::OnMnuLoadSnippetsFromFile)
	EVT_MENU(idMnuSaveSnippets,     CodeSnippetsWindow::OnMnuSaveSnippets)
	EVT_MENU(idMnuSaveSnippetsToFile,CodeSnippetsWindow::OnMnuSaveSnippetsAs)
	EVT_MENU(idMnuFileBackup,       CodeSnippetsWindow::OnMnuFileBackup)
	EVT_MENU(idMnuRemoveAll,        CodeSnippetsWindow::OnMnuRemoveAll)
	EVT_MENU(idMnuCopyToClipboard,  CodeSnippetsWindow::OnMnuCopyToClipboard)
	EVT_MENU(idMnuEditSnippet,      CodeSnippetsWindow::OnMnuEditSnippet)
	EVT_MENU(idMnuOpenFileLink,     CodeSnippetsWindow::OnMnuOpenFileLink)
	EVT_MENU(idMnuConvertToFileLink,CodeSnippetsWindow::OnMnuSaveSnippetAsFileLink)
	EVT_MENU(idMnuProperties,       CodeSnippetsWindow::OnMnuProperties)
	EVT_MENU(idMnuSettings,         CodeSnippetsWindow::OnMnuSettings)
	EVT_MENU(idMnuAbout,            CodeSnippetsWindow::OnMnuAbout)
	// ---

	// Search config menu event
	EVT_MENU(idMnuCaseSensitive, CodeSnippetsWindow::OnMnuCaseSensitive)
	EVT_MENU(idMnuScopeSnippets, CodeSnippetsWindow::OnMnuChangeScope)
	EVT_MENU(idMnuScopeCategories, CodeSnippetsWindow::OnMnuChangeScope)
	EVT_MENU(idMnuScopeBoth, CodeSnippetsWindow::OnMnuChangeScope)
	EVT_MENU(idMnuClear, CodeSnippetsWindow::OnMnuClear)
	// ---

	EVT_BUTTON(idSearchCfgBtn, CodeSnippetsWindow::OnSearchCfg)
	EVT_TEXT(idSearchSnippetCtrl, CodeSnippetsWindow::OnSearch)
	EVT_TREE_ITEM_ACTIVATED(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemActivated)
	EVT_TREE_ITEM_MENU(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemMenu)
	EVT_TREE_BEGIN_DRAG(idSnippetsTreeCtrl, CodeSnippetsWindow::OnBeginDrag)
	EVT_TREE_END_DRAG(idSnippetsTreeCtrl, CodeSnippetsWindow::OnEndDrag)
	EVT_TREE_BEGIN_LABEL_EDIT(idSnippetsTreeCtrl, CodeSnippetsWindow::OnBeginLabelEdit)
	EVT_TREE_END_LABEL_EDIT(idSnippetsTreeCtrl, CodeSnippetsWindow::OnEndLabelEdit)
	EVT_TREE_ITEM_GETTOOLTIP(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemGetToolTip)
	// ---
	// EVT_CLOSE Doesn't work on wxAUI windows, this is called from a Connect()
	EVT_CLOSE( CodeSnippetsWindow::OnClose) //never occurs with wxAUI
	// EVT_IDLE(   CodeSnippetsWindow::OnIdle) //works ok

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsWindow::CodeSnippetsWindow(wxWindow* parent)
// ----------------------------------------------------------------------------
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,wxTAB_TRAVERSAL,wxT("csPanel"))
{
    m_isCheckingForExternallyModifiedFiles = false;
    pTiXmlDoc = 0;

    if (not GetConfig()->pMainFrame)
        GetConfig()->pMainFrame = parent;

	// Initialize the dialog
	InitDlg();
	m_AppendItemsFromFile = false;

	// Load the conf/ini settings
	GetConfig()->SettingsLoad();

    wxString s(__FUNCTION__, wxConvUTF8);
    LOGIT(s+wxT("LoadingFile:%s"),GetConfig()->SettingsSnippetsXmlFullPath.c_str());

	// Load the snippets
	GetSnippetsTreeCtrl()->LoadItemsFromFile(GetConfig()->SettingsSnippetsXmlFullPath, false);
    GetSnippetsTreeCtrl()->SaveFileModificationTime();
}
// ----------------------------------------------------------------------------
CodeSnippetsWindow::~CodeSnippetsWindow()
// ----------------------------------------------------------------------------
{
	// Save the snippets
	if ( GetFileChanged() )
        GetSnippetsTreeCtrl()->SaveItemsToFile(GetConfig()->SettingsSnippetsXmlFullPath);

    if (pTiXmlDoc) { delete pTiXmlDoc; pTiXmlDoc = 0;}
  	GetConfig()->pSnippetsSearchCtrl = 0;
}
// ----------------------------------------------------------------------------
// EVT_CLOSE is not generated from wxAUI windows wx2.6.3
// This routine is hooked from it's parent
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnClose(wxCloseEvent& event)
// ----------------------------------------------------------------------------
{
    // this routine is Connect()ed for wxAUI->OnClose
    // in codesnippets.cpp CreateSnippetsWindow() routine
    // and Disconnect() in codesnippets.cpp OnIdle() when the
    // window is closed.

    if ( GetConfig()->m_appIsShutdown) { event.Skip(); return;}

    if ( not GetConfig()->GetSnippetsWindow() )
        {event.Skip();return;}
     LOGIT( _T("CodeSnippetsWindow::Onclose Saving Settings"));
    GetConfig()->SettingsSave();
    if (GetConfig()->IsPlugin())
    {
        if ( GetConfig()->IsFloatingWindow() )
        {   GetConfig()->SettingsSaveWinPosition();
            // This connect was made in codesnippets.cpp OnViewSnippets()
            // in order to catch the system close button
            // This Disconnect causes a crash on wxMSW even tho I've verified the pointer is ok
            //-if ( GetConfig()->m_pEvtCloseConnect )
            //-{    GetConfig()->m_pEvtCloseConnect->Disconnect( wxEVT_CLOSE_WINDOW,
            //-        (wxObjectEventFunction)(wxEventFunction)
            //-        (wxCloseEventFunction) &CodeSnippetsWindow::OnClose,NULL,this);
            //-    GetConfig()->m_pEvtCloseConnect = 0;
            //-}
        }//if
    }//if

    // crash on shutdown if we don't do this test
    if ((not GetConfig()->m_appIsShutdown) && (not GetConfig()->m_appIsDisabled))
    {
        wxMenuBar* pbar = GetConfig()->m_pMenuBar;
        pbar->Check(idViewSnippets, false);
    }

    GetConfig()->m_pEvtCloseConnect = 0;
    Destroy();
    GetConfig()->pSnippetsWindow = 0;
    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::InitDlg()
// ----------------------------------------------------------------------------
{
	// Color which we're going to use as mask
	wxColor maskColor(255, 0, 255);

	wxBoxSizer* parentSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* searchCtrlSizer = new wxBoxSizer(wxHORIZONTAL);

	m_SearchSnippetCtrl = new wxTextCtrl(this, idSearchSnippetCtrl, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	searchCtrlSizer->Add(m_SearchSnippetCtrl, 1, wxBOTTOM|wxLEFT|wxTOP, 5);

	m_SearchCfgBtn = new wxButton(this, idSearchCfgBtn, _T(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	searchCtrlSizer->Add(m_SearchCfgBtn, 0, wxBOTTOM|wxRIGHT|wxTOP, 5);

	parentSizer->Add(searchCtrlSizer, 0, wxEXPAND, 5);

	wxBoxSizer* treeCtrlSizer = new wxBoxSizer(wxVERTICAL);

	m_SnippetsTreeCtrl = new CodeSnippetsTreeCtrl(this, idSnippetsTreeCtrl, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS);
	treeCtrlSizer->Add(GetSnippetsTreeCtrl(), 1, wxEXPAND, 5);

	parentSizer->Add(treeCtrlSizer, 1, wxEXPAND, 5);

	SetSizer(parentSizer);
	Layout();

	GetSnippetsTreeCtrl()->SetDropTarget(new SnippetsDropTarget(GetSnippetsTreeCtrl()));

    // set the treeCtrl image list
	GetSnippetsTreeCtrl()->SetImageList(GetSnipImageList());

	// Add root item
	SnippetItemData* rootData = new SnippetItemData(SnippetItemData::TYPE_ROOT);
	GetSnippetsTreeCtrl()->AddRoot(_("All snippets"), 0, -1, rootData);
	GetConfig()->pSnippetsSearchCtrl = m_SearchSnippetCtrl;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnSearchCfg(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	wxMenu* searchCfgMenu = new wxMenu();
	wxMenu* searchScopeMenu = new wxMenu();

	searchScopeMenu->AppendRadioItem(idMnuScopeSnippets, _("Snippets"));
	searchScopeMenu->AppendRadioItem(idMnuScopeCategories, _("Categories"));
	searchScopeMenu->AppendRadioItem(idMnuScopeBoth, _("Snippets and categories"));
	switch (GetConfig()->m_SearchConfig.scope)
	{
		case CodeSnippetsConfig::SCOPE_SNIPPETS:
			searchScopeMenu->Check(idMnuScopeSnippets, true);
		break;

		case CodeSnippetsConfig::SCOPE_CATEGORIES:
			searchScopeMenu->Check(idMnuScopeCategories, true);
		break;

		case CodeSnippetsConfig::SCOPE_BOTH:
			searchScopeMenu->Check(idMnuScopeBoth, true);
		break;
	}

	searchCfgMenu->AppendCheckItem(idMnuCaseSensitive, _("Case sensitive"));
	if (GetConfig()->m_SearchConfig.caseSensitive)
	{
		searchCfgMenu->Check(idMnuCaseSensitive, true);
	}

	searchCfgMenu->Append(idMnuScope, _("Scope"), searchScopeMenu);
	searchCfgMenu->AppendSeparator();
	searchCfgMenu->Append(idMnuClear, _("Clear"));

	if (m_SearchSnippetCtrl->GetValue().IsEmpty())
	{
		searchCfgMenu->Enable(idMnuClear, false);
	}

	// Always pop up the menu at the same position
	wxRect btnRect = m_SearchCfgBtn->GetRect();
	PopupMenu(searchCfgMenu, btnRect.x + btnRect.GetWidth(), btnRect.y);

	searchCfgMenu->Destroy(idMnuScope);
	delete searchCfgMenu;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnSearch(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	if (m_SearchSnippetCtrl->GetValue().IsEmpty())
	{
		// Reset the root node's title
		GetSnippetsTreeCtrl()->SetItemText(GetSnippetsTreeCtrl()->GetRootItem(), _("All snippets"));

		// Reset the searchbox's background color
		m_SearchSnippetCtrl->SetBackgroundColour(wxNullColour);
		m_SearchSnippetCtrl->Refresh();
	}
	else
	{
		// Edit the root node's title so that the user knows we are
		// searching for specific item
		GetSnippetsTreeCtrl()->SetItemText(GetSnippetsTreeCtrl()->GetRootItem(), wxString::Format(_("Search \"%s\""), m_SearchSnippetCtrl->GetValue().c_str()));

		// Search it!
		wxString searchTerms = m_SearchSnippetCtrl->GetValue();
		if (not GetConfig()->m_SearchConfig.caseSensitive)
		{
			searchTerms.LowerCase();
		}

		wxTreeItemId foundID = SearchSnippet(searchTerms, GetSnippetsTreeCtrl()->GetRootItem());

		if (foundID.IsOk())
		{
			// Highlight the item
			GetSnippetsTreeCtrl()->EnsureVisible(foundID);
			GetSnippetsTreeCtrl()->SelectItem(foundID);

			// Reset the searchbox's backgroud color
			m_SearchSnippetCtrl->SetBackgroundColour(wxNullColour);
			m_SearchSnippetCtrl->Refresh();
		}
		else
		{
			// Select the root item so user doesn't think we found something
			GetSnippetsTreeCtrl()->EnsureVisible(GetSnippetsTreeCtrl()->GetRootItem());
			GetSnippetsTreeCtrl()->SelectItem(GetSnippetsTreeCtrl()->GetRootItem());

			// Add visual feedback: paint the searchbox's background with
			// light red color
			m_SearchSnippetCtrl->SetBackgroundColour(wxColor(244, 168, 168));
			m_SearchSnippetCtrl->Refresh();
		}
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnItemActivated(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{

    // Double click on snippet item
    // if shift key is down, apply the snippet, else edit it.

    if ( IsTreeBusy() ) return;

    if (::wxGetKeyState(WXK_SHIFT) )
    {    ApplySnippet(event.GetItem());
        return;
    }

    wxCommandEvent ev;

    if (::wxGetKeyState(WXK_ALT) )
    {   OnMnuOpenFileLink( ev );
        return;
    }

    OnMnuEditSnippet( ev )        ;
    return;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnItemMenu(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // The context menu for the selected item has been requested, either by a
    // right click or by using the menu key.

    if ( IsTreeBusy() ) return;

	// Get the item associated with the event
	wxTreeItemId itemId = event.GetItem();

	if (const SnippetItemData* eventItem = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(event.GetItem())))
	{
		wxMenu* snippetsTreeMenu = new wxMenu();

		// Check the type of the item and add the menu items
		switch (eventItem->GetType())
		{
		    // -----------------
		    // Root context menu
		    // -----------------
			case SnippetItemData::TYPE_ROOT:
				snippetsTreeMenu->Append(idMnuAddSnippet, _("Add Snippet"));
				snippetsTreeMenu->Append(idMnuAddSubCategory, _("Add SubCategory"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuPaste, _("Paste Items"));
				snippetsTreeMenu->Enable(idMnuPaste, pTiXmlDoc);
				snippetsTreeMenu->Append(idMnuRemoveAll, _("Remove all items"));

				// Disable "Remove all items" if the root item does not have child items
				if (!GetSnippetsTreeCtrl()->ItemHasChildren(GetSnippetsTreeCtrl()->GetRootItem()))
					snippetsTreeMenu->Enable(idMnuRemoveAll, false);

                if ( GetConfig()->IsPlugin() )
                {
                    snippetsTreeMenu->AppendSeparator();
                    snippetsTreeMenu->Append(idMnuSaveSnippets, _("Save Index"));
                    snippetsTreeMenu->Append(idMnuSaveSnippetsToFile, _("Save Index As..."));
                    snippetsTreeMenu->Append(idMnuFileBackup, _("Backup Main Index"));

                    // Disable "Save to file..." if the root item does not have child items
                    if (!GetSnippetsTreeCtrl()->ItemHasChildren(GetSnippetsTreeCtrl()->GetRootItem()))
                    {	snippetsTreeMenu->Enable(idMnuSaveSnippetsToFile, false);
                        snippetsTreeMenu->Enable(idMnuSaveSnippets, false);
                        snippetsTreeMenu->Enable(idMnuFileBackup, false);
                    }

                    // Check if Shift key is pressed
                    if (::wxGetKeyState(WXK_SHIFT))
                    {
                        // Add append from file entry
                        snippetsTreeMenu->Append(idMnuLoadSnippetsFromFile, _("Load Index File (append)..."));
                        m_AppendItemsFromFile = true;
                    }
                    else
                    {
                        // Use the normal load from file entry
                        snippetsTreeMenu->Append(idMnuLoadSnippetsFromFile, _("Load Index File..."));
                        m_AppendItemsFromFile = false;
                    }
                }//if IsPlugin

				snippetsTreeMenu->AppendSeparator();
                snippetsTreeMenu->Append(idMnuSettings, _("Settings..."));
               if ( GetConfig()->IsPlugin() )
                    snippetsTreeMenu->Append(idMnuAbout, _("About..."));
			break;

            // ---------------------
            // Category context menu
            // ---------------------
			case SnippetItemData::TYPE_CATEGORY:
				snippetsTreeMenu->Append(idMnuAddSnippet, _("Add Snippet"));
				snippetsTreeMenu->Append(idMnuAddSubCategory, _("Add SubCategory"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuCopy,  _("Copy  Category"));
				snippetsTreeMenu->Append(idMnuPaste, _("Paste Tree Items"));
				snippetsTreeMenu->Enable(idMnuPaste, pTiXmlDoc);
				snippetsTreeMenu->Append(idMnuRename, _("Rename"));
				snippetsTreeMenu->Append(idMnuRemove, _("Remove"));
			break;

            // --------------------
            // Snippet context menu
            // --------------------
			case SnippetItemData::TYPE_SNIPPET:
                if ( IsFileSnippet(itemId) )
                {   snippetsTreeMenu->Append(idMnuEditSnippet, _("Edit File"));
                    snippetsTreeMenu->Append(idMnuOpenFileLink,_("Open File"));
                }
                else // it's a text snippet
                {   snippetsTreeMenu->Append(idMnuEditSnippet, _("Edit Text"));
                    if ( IsUrlSnippet() )
                        snippetsTreeMenu->Append(idMnuOpenFileLink,_("Open Url"));
                }

                #if defined(BUILDING_PLUGIN)
                if (GetConfig()->IsPlugin())
                {   snippetsTreeMenu->Append(idMnuApplySnippet, _("Apply"));
                }
                #endif
                if ( IsFileSnippet(itemId) )
                    snippetsTreeMenu->Append(idMnuCopyToClipboard, _("Clipboard <= FileName"));
                else
                    snippetsTreeMenu->Append(idMnuCopyToClipboard, _("Clipboard <= Text"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuConvertToCategory, _("Convert to Category"));
				snippetsTreeMenu->Append(idMnuConvertToFileLink, _("Convert to File Link..."));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuCopy,  _("Copy Tree Item"));
				snippetsTreeMenu->Append(idMnuPaste, _("Paste Tree Item"));
				snippetsTreeMenu->Enable(idMnuPaste, pTiXmlDoc);
				snippetsTreeMenu->Append(idMnuRemove, _("Remove"));
				snippetsTreeMenu->AppendSeparator();
                snippetsTreeMenu->Append(idMnuProperties, _("Properties..."));
			break;
		}

		// Save the item ID for later use
		//-m_MnuAssociatedItemID = eventItem->GetId();
		GetSnippetsTreeCtrl()->SetAssociatedItemID( eventItem->GetId() );

		PopupMenu(snippetsTreeMenu);

		delete snippetsTreeMenu;
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnBeginDrag(wxTreeEvent& /*event*/)
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnEndDrag(wxTreeEvent& /*event*/)
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuAddSubCategory(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	// Add new category using the associated item ID
	CodeSnippetsTreeCtrl* pTree = GetSnippetsTreeCtrl();
	wxTreeItemId newItem =
        GetSnippetsTreeCtrl()->AddCategory(GetSnippetsTreeCtrl()->GetAssociatedItemID(), _("New category"), true);
	SetFileChanged(true);
	if (newItem.IsOk())
	{
	    pTree->SelectItem(newItem);
	    pTree->SetAssociatedItemID(newItem);
        OnMnuRename(event);
	}
	if ( (newItem.IsOk()) && (pTree->GetItemText(newItem).IsEmpty()) )
        pTree->RemoveItem(newItem);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuRemove(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Get the associated item id
	wxTreeItemId itemID = GetAssociatedItemID();

//	// Sanity check
//	if (itemID != GetSnippetsTreeCtrl()->GetRootItem())
//	{
//		// No questions asked
//		GetSnippetsTreeCtrl()->DeleteChildren(itemID);
//		GetSnippetsTreeCtrl()->Delete(itemID);
//		SetFileChanged(true);
//	}
    GetSnippetsTreeCtrl()->RemoveItem(itemID);
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuRename(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	// Get the associated item id
	wxTreeItemId itemID = GetAssociatedItemID();

    CodeSnippetsTreeCtrl* pTree = GetSnippetsTreeCtrl();
    wxString itemLabel = pTree->GetItemText(itemID);
    wxPoint currentMousePosn = ::wxGetMousePosition();

    // get new label(on ok) or empty string(on cancel)
    wxString newLabel = ::wxGetTextFromUser(wxT("New Category Label"),wxT("Rename"),
        itemLabel, pTree,
        currentMousePosn.x, currentMousePosn.y,false);
    LOGIT( _T("GetTextFromUser[%s] oldLabel[%s]"),newLabel.c_str(),itemLabel.c_str() );

    // label may have been edited
    if (not newLabel.IsEmpty())
        pTree->SetItemText( itemID, newLabel );

    // remove unlabeled item
	if (itemID.IsOk() && pTree->GetItemText(itemID).IsEmpty())
        pTree->RemoveItem(itemID);

}//OnMnuRename
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuCopy(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // copy a tree item structure
    SetActiveMenuId( event.GetId() );
    if (pTiXmlDoc) { delete pTiXmlDoc; pTiXmlDoc = 0;}
    pTiXmlDoc = GetSnippetsTreeCtrl()->CopyTreeNodeToXmlDoc( GetAssociatedItemID() );
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuPaste(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // paste a tree item structure into another tree item structure
    SetActiveMenuId( event.GetId() );
    if (not pTiXmlDoc) return;

    // if target item is a snippet type, first convert it to a category
    wxTreeItemId targetItem = GetAssociatedItemID();
    if ( IsSnippet(targetItem) )
    {
        if ( targetItem = GetSnippetsTreeCtrl()->ConvertSnippetToCategory(targetItem))
        {;}//ok
        else return;
        // now targetItem is a category
    }

    GetSnippetsTreeCtrl()->CopyXmlDocToTreeNode( pTiXmlDoc, targetItem  );
    if (pTiXmlDoc){ delete pTiXmlDoc; pTiXmlDoc = 0; }
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuConvertToCategory(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // create category to hold this snippet
    // place old snippet under new category
    // delete the old snippet

    SetActiveMenuId( event.GetId() );
    GetSnippetsTreeCtrl()->ConvertSnippetToCategory( GetAssociatedItemID());

}//OnMnuConvertToCategory

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuAddSnippet(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Add new snippet using the associated item ID
	GetSnippetsTreeCtrl()->AddCodeSnippet(GetAssociatedItemID(), _("New snippet"), wxEmptyString, true);
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::ApplySnippet(const wxTreeItemId& itemID)
// ----------------------------------------------------------------------------
{
	// Get the item
	if (const SnippetItemData* item = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(itemID)))
	{
		// Check that we're using the correct item type
		if (item->GetType() != SnippetItemData::TYPE_SNIPPET)
		{
			return;
		}
      #if defined(BUILDING_PLUGIN)
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
			wxString snippet = item->GetSnippet();
			CheckForMacros(snippet);
			//wxLeaner: http://forums.codeblocks.org/index.php/topic,5375.new.html#new
			// Honor target source line indentation
            snippet.Replace(wxT("\n"), wxT('\n') + editor->GetLineIndentString(ctrl->GetCurrentLine()));
            ctrl->AddText(snippet);
		}
	  #else //NOT defined(BUILDING_PLUGIN)
        AddTextToClipBoard( item->GetSnippet() );
	  #endif //defined(BUILDING_PLUGIN)
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuApplySnippet(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Apply the snippet using the associated item id
	ApplySnippet(GetAssociatedItemID());
}

// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsWindow::SearchSnippet(const wxString& searchTerms, const wxTreeItemId& node)
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
					if (GetConfig()->m_SearchConfig.scope == GetConfig()->SCOPE_CATEGORIES)
					{
						ignoreThisType = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if (GetConfig()->m_SearchConfig.scope == GetConfig()->SCOPE_SNIPPETS)
					{
						ignoreThisType = true;
					}
				break;
			}

			if (!ignoreThisType)
			{
				wxString label = GetSnippetsTreeCtrl()->GetItemText(item);

				if (not GetConfig()->m_SearchConfig.caseSensitive)
				{
					label.LowerCase();
				}

				if(label.Contains(searchTerms))
				{
					return item;
				}
			}

			if(GetSnippetsTreeCtrl()->ItemHasChildren(item))
			{
				wxTreeItemId search = SearchSnippet(searchTerms, item);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetSnippetsTreeCtrl()->GetNextChild(node, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem;
   return dummyItem;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnBeginLabelEdit(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	// Deny editing of the root item
	if (event.GetItem() == GetSnippetsTreeCtrl()->GetRootItem())
	{
		event.Veto();
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuLoadSnippetsFromFile(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	wxFileDialog dlg(this, _("Load snippets from file"), wxEmptyString, wxEmptyString, _("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxOPEN|wxFILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
        LOGIT(wxT("LoadingFile:%s"),dlg.GetPath().c_str());
		GetSnippetsTreeCtrl()->LoadItemsFromFile(dlg.GetPath(), m_AppendItemsFromFile);
        GetConfig()->SettingsSnippetsXmlFullPath = dlg.GetPath();
        SetFileChanged(false);
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSaveSnippets(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	{
		GetSnippetsTreeCtrl()->SaveItemsToFile(GetConfig()->SettingsSnippetsXmlFullPath);
		SetFileChanged(false);
	}
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSaveSnippetsAs(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	wxFileDialog dlg(this, _("Save snippets to file"), wxEmptyString, _T("codesnippets.xml"), _("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		GetSnippetsTreeCtrl()->SaveItemsToFile(dlg.GetPath());
		SetFileChanged(false);
	}
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuFileBackup(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // save the current XML data if changed
    // append the next number onto the XML file name
    // copy the XML Index to the new file name

    if (GetFileChanged() ) OnMnuSaveSnippets(event);
    const wxString IndexFile = GetConfig()->SettingsSnippetsXmlFullPath;
    wxString bkupName = wxEmptyString;
    unsigned i = 0;
    while (true)
    {
        ++i;
        bkupName = IndexFile;
        bkupName << wxT(".")  << i;
        if ( ::wxFileExists(bkupName) ) {continue;}
        break;
    }//while
    int done = ::wxCopyFile(IndexFile, bkupName);
    messageBox( wxString::Format( wxT("Backup %s for\n\n %s"),
                done?wxT("succeeded"):wxT("failed"),
                bkupName.c_str()) );

}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnEndLabelEdit(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	// Sort all the parent item's children
	GetSnippetsTreeCtrl()->SortChildren(GetSnippetsTreeCtrl()->GetItemParent(event.GetItem()));
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuRemoveAll(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Remove all items
	GetSnippetsTreeCtrl()->DeleteChildren(GetSnippetsTreeCtrl()->GetRootItem());
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuCaseSensitive(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	GetConfig()->m_SearchConfig.caseSensitive = (not GetConfig()->m_SearchConfig.caseSensitive);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuChangeScope(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	if (event.GetId() == idMnuScopeSnippets)
	{
		GetConfig()->m_SearchConfig.scope = GetConfig()->SCOPE_SNIPPETS;
	}
	else if (event.GetId() == idMnuScopeCategories)
	{
		GetConfig()->m_SearchConfig.scope = GetConfig()->SCOPE_CATEGORIES;
	}
	else if (event.GetId() == idMnuScopeBoth)
	{
		GetConfig()->m_SearchConfig.scope = GetConfig()->SCOPE_BOTH;
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuClear(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // search->clear context menu item
	m_SearchSnippetCtrl->Clear();

}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuCopyToClipboard(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	if (wxTheClipboard->Open())
	{
		const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID()));
		if (itemData)
		{
		    wxString itemStr = itemData->GetSnippet();
            #if defined(BUILDING_PLUGIN)
                Manager::Get()->GetMacrosManager()->ReplaceMacros(itemStr);
            #endif

			wxTheClipboard->SetData(new wxTextDataObject(itemStr));
			wxTheClipboard->Close();
		}
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuEditSnippet(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{

    if (not IsSnippet() ) return;
    CodeSnippetsTreeCtrl* pTree = GetConfig()->GetSnippetsTreeCtrl();
	if (SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID() )))
	{
        if (not itemData){;} //variable unused

        wxTreeItemId itemId = GetAssociatedItemID();
        wxString FileName = pTree->GetSnippetFileLink( itemId );
        LOGIT( _T("OnMnuEditSnippet FileName[%s]"),FileName.c_str() );

        // If snippet is text, edit it as text
        if (FileName.Length() > 128)
        {   // if text is > 128 characters, open a temp file with snippet text as data.
            GetSnippetsTreeCtrl()->EditSnippetAsText();
            return;
        }

        // If snippet is non-existent file, edit as text
            if ( (FileName.IsEmpty())
                || (not ::wxFileExists( FileName)) )
            {   // if, non-existent file, open snippet text as data
                GetSnippetsTreeCtrl()->EditSnippetAsText();
                return;
            }

        // else edit snippet as file link
        GetSnippetsTreeCtrl()->EditSnippetAsFileLink();
	}
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuOpenFileLink(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{

    if (not IsSnippet() ) return;
    CodeSnippetsTreeCtrl* pTree = GetConfig()->GetSnippetsTreeCtrl();
	if (SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID() )))
	{
        if (not itemData){;} //variable unused

        wxTreeItemId itemId = GetAssociatedItemID();
        wxString FileName = pTree->GetSnippetFileLink( itemId );
        LOGIT( _T("OnMnuOpenFileLink FileName[%s]"),FileName.c_str() );

        // If snippet is text, edit it as text
        if (FileName.Length() > 128)
        {   // if text is > 128 characters, open a temp file with snippet text as data.
            GetSnippetsTreeCtrl()->EditSnippetAsText();
            return;
        }

        GetSnippetsTreeCtrl()->OpenSnippetAsFileLink();
	}
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::CheckForMacros(wxString& snippet)
// ----------------------------------------------------------------------------
{
    //FIXME: CheckForMacros in App???
	// Copied from cbEditor::Autocomplete, I admit it

    wxPoint currentMousePosn = ::wxGetMousePosition();
	int macroPos = snippet.Find(_T("$("));
	int currPosn = macroPos;
	while (macroPos != -1)
	{
		// locate ending parenthesis
		int macroPosEnd = macroPos + 2;
		int len = (int)snippet.Length();

		while (macroPosEnd < len && snippet.GetChar(macroPosEnd) != _T(')'))
			++macroPosEnd;

		if (macroPosEnd == len)
			break; // no ending parenthesis

		wxString macroName = snippet.SubString(macroPos + 2, macroPosEnd - 1);
        wxString defaultResult = snippet.SubString(macroPos,macroPosEnd);
        #if defined(BUILDING_PLUGIN)
          Manager::Get()->GetMacrosManager()->ReplaceMacros(defaultResult);
        #endif

		wxString macro = wxGetTextFromUser(wxString::Format(_("Please enter the text for \"%s\":"), macroName.c_str()),
            _("Macro substitution"),defaultResult,0,
            currentMousePosn.x, currentMousePosn.y,false);
		if (not macro.IsEmpty())
            snippet.Replace(_T("$(") + macroName + _T(")"), macro);

		//-macroPos = snippet.Find(_T("$("));
		currPosn = currPosn + macroPos + 1;
		macroPos = snippet.Mid(currPosn).Find(_T("$("));
		if (macroPos == -1) break;
		macroPos = macroPos + currPosn;
		#if defined(LOGGING)
		//LOGIT( _T("CheckForMacros:currPosn[%d],macroPos[%d],Macro[%s]"),
        //    currPosn, macroPos, snippet.Mid(macroPos).c_str());
		#endif
	}
}//CheckForMacros
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnItemGetToolTip(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	// "My eyes! The goggles do nothing!"
	//
	// Use the following only on wxWidgets 2.8.
	#if wxCHECK_VERSION(2, 8, 0)
	if (const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(event.GetItem())))
	{
		if (itemData->GetType() == SnippetItemData::TYPE_SNIPPET)
		{
			wxString snippetToolTip = itemData->GetSnippet();
			size_t originalLength = snippetToolTip.Len();

			//// Take the first 255 characters or less, note that the
			//// wxWidgets documentation doesn't say what is maximum lenght of
			//// the tooltip so this can be increased if needed.

			// Use only first line of snippet
			snippetToolTip = snippetToolTip.BeforeFirst('\r');
			snippetToolTip = snippetToolTip.BeforeFirst('\n');
			size_t charsInToolTip = 128;
			snippetToolTip = snippetToolTip.Mid(0, charsInToolTip);

			// Replace all tabs with spaces; tabs break the tooltips
			snippetToolTip.Replace(_T("\t"), _T("    "));

			if (snippetToolTip.Len() > charsInToolTip || originalLength > charsInToolTip)
			{
				snippetToolTip = snippetToolTip.Mid(0, charsInToolTip - 4);
				snippetToolTip.Append(_T(" ..."));
			}

			event.SetToolTip(snippetToolTip);
		}
	}
	#endif
}
// ----------------------------------------------------------------------------
bool CodeSnippetsWindow::AddTextToClipBoard(const wxString& text)
// ----------------------------------------------------------------------------
{
    if ( not wxTheClipboard->Open() ) {
        wxLogError( GetConfig()->AppName + _T(":Can't open clipboard."));
        return false;
    }
    wxTheClipboard->SetData(new wxTextDataObject(text));
    wxTheClipboard->Close();

    #ifdef LOGGING
     //LOGIT( wxT("AddTextToClipBoard:Text[%s]"), text.GetData() );
     LOGIT( wxT("AddTextToClipBoard()") );
    #endif //LOGGING
    return true;
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuProperties(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{                                                           //(pecan 2006/9/12)
    SetActiveMenuId( event.GetId() );

	// Examine the snippet using the associated item id
	wxTreeItemId itemID = GetAssociatedItemID();
	GetSnippetsTreeCtrl()->EditSnippetProperties( itemID );
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSettings(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    SetActiveMenuId( event.GetId() );
    wxString oldWindowState = GetConfig()->GetSettingsWindowState();
    SettingsDlg* pDlg = new SettingsDlg( this );
    pDlg->ShowModal();
    delete pDlg;
    if ( 0 != GetConfig()->GetSettingsWindowState().Cmp(oldWindowState) )
        GetConfig()->m_bWindowStateChanged = true;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::SetSnippetImage(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
    // set the item tree image
    if ( IsFileSnippet(itemId) )
        GetSnippetsTreeCtrl()->SetItemImage( itemId, TREE_IMAGE_SNIPPET_FILE);
    else
        GetSnippetsTreeCtrl()->SetItemImage( itemId, TREE_IMAGE_SNIPPET_TEXT);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::CheckForExternallyModifiedFiles()
// ----------------------------------------------------------------------------
{
    // when we know we've changed the file internally, finesse this

    if ( IsTreeBusy() ) return;
    if ( GetFileChanged() ) return;

    wxString whichApp = wxT("CodeSnippets Plugin ");
    if (GetConfig()->IsApplication()) whichApp = wxT("CodeSnippets Program ");

    if(m_isCheckingForExternallyModifiedFiles) // for some reason, a mutex locker does not work???
        return;
    m_isCheckingForExternallyModifiedFiles = true;

    //-bool reloadAll = false; // flag to stop bugging the user
    bool failedFiles = false;  // file failed to reload
    bool b_modified = false;

    // Was file deleted?
    if ( not wxFileExists(GetConfig()->SettingsSnippetsXmlFullPath) )
    {
        //        if( GetFileChanged() ) // Already set the flag
        //            return;;
        //        wxString msg;
        //        msg.Printf(_("%s \nhas been deleted, or is no longer available.\n"
        //                     "Do you wish to keep the file open?\n"
        //                     "Yes to keep the file, No to close it."), GetConfig()->SettingsSnippetsXmlFullPath.c_str());
        //        if (messageBox(msg, whichApp + _("File changed!"), wxICON_QUESTION | wxYES_NO) == wxYES)
        //            SetFileChanged(true);
        //        else
        //        {
        //        }
        m_isCheckingForExternallyModifiedFiles = false;
        return; //return when file does not exist
    }

    wxFileName fname( GetConfig()->SettingsSnippetsXmlFullPath );
    wxDateTime last = fname.GetModificationTime();

    //    ProjectFile* pf = ed->GetProjectFile();
    //
    //    //File changed from RO -> RW?
    //    if (ed->GetControl()->GetReadOnly() &&
    //            wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
    //    {
    //        b_modified = false;
    //        ed->GetControl()->SetReadOnly(false);
    //        if (pf)
    //            pf->SetFileState(fvsNormal);
    //    }
    //    //File changed from RW -> RO?
    //    if (!ed->GetControl()->GetReadOnly() &&
    //            !wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
    //    {
    //        b_modified = false;
    //        ed->GetControl()->SetReadOnly(true);
    //        if (pf)
    //            pf->SetFileState(fvsReadOnly);
    //    }

    //Was File content changed?
    if ( last.IsLaterThan(GetSnippetsTreeCtrl()->GetSavedFileModificationTime()) )
        b_modified = true;

    if (b_modified)
    {
        // modified; ask to reload
        int ret = -1;
        {
            wxString msg;
            msg.Printf(_("%s\n\nFile is modified outside the IDE...\nDo you want to reload it (you will lose any unsaved work)?"),
                       GetConfig()->SettingsSnippetsXmlFullPath.c_str());
            if (messageBox(msg, whichApp + _("needs to Reload file?!"), wxICON_QUESTION | wxYES_NO) == wxYES)
                ret = wxYES;
            else
                ret = wxNO;

        }
        if ( ret == wxYES )
        {
            if (not GetSnippetsTreeCtrl()->LoadItemsFromFile(GetConfig()->SettingsSnippetsXmlFullPath, m_AppendItemsFromFile))
                failedFiles = true;
                // File modification time is saved by LoadItemsFromFile
                //GetSnippetsTreeCtrl()->SaveFileModificationTime(last);
        }
        else if (ret == wxNO)
            GetSnippetsTreeCtrl()->SaveFileModificationTime();
    }

    if (failedFiles)
    {
        wxString msg;
        msg.Printf(_("Could not reload file:\n\n%s"), GetConfig()->SettingsSnippetsXmlFullPath.c_str() );
        messageBox(msg, whichApp +  _("Error"), wxICON_ERROR);
    }
    m_isCheckingForExternallyModifiedFiles = false;

} // end of CheckForExternallyModifiedFiles

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuEditSnippetAsFileLink(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Open Snippet as file from context menu
    SetActiveMenuId( event.GetId() );
    GetSnippetsTreeCtrl()->EditSnippetAsFileLink( );
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSaveSnippetAsFileLink(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Open Snippet as file from context menu
    SetActiveMenuId( event.GetId() );

    GetSnippetsTreeCtrl()->SaveSnippetAsFileLink( );
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuAbout(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxString wxbuild(wxVERSION_STRING);

    #if defined(__WXMSW__)
        wxbuild << _T("-Windows");
    #elif defined(__UNIX__)
        wxbuild << _T("-Linux");
    #endif

    #if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
    #else
        wxbuild << _T("-ANSI build");
    #endif // wxUSE_UNICODE

    wxString buildInfo = wxbuild;
    wxString
        pgmVersionString = wxT("CodeSnippets v") + GetConfig()->GetVersion();
    buildInfo = wxT("\t")+pgmVersionString + wxT("\n")+ wxT("\t")+buildInfo;
    buildInfo = buildInfo + wxT("\n\n\t")+wxT("Original Code by Arto Jonsson");
    buildInfo = buildInfo + wxT("\n\t")+wxT("Modified/Enhanced by Pecan Heber");

    ShowSnippetsAbout( buildInfo);
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::ShowSnippetsAbout(wxString buildInfo)
// ----------------------------------------------------------------------------
{

    //wxString msg = wxbuildinfo(long_f);
    wxString helpText;
    helpText << wxT(" Each Snippet item may specify either text or a File Link.\n")
             << wxT("\n")
             << wxT(" Snippets may be edited from within the context menu \n")
             << wxT("\n")

             << wxT(" File Link snippets are created by dragging text to a new snippet, \n")
             << wxT(" then using the context menu to \"Convert to File Link\". \n")
             << wxT(" The data will be written to the specified file and the filename \n")
             << wxT(" will be placed in the snippets text area as a Link. \n")
             << wxT("\n")

             << wxT(" Snippets are accessed by using the context menu \"Edit\" \n")
             << wxT(" or via the Properties context menu entry. \n")
             << wxT("\n")

             << wxT(" Use the \"Settings\" menu to specify an external editor and \n")
             << wxT(" to specify a non-default Snippets index file. \n")
             << wxT("\n")

             << wxT(" Both the text and file snippets may be dragged outward\n")
             << wxT(" or copied to the clipboard.\n")
             << wxT("\n")

             << wxT(" Dragging a file snippet onto an external program window \n")
             << wxT(" will open the file. Dragging it into the edit area will \n")
             << wxT(" insert the text.\n");

    messageBox(wxT("\n\n")+buildInfo+wxT("\n\n")+helpText, _("About"),wxOK, wxSIMPLE_BORDER);

}

// ////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
bool SnippetsDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
// ----------------------------------------------------------------------------
{
    // This Drop routine refers to dragging external text onto a tree item

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
}

// ----------------------------------------------------------------------------
