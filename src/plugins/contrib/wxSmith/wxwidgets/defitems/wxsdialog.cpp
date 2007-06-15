/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
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

#include "wxsdialog.h"

namespace
{
    wxsRegisterItem<wxsDialog> Reg( _T("Dialog"), wxsTContainer, _T(""), 0 );

    WXS_ST_BEGIN(wxsDialogStyles,_T("wxDEFAULT_DIALOG_STYLE"))
        WXS_ST_CATEGORY("wxDialog")
        WXS_ST(wxSTAY_ON_TOP)
        WXS_ST(wxCAPTION)
        WXS_ST(wxDEFAULT_DIALOG_STYLE)
     //   WXS_ST(wxTHICK_FRAME)// replaced by wxRESIZE_BORDER in 2.6
        WXS_ST(wxSYSTEM_MENU)
        WXS_ST(wxRESIZE_BORDER)
     //   WXS_ST(wxRESIZE_BOX) // replaced by wxMAXIMIZE_BOX in 2.6
        WXS_ST(wxCLOSE_BOX)
     //   WXS_ST(wxDIALOG_MODAL)    // removed in 2.6
     //   WXS_ST(wxDIALOG_MODELESS) // removed in 2.6
        WXS_ST(wxDIALOG_NO_PARENT)
        WXS_ST(wxTAB_TRAVERSAL)
        WXS_ST(wxDIALOG_EX_METAL)
        WXS_ST(wxMAXIMIZE_BOX)
        WXS_ST(wxMINIMIZE_BOX)
        WXS_ST(wxFRAME_SHAPED)
    // NOTE (cyberkoa#1#): wxNO_3D & wxDIALOG_EX_CONTEXTHELP is only available
    // on WXMSW wxDIALOG_EX_METAL only for WXMAC
    // NOTE (cyberkoa#1#):  wxDIALOG_EX_CONTEXTHELP & wxDIALOG_EX_METAL are
    // extended style which need 2 steps construction
     //   WXS_ST_MASK(wxNO_3D,wxsSFWin,0,true) // removed in 2.6
        WXS_EXST_MASK(wxWS_EX_VALIDATE_RECURSIVELY, wxsSFAll, 0, true);
        WXS_EXST_MASK(wxDIALOG_EX_CONTEXTHELP,wxsSFWin,0,true)
        WXS_EXST_MASK(wxDIALOG_EX_METAL,wxsSFOSX,0,true)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsDialogEvents)
        WXS_EVI(EVT_INIT_DIALOG,wxEVT_INIT_DIALOG,wxInitDialogEvent,Init)
        WXS_EVI(EVT_CLOSE,wxEVT_CLOSE_WINDOW,wxCloseEvent,Close)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsDialog::wxsDialog(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsDialogEvents,
        wxsDialogStyles),
    Title(_("Dialog")),
    Centered(true)
{}

void wxsDialog::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%t,wxDefaultPosition,wxDefaultSize,%T,%N);\n"),Title.c_str());
            if ( !GetBaseProps()->m_Size.IsDefault )
            {
                Codef(_T("%ASetClientSize(%S);\n"));
            }
            if ( !GetBaseProps()->m_Position.IsDefault )
            {
                Codef(_T("%AMove(%P);\n"));
            }
            SetupWindowCode(Code,WindowParent,Language);
            AddChildrenCode(Code,wxsCPP);
            if ( Centered )
            {
                Code << Codef(Language,_T("%ACenter();\n"));
            }

            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDialog::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsDialog::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxWindow* NewItem = NULL;
    wxDialog* Dlg = NULL;

    // In case of frame and dialog when in "Exact" mode, we do not create
    // new object, but use Parent and call Create for it.
    if ( Flags & pfExact )
    {
        Dlg = wxDynamicCast(Parent,wxDialog);
        if ( Dlg )
        {
            Dlg->Create(NULL,GetId(),Title,wxDefaultPosition,wxDefaultSize,Style());
            Dlg->SetClientSize(Size(wxTheApp->GetTopWindow()));
            Dlg->Move(Pos(wxTheApp->GetTopWindow()));
        }
        NewItem = Dlg;
        SetupWindow(NewItem,Flags);
        AddChildrenPreview(NewItem,Flags);
        if ( Centered )
        {
            Dlg->Centre();
        }
    }
    else
    {
        // TODO: Use grid-viewing panel
        NewItem = new wxPanel(Parent,GetId(),wxPoint(0,0),Size(Parent),0);
        NewItem->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE ));
        SetupWindow(NewItem,Flags);
        AddChildrenPreview(NewItem,Flags);

        // wxPanel tends to behave very strange when it has children and no sizer,
        // we have to manually resize it's content
        if ( !GetChildCount() || GetChild(0)->GetType()!=wxsTSizer )
        {
            wxSize NewSize = Size(Parent);
            if ( !NewSize.IsFullySpecified() )
            {
                NewSize.SetDefaults(wxSize(400,450));
                NewItem->SetSize(NewSize);
                #if wxCHECK_VERSION(2,8,0)
                    NewItem->SetInitialSize(NewSize);
                #else
                    NewItem->SetBestFittingSize(NewSize);
                #endif
                if ( GetChildCount() == 1 )
                {
                    // If there's only one child it's size gets dialog's size
                    wxWindow* ChildPreview = wxDynamicCast(GetChild(0)->GetLastPreview(),wxWindow);
                    if ( ChildPreview )
                    {
                        ChildPreview->SetSize(0,0,NewItem->GetClientSize().GetWidth(),NewItem->GetClientSize().GetHeight());
                    }
                }
            }
            else
            {
                NewItem->SetSize(NewSize);
                #if wxCHECK_VERSION(2,8,0)
                    NewItem->SetInitialSize(NewSize);
                #else
                    NewItem->SetBestFittingSize(NewSize);
                #endif
            }
        }
    }

    return NewItem;
}

void wxsDialog::OnEnumContainerProperties(long Flags)
{
    WXS_SHORT_STRING(wxsDialog,Title,_("Title"),_T("title"),_T(""),false)
    WXS_BOOL(wxsDialog,Centered,_("Centered"),_T("centered"),false);
}

void wxsDialog::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/dialog.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsDialog::OnEnumDeclFiles"),Language);
    }
}
