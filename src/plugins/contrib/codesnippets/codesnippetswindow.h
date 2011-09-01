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

#ifndef CODESNIPPETSWINDOW_H
#define CODESNIPPETSWINDOW_H

#include <wx/dnd.h>
#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/datetime.h>
#include "scrollingdialog.h"

#include "snippetsconfig.h"
#include "codesnippetstreectrl.h"

class wxTextCtrl;
class wxButton;
class wxTreeCtrl;
class wxCommandEvent;
class wxTreeEvent;
class ThreadSearchFrame;
class CodeSnippetsEvent;

// ----------------------------------------------------------------------------
class CodeSnippetsWindow : public wxPanel
// ----------------------------------------------------------------------------
{
	// Ugly as hell but this how it needs to be done
	friend class SnippetsDropTarget;
	friend class CodeSnippetsAppFrame;
	friend class CodeSnippets;

	public:

		CodeSnippetsWindow(wxWindow* parent);
		~CodeSnippetsWindow();

        void SaveSnippetsToFile(const wxString& fileName);
        bool IsSnippet(wxTreeItemId item = (void*)0  )
            { return GetSnippetsTreeCtrl()->IsSnippet(item); }
        bool IsFileSnippet(wxTreeItemId itemId=(void*)0)
            { return GetSnippetsTreeCtrl()->IsFileSnippet(itemId); }
        bool IsUrlSnippet(wxTreeItemId itemId=(void*)0)
            { return GetSnippetsTreeCtrl()->IsUrlSnippet(itemId); }

        //Getter helper routines
        CodeSnippetsTreeCtrl*   GetSnippetsTreeCtrl(){ return m_SnippetsTreeCtrl ;}
        wxImageList*            GetSnipImageList(){ return GetConfig()->GetSnipImages()->GetSnipImageList();}

        bool GetFileChanged( )
            {return GetSnippetsTreeCtrl() && GetSnippetsTreeCtrl()->GetFileChanged();}
        bool SetFileChanged( bool truefalse )
            {return GetSnippetsTreeCtrl()->SetFileChanged(truefalse);}

        wxString GetSnippet() { return GetSnippetsTreeCtrl()->GetSnippet();}
        wxString GetSnippet( wxTreeItemId itemId ) { return GetSnippetsTreeCtrl()->GetSnippet(itemId);}
        wxTreeItemId GetAssociatedItemID(){return GetSnippetsTreeCtrl()->GetAssociatedItemID();}
        void CloseThreadSearchFrame();

        bool IsEditingLabel() {return m_bIsEditingLabel;}
        void IsEditingLabel( bool trueorfalse) { m_bIsEditingLabel = trueorfalse;}
        bool IsAppendingFile(){return m_AppendItemsFromFile;}

	private:
		void InitDlg();
		void ApplySnippet(const wxTreeItemId& itemID);
		void CheckForMacros(wxString& snippet);
		bool AddTextToClipBoard(const wxString& text);
        void SetSnippetImage(wxTreeItemId itemId);
        void CheckForExternallyModifiedFiles();
        void ShowSnippetsAbout(wxString buildInfo);
		wxTreeItemId SearchSnippet(const wxString& searchTerms, const wxTreeItemId& node);
		bool IsTreeBusy(){
		    if (not GetSnippetsTreeCtrl()) return true; //debugging
		    return GetSnippetsTreeCtrl()->IsTreeBusy();
        }//IsTreeBusy

		wxTextCtrl*             m_SearchSnippetCtrl;
		wxButton*               m_SearchCfgBtn;
		CodeSnippetsTreeCtrl*   m_SnippetsTreeCtrl;
		bool                    m_AppendItemsFromFile;
//-		SearchConfiguration     m_SearchConfig;
		bool                    m_isCheckingForExternallyModifiedFiles;
        TiXmlDocument*          pTiXmlDoc;
        bool                    m_bIsEditingLabel;

        //-Utils utils;

		void OnSearchCfg(wxCommandEvent& event);
		void OnSearch(wxCommandEvent& event);
		void OnItemActivated(wxTreeEvent& event);
		void OnItemMenu(wxTreeEvent& event);
		void OnBeginDrag(wxTreeEvent& event);
		void OnEndDrag(wxTreeEvent& event);
		void OnMnuAddSubCategory(wxCommandEvent& event);
		void OnMnuRemove(wxCommandEvent& event);
		void OnMnuRename(wxCommandEvent& event);
		void OnMnuConvertToCategory(wxCommandEvent& event);
		void OnMnuAddSnippet(wxCommandEvent& event);
		void OnMnuApplySnippet(wxCommandEvent& event);
		void OnBeginLabelEdit(wxTreeEvent& event);
		void OnMnuLoadSnippetsFromFile(wxCommandEvent& event);
		void OnMnuSaveSnippets(wxCommandEvent& event);
		void OnMnuSaveSnippetsAs(wxCommandEvent& event);
		void OnEndLabelEdit(wxTreeEvent& event);
		void OnMnuRemoveAll(wxCommandEvent& event);
		void OnMnuCaseSensitive(wxCommandEvent& event);
		void OnMnuChangeScope(wxCommandEvent& event);
		void OnMnuClear(wxCommandEvent& event);
		void OnMnuCopyToClipboard(wxCommandEvent& event);
		void OnMnuEditSnippet(wxCommandEvent& event);
		void OnItemGetToolTip(wxTreeEvent& event);
        void OnMnuProperties(wxCommandEvent& event);
        void OnMnuEditSnippetAsFileLink(wxCommandEvent& event);
        void OnMnuOpenFileLink(wxCommandEvent& event);
        void OnMnuSaveSnippetAsFileLink(wxCommandEvent& event);
        void OnMnuSettings(wxCommandEvent& event);
        void OnMnuAbout(wxCommandEvent& event);
        void OnMnuTest(wxCommandEvent& event);
        void OnShutdown(wxCloseEvent& event);
        void OnMnuCopy(wxCommandEvent& event);
        void OnMnuPaste(wxCommandEvent& event);
        void OnMnuFileBackup(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event); //never occurs
        void OnIdle(wxIdleEvent& event);
        void OnMnuSearchExtended(wxCommandEvent& event);
        void OnLeaveWindow (wxMouseEvent &event);
        void OnEnterWindow (wxMouseEvent &event);

        void OnCodeSnippetsNewIndex(CodeSnippetsEvent& event);

		DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
class SnippetsDropTarget : public wxTextDropTarget
// ----------------------------------------------------------------------------
{
	public:
		SnippetsDropTarget(CodeSnippetsTreeCtrl* treeCtrl) : m_TreeCtrl(treeCtrl) {}
		~SnippetsDropTarget() {}
		bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	private:
		CodeSnippetsTreeCtrl* m_TreeCtrl;
};

#endif // CODESNIPPETSWINDOW_H
