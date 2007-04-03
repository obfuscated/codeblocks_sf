/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

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
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <wx/treectrl.h>
#include "settingsdlgform.h"
#include "snippetitemdata.h"

// ----------------------------------------------------------------------------
class SettingsDlg : public SettingsDlgForm
// ----------------------------------------------------------------------------
{
    public:
        SettingsDlg(wxWindow* parent);
        virtual ~SettingsDlg();
    protected:
        wxPoint pos;
        void OnOk(wxCommandEvent& event);
        void OnExtEditorButton(wxCommandEvent& event);
        void OnSnippetFileButton(wxCommandEvent& event);

        void     OnSettingsSave(wxCommandEvent& event);
        void     GetFileName(wxString& newFileName);
        wxString AskForPathName();


    private:
        wxTreeItemId        m_TreeItemId;
        SnippetItemData*    m_pSnippetDataItem;
        wxTreeCtrl*         m_pTreeCtrl;


    DECLARE_EVENT_TABLE()
};

#endif // SETTINGSDLG_H
