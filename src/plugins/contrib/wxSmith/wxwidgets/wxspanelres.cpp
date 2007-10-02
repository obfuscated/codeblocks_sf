/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxspanelres.h"
#include "wxsitemresdata.h"
#include "wxsflags.h"
#include <wx/button.h>

using namespace wxsFlags;

namespace
{
    class wxsPanelResPreview: public wxDialog
    {
        public:

            wxsPanelResPreview(wxWindow* Parent,wxsItemResData* Data): m_Data(Data)
            {
                Create(Parent,-1,_("Preview for wxPanel class"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
                wxObject* PreviewObj = m_Data->GetRootItem()->BuildPreview(this,pfExact);
                wxWindow* PreviewWnd = wxDynamicCast(PreviewObj,wxWindow);
                if ( !PreviewWnd )
                {
                    delete PreviewObj;
                }
                else
                {
                    wxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
                    Sizer->Add(PreviewWnd,0,wxEXPAND,0);
                    SetSizer(Sizer);
                    Sizer->Layout();
                    Sizer->SetSizeHints(this);
                }
                Center();
                wxAcceleratorEntry Acc[1];
                Acc[0].Set(wxACCEL_NORMAL,WXK_ESCAPE,wxID_EXIT);
                wxAcceleratorTable Table(1,Acc);
                SetAcceleratorTable(Table);
            }

            ~wxsPanelResPreview()
            {
                m_Data->NotifyPreviewClosed();
            }

            void OnEscape(wxCommandEvent& event)
            {
                Close();
            }

            void OnClose(wxCloseEvent& event)
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

    BEGIN_EVENT_TABLE(wxsPanelResPreview,wxDialog)
        EVT_MENU(wxID_EXIT,wxsPanelResPreview::OnEscape)
        EVT_CLOSE(wxsPanelResPreview::OnClose)
        EVT_BUTTON(wxID_ANY,wxsPanelResPreview::OnButton)
    END_EVENT_TABLE()
}

const wxString wxsPanelRes::ResType = _T("wxPanel");

wxWindow* wxsPanelRes::OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data)
{
    wxDialog* Dlg = new wxsPanelResPreview(Parent,Data);
    Dlg->Show();
    return Dlg;
}
