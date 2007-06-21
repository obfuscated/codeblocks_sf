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

#include "wxsframe.h"

namespace
{
    wxsRegisterItem<wxsFrame> Reg( _T("Frame"), wxsTContainer, _T(""), 0 );

    WXS_ST_BEGIN(wxsFrameStyles,_T("wxDEFAULT_FRAME_STYLE"))
        WXS_ST_CATEGORY("wxFrame")
        WXS_ST(wxSTAY_ON_TOP)
        WXS_ST(wxCAPTION)
        WXS_ST(wxDEFAULT_DIALOG_STYLE)
        WXS_ST(wxDEFAULT_FRAME_STYLE)
     //   WXS_ST(wxTHICK_FRAME)// replaced by wxRESIZE_BORDER in 2.6
        WXS_ST(wxSYSTEM_MENU)
        WXS_ST(wxRESIZE_BORDER)
     //   WXS_ST(wxRESIZE_BOX) // replaced by wxMAXIMIZE_BOX in 2.6
        WXS_ST(wxCLOSE_BOX)

        WXS_ST(wxFRAME_NO_TASKBAR)
        WXS_ST(wxFRAME_SHAPED)
        WXS_ST(wxFRAME_TOOL_WINDOW)
        WXS_ST(wxFRAME_FLOAT_ON_PARENT)
        WXS_ST(wxMAXIMIZE_BOX)
        WXS_ST(wxMINIMIZE_BOX)
        WXS_ST(wxSTAY_ON_TOP)

     //   WXS_ST(wxNO_3D)
        WXS_ST(wxTAB_TRAVERSAL)
        WXS_EXST_MASK(wxWS_EX_VALIDATE_RECURSIVELY, wxsSFAll, 0, true);
        WXS_EXST_MASK(wxDIALOG_EX_CONTEXTHELP,wxsSFWin,0,true)
        WXS_EXST_MASK(wxDIALOG_EX_METAL,wxsSFOSX,0,true)

    // NOTE (cyberkoa##): wxMINIMIZE, wxMAXIMIZE are in the HELP file but not in XRC
    //#ifdef __WXMSW__
    // NOTE (cyberkoa##): There is a style wxICONIZE which is identical to wxMINIMIZE , not included.
    //    WXS_ST(wxMINIMIZE)
    //    WXS_ST(wxMAXIMIZE)
    //#endif

    WXS_ST_END()

    WXS_EV_BEGIN(wxsFrameEvents)
        WXS_EVI(EVT_CLOSE,wxEVT_CLOSE_WINDOW,wxCloseEvent,Close)
        WXS_EV_DEFAULTS()
    WXS_EV_END()

}

wxsFrame::wxsFrame(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsFrameEvents,
        wxsFrameStyles),
    Title(_("Frame")),
    Centered(false)
{}

void wxsFrame::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Codef(_T("%C(%W,%I,%t,wxDefaultPosition,wxDefaultSize,%T,%N);\n"),Title.c_str());
            if ( !GetBaseProps()->m_Size.IsDefault )
            {
                Codef(_T("%ASetClientSize(%S);\n"));
            }
            if ( !GetBaseProps()->m_Position.IsDefault )
            {
                Codef(_T("%AMove(%P);\n"));
            }
            SetupWindowCode(Code,WindowParent,Language);
            // TODO: Setup Icon

            AddChildrenCode(Code,wxsCPP);
            if ( Centered )
            {
                Codef(_T("%ACenter();\n"));
            }

            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFrame::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsFrame::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxWindow* NewItem = 0;
    wxFrame* Frm = 0;

    // In case of frame and dialog when in "Exact" mode, we do not create
    // new object, but use Parent and call Create for it.
    if ( Flags & pfExact )
    {
        Frm = wxDynamicCast(Parent,wxFrame);
        if ( Frm )
        {
            Frm->Create(0,GetId(),Title,wxDefaultPosition,wxDefaultSize,Style());
            Frm->SetClientSize(Size(wxTheApp->GetTopWindow()));
            Frm->Move(Pos(wxTheApp->GetTopWindow()));
        }
        NewItem = Frm;
        SetupWindow(NewItem,Flags);
        AddChildrenPreview(NewItem,Flags);
        if ( Centered )
        {
            Frm->Centre();
        }

    }
    else
    {
        // TODO: Use grid-viewing panel
        NewItem = new wxPanel(Parent,GetId(),wxDefaultPosition,Size(Parent),0);
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
            }
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
    }

    return NewItem;
}

void wxsFrame::OnEnumContainerProperties(long Flags)
{
    WXS_SHORT_STRING(wxsFrame,Title,_("Title"),_T("title"),_T(""),false)
    WXS_BOOL(wxsFrame,Centered,_("Centered"),_T("centered"),false);
    WXS_ICON(wxsFrame,Icon,_T("Icon"),_T("icon"),_T("wxART_FRAME_ICON"));
}


void wxsFrame::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/frame.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsFrame::OnEnumDeclFiles"),Language);
    }
}
