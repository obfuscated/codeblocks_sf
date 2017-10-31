/*
    This file is part of SnipList. A standalone adaptation of
    Code Snippets by Arto Jonsson. Parts Copyright (C) Pecan Heber

	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// ----------------------------------------------------------------------------
//  SnippetProperty.h                                           //(pecan 2006/9/12)
// ----------------------------------------------------------------------------
#ifndef SNIPPETPROPERTY_H
#define SNIPPETPROPERTY_H

#include <wx/treectrl.h>
#include "snippetpropertyform.h"

class wxScintilla;

#include "snippetitemdata.h"

// ----------------------------------------------------------------------------
class SnippetProperty : public SnippetPropertyForm
// ----------------------------------------------------------------------------
{
    friend class SnippetDropTarget;

    public:
        SnippetProperty(wxTreeCtrl* pTree, wxTreeItemId itemId, wxSemaphore* pWaitSem);
        virtual ~SnippetProperty();

    protected:
        wxPoint pos;
        void OnOk(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnSnippetButton(wxCommandEvent& event);
        void OnFileSelectButton(wxCommandEvent& event);
        //-void OnLeaveWindow(wxMouseEvent& event);

        void InvokeEditOnSnippetText();
        void InvokeEditOnSnippetFile();

        wxScintilla* GetSnippetEditCtrl(){return m_SnippetEditCtrl;}
        wxTreeCtrl*  GetSnippetsTreeCtrl(){return m_pTreeCtrl;}
        wxString     GetSnippetString(){return m_pSnippetDataItem->GetSnippetString();}
        bool         IsSnippetFile(){return m_pSnippetDataItem->IsSnippetFile();}

    private:
        void InitSnippetProperty(wxTreeCtrl* pTree, wxTreeItemId itemId, wxSemaphore* pWaitSem);

        wxTreeItemId        m_TreeItemId;
        SnippetTreeItemData*    m_pSnippetDataItem;
        wxTreeCtrl*         m_pTreeCtrl;
        int                 m_nScrollWidthMax;
        wxSemaphore*        m_pWaitingSemaphore;

    DECLARE_EVENT_TABLE()
};

#endif // SNIPPETPROPERTY_H
