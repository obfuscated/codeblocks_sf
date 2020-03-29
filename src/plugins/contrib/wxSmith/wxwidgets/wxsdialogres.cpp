/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsdialogres.h"
#include "wxsitemresdata.h"
#include "wxsflags.h"
#include <wx/button.h>

using namespace wxsFlags;

namespace
{
    class wxsDialogResPreview: public wxDialog
    {
        public:

            wxsDialogResPreview(cb_unused wxWindow* Parent,wxsItemResData* Data): m_Data(Data)
            {
                m_Data->GetRootItem()->BuildPreview(this,pfExact);
                wxAcceleratorEntry Acc[1];
                Acc[0].Set(wxACCEL_NORMAL,WXK_ESCAPE,wxID_EXIT);
                wxAcceleratorTable Table(1,Acc);
                SetAcceleratorTable(Table);
            }

            ~wxsDialogResPreview()
            {
                m_Data->NotifyPreviewClosed();
            }

            void OnEscape(cb_unused wxCommandEvent& event)
            {
                Close();
            }

            void OnClose(cb_unused wxCloseEvent& event)
            {
                Destroy();
            }

            void OnButton(wxCommandEvent& event)
            {
                wxWindowID Id = event.GetId();
                if ( Id == wxID_OK  ||
                     Id == wxID_APPLY ||
                     Id == wxID_CANCEL )
                {
                    Close();
                }
            }

            wxsItemResData* m_Data;

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(wxsDialogResPreview,wxDialog)
        EVT_BUTTON(wxID_ANY,wxsDialogResPreview::OnButton)
        EVT_MENU(wxID_EXIT,wxsDialogResPreview::OnEscape)
        EVT_CLOSE(wxsDialogResPreview::OnClose)
    END_EVENT_TABLE()
}

const wxString wxsDialogRes::ResType = _T("wxDialog");

wxString wxsDialogRes::OnGetAppBuildingCode()
{
    return wxString::Format(
        _T("\t%s Dlg(0);\n")
        _T("\tSetTopWindow(&Dlg);\n")
        _T("\tDlg.ShowModal();\n")
        _T("\twxsOK = false;\n"),
            GetResourceName().c_str());
}

wxWindow* wxsDialogRes::OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data)
{
    wxDialog* Dlg = new wxsDialogResPreview(Parent,Data);
    Dlg->Show();
    return Dlg;
}
