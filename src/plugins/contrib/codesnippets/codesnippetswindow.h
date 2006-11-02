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

#ifndef CODESNIPPETSLIST_H
#define CODESNIPPETSLIST_H

#include <wx/dnd.h>
#include <wx/string.h>

#include "codesnippetstreectrl.h"

class wxTextCtrl;
class wxButton;
class wxTreeCtrl;
class wxCommandEvent;
class wxTreeEvent;


class CodeSnippetsWindow : public wxPanel
{
	// Ugly as hell but this how it needs to be done
	friend class SnippetsDropTarget;

	public:
		enum SearchScope
		{
			SCOPE_SNIPPETS,		// Searches only snippets
			SCOPE_CATEGORIES,	// Searches only categories
			SCOPE_BOTH			// Searches both snippets and categories
		};

		struct SearchConfiguration
		{
			bool caseSensitive;
			SearchScope scope;

			SearchConfiguration()
			{
				// Default settings
				caseSensitive = true;
				scope = SCOPE_BOTH;
			}
		};

		CodeSnippetsWindow();
		~CodeSnippetsWindow();
	private:
		void InitDialog();
		void ApplySnippet(const wxTreeItemId& itemID);
		wxTreeItemId SearchSnippet(const wxString& searchTerms, const wxTreeItemId& node);

		wxTextCtrl* m_SearchSnippetCtrl;
		wxButton* m_SearchCfgBtn;
		CodeSnippetsTreeCtrl* m_SnippetsTreeCtrl;
		wxTreeItemId m_MnuAssociatedItemID;
		wxImageList* m_SnippetsTreeImageList;
		bool m_AppendItemsFromFile;
		SearchConfiguration m_SearchConfig;

		void OnSearchCfg(wxCommandEvent& event);
		void OnSearch(wxCommandEvent& event);
		void OnItemActivated(wxTreeEvent& event);
		void OnItemMenu(wxTreeEvent& event);
		void OnBeginDrag(wxTreeEvent& event);
		void OnEndDrag(wxTreeEvent& event);
		void OnMnuAddSubCategory(wxCommandEvent& event);
		void OnMnuRemove(wxCommandEvent& event);
		void OnMnuAddSnippet(wxCommandEvent& event);
		void OnMnuApplySnippet(wxCommandEvent& event);
		void OnBeginLabelEdit(wxTreeEvent& event);
		void OnMnuLoadSnippetsFromFile(wxCommandEvent& event);
		void OnMnuSaveSnippetsToFile(wxCommandEvent& event);
		void OnEndLabelEdit(wxTreeEvent& event);
		void OnMnuRemoveAll(wxCommandEvent& event);
		void OnMnuCaseSensitive(wxCommandEvent& event);
		void OnMnuChangeScope(wxCommandEvent& event);
		void OnMnuClear(wxCommandEvent& event);
		void OnMnuCopyToClipboard(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};

class SnippetsDropTarget : public wxTextDropTarget
{
	public:
		SnippetsDropTarget(CodeSnippetsTreeCtrl* treeCtrl) : m_TreeCtrl(treeCtrl) {}
		~SnippetsDropTarget() {}
		bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	private:
		CodeSnippetsTreeCtrl* m_TreeCtrl;
};

#endif // CODESNIPPETSLIST_H
