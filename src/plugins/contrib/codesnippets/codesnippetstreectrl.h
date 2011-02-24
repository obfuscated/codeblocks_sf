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
// RCS-ID: $Id: codesnippetstreectrl.h 112 2008-01-07 17:03:31Z Pecan $

#ifndef CODESNIPPETSTREECTRL_H
#define CODESNIPPETSTREECTRL_H

class TiXmlNode;
class TiXmlElement;

#include <wx/treectrl.h>
#include <wx/dynarray.h>
#include "wx/mimetype.h"

#include "snippetitemdata.h"
#include <tinyxml/tinyxml.h>
#include "snippetproperty.h"
#include "codesnippetsevent.h"
#include "snippetsconfig.h"

class EditSnippetFrame;

WX_DEFINE_ARRAY(wxScrollingDialog*, DlgPtrArray);
//-WX_DEFINE_ARRAY(int, DlgRetcodeArray); //(stahta01 2007/4/21 for wxGTK2.8)
WX_DEFINE_ARRAY_INT(int, DlgRetcodeArray);
// ----------------------------------------------------------------------------
class CodeSnippetsTreeCtrl : public wxTreeCtrl
// ----------------------------------------------------------------------------
{
	public:
		CodeSnippetsTreeCtrl() { }
		CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
		~CodeSnippetsTreeCtrl();

		int         OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
		void        SaveItemsToFile(const wxString& fileName);
		bool        LoadItemsFromFile(const wxString& fileName, bool bAppendItems);
		void        LoadItemsFromXmlNode(const TiXmlElement* node, const wxTreeItemId& parentID);
		void        SaveItemsToXmlNode(TiXmlNode* node, const wxTreeItemId& parentID);
		void        AddCodeSnippet(const wxTreeItemId& parent, wxString title, wxString codeSnippet, long ID, bool editNow);
		wxTreeItemId AddCategory(const wxTreeItemId& parent, wxString title, long ID, bool editNow);
        bool        RemoveItem(const wxTreeItemId itemId);

        bool        SetFileChanged(bool truefalse) { return m_fileChanged = truefalse; }
        bool        GetFileChanged() { return m_fileChanged; }
        void        FetchFileModificationTime(wxDateTime savedTime = time_t(0));
        wxDateTime  GetSavedFileModificationTime(){ return m_LastXmlModifiedTime;}
        wxTreeItemId ConvertSnippetToCategory(wxTreeItemId itemId);
        void        OnItemSelected(wxTreeEvent& event);
        void        OnItemRightSelected(wxTreeEvent& event);
        void        OnItemSelectChanging(wxTreeEvent& event);

        void        EditSnippetAsFileLink();
        void        SaveSnippetAsFileLink();
        void        EditSnippetAsText();
        void        EditSnippetWithMIME();

        void SaveDataAndCloseEditorFrame(EditSnippetFrame*);
        void SaveEditorsXmlData(EditSnippetFrame* pEdFrame);

        // This OnIdle() is driven from the plugin|app OnIdle routines
        void OnIdle();


        CodeSnippetsTreeCtrl*
                    GetSnippetsTreeCtrl(){return m_pSnippetsTreeCtrl;}

        TiXmlDocument*  CopyTreeNodeToXmlDoc(wxTreeItemId TreeItemId = (void*)0 );
        void            CopySnippetsToXmlDoc(TiXmlNode* Node, const wxTreeItemId& itemID);
        void            CopyXmlDocToTreeNode(TiXmlDocument* pTiXmlDoc, wxTreeItemId targetItem  );

        wxString GetSnippet()
            {   wxString itemData = wxEmptyString;
                wxTreeItemId itemID = GetSelection();
                if (not itemID.IsOk()) return itemData;
                SnippetItemData* pItem = (SnippetItemData*)(GetItemData(itemID));
                itemData = pItem->GetSnippet();
                return itemData;
            }
        wxString GetSnippet( wxTreeItemId itemId )
            {   wxString itemData = wxEmptyString;
                if (not itemId.IsOk()) return itemData;
                SnippetItemData* pItem = (SnippetItemData*)(GetItemData(itemId));
                itemData = pItem->GetSnippet();
                return itemData;
            }

        long GetSnippetID( wxTreeItemId itemId )
            {   wxString itemData = wxEmptyString;
                if (not itemId.IsOk()) return 0;
                SnippetItemData* pItem = (SnippetItemData*)(GetItemData(itemId));
                return pItem->GetID();
            }

        wxString GetSnippetLabel(wxTreeItemId treeItemId=(void*)0)
            {   wxTreeItemId itemId = treeItemId;
                if (not itemId.IsOk()) itemId = GetSelection();
                if (not itemId.IsOk()) return wxEmptyString;
                return GetItemText(itemId);
            }

        void SetSnippet( wxString text )
            {   wxTreeItemId itemID = GetSelection();
                if (not itemID.IsOk()) return;
                SnippetItemData* pItem = (SnippetItemData*)(GetItemData(itemID));
                pItem->SetSnippet( text);
                SetFileChanged(true);
                return;
            }
        wxString GetSnippetFileLink(wxTreeItemId treeItemId = (void*)0 )
            {   wxTreeItemId itemId = treeItemId;
                if ( itemId == (void*)0) itemId = GetSelection();
                if (not itemId.IsOk()) return wxEmptyString;
                if (not IsSnippet(itemId) ) return wxEmptyString;
                wxString fileName = GetSnippet(itemId).BeforeFirst('\r');
                fileName = fileName.BeforeFirst('\n');
                //-#if defined(BUILDING_PLUGIN)
                static const wxString delim(_T("$%["));
                if( fileName.find_first_of(delim) != wxString::npos )
                    Manager::Get()->GetMacrosManager()->ReplaceMacros(fileName);
                //-#endif
                return fileName;
            }


        bool IsCategory(wxTreeItemId treeItemId = (void*)0)
            {   wxTreeItemId itemId = treeItemId;
                if (itemId == (void*)0) itemId = GetSelection();
                if (not itemId.IsOk()) return false;
                SnippetItemData* pItem = (SnippetItemData*)(GetItemData(itemId));
                return pItem->IsCategory();
            }
        bool IsSnippet(wxTreeItemId treeItemId = (void*)0)
            {   wxTreeItemId itemId = treeItemId;
                if (itemId == (void*)0) itemId = GetSelection();
                if (not itemId.IsOk()) return false;
                SnippetItemData* pItem = (SnippetItemData*)(GetItemData(itemId));
                return pItem->IsSnippet();
            }
        bool IsFileSnippet (wxTreeItemId treeItemId = (void*)0 );
        bool IsUrlSnippet (wxTreeItemId treeItemId = (void*)0 )
            {   wxTreeItemId itemId = treeItemId;
                if ( itemId == (void*)0) itemId = GetSelection();
                if (not itemId.IsOk()) return false;
                if (not IsSnippet(itemId) ) return false;
                wxString fileName = GetSnippet(itemId).BeforeFirst('\r');
                fileName = fileName.BeforeFirst('\n');
                if ( not fileName.StartsWith(wxT("http://")) ) return false;
                return true;
            }
        bool     IsFileLinkSnippet (wxTreeItemId treeItemId  );
        wxString GetFileLinkExt (wxTreeItemId treeItemId  );

        bool IsTreeBusy(){return (m_pPropertiesDialog != 0);}

        void            SetSnippetImage(wxTreeItemId itemId);
        wxTreeItemId    GetAssociatedItemID(){return m_MnuAssociatedItemID;}
        void            SetAssociatedItemID(wxTreeItemId id){m_MnuAssociatedItemID = id;}
        bool            EditSnippetProperties(wxTreeItemId& itemId);
        void            OpenSnippetAsFileLink();
        int             ExecuteDialog(wxScrollingDialog* pdlg, wxSemaphore& waitSem);

        wxTreeItemId FindTreeItemByLabel(const wxString& searchTerms, const wxTreeItemId& node, int requestType);
        wxTreeItemId FindTreeItemByTreeId(const wxTreeItemId& itemToFind, const wxTreeItemId& startNode, int itemToFindType);
        wxTreeItemId FindTreeItemBySnippetId(const SnippetItemID& IDToFind, const wxTreeItemId& startNode);
        wxTreeItemId ResetSnippetsIDs(const wxTreeItemId& startNode);
        wxTreeItemId FillFileLinksMapArray(const wxTreeItemId& startNode, FileLinksMapArray& fileLinksMapArray);

        // CodeSnippet/ThreadSearch events
        void OnCodeSnippetsEvent_Select(CodeSnippetsEvent& event);
        void OnCodeSnippetsEvent_Edit(CodeSnippetsEvent& event);
        void OnCodeSnippetsEvent_GetFileLinks(CodeSnippetsEvent& event);

	private:

	    bool                    m_fileChanged;
   		wxDateTime              m_LastXmlModifiedTime;
		bool                    m_bBeginInternalDrag;
        wxTreeItemId            m_TreeItemId;
        wxPoint                 m_TreeMousePosn;
        wxString                m_TreeText;

        bool                    m_bMouseCtrlKeyDown;
        bool                    m_bMouseLeftKeyDown;
        bool                    m_bMouseIsDragging;
        int                     m_MouseDownX, m_MouseDownY;
        int                     m_MouseUpX, m_MouseUpY;
        wxTreeItemId            m_itemAtKeyUp, m_itemAtKeyDown;

        bool                    m_bDragCursorOn;
        wxCursor*               m_pDragCursor;
        wxCursor                m_oldCursor;

   		wxTreeItemId            m_MnuAssociatedItemID;
   		bool                    m_bMouseExitedWindow;
        wxScrollingDialog*      m_pPropertiesDialog;
   		CodeSnippetsTreeCtrl*   m_pSnippetsTreeCtrl;
   		// Snippet Window Parent could be floating wxAUI window or CodeBlocks.
   		wxWindow*               m_pSnippetWindowParent;
   		bool                    m_bShutDown;
   		DlgPtrArray             m_aEdFramePtrs;
   		DlgRetcodeArray         aEdFrameRetcodes;
   		wxMimeTypesManager*     m_mimeDatabase;

        void EditSnippet(SnippetItemData* pSnippetItemData, wxString fileName=wxEmptyString);

        void BeginInternalTreeItemDrag();
        void EndInternalTreeItemDrag();

        void OnBeginTreeItemDrag(wxTreeEvent& event);
        void OnEndTreeItemDrag(wxTreeEvent& event);
   		void OnLeaveWindow(wxMouseEvent& event);
   		void OnEnterWindow(wxMouseEvent& event);
        //void OnMouseMotionEvent(wxMouseEvent& event);
        void OnMouseWheelEvent(wxMouseEvent& event);
        //void OnMouseLeftDownEvent(wxMouseEvent& event);
        //void OnMouseLeftUpEvent(wxMouseEvent& event);
        void OnShutdown(wxCloseEvent& event);
        //-void OnIdle(wxIdleEvent& event);
        void CreateDirLevels(const wxString& pathNameIn);
        void FinishExternalDrag();
        void SendMouseLeftUp(const wxWindow* pWin, const int mouseX, const int mouseY);
        void MSW_MouseMove(int x, int y );

		// Must use this so overridden OnCompareItems() works on MSW,
		// see wxWidgets Samples -> TreeCtrl sample
		//
		// Yes, again, ugly way to solve wxWidgets' weirdness
		DECLARE_DYNAMIC_CLASS(CodeSnippetsTreeCtrl)
        DECLARE_EVENT_TABLE()
};



#endif // CODESNIPPETSTREECTRL_H
