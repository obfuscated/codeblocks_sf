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
        WXS_ST(wxTHICK_FRAME)
        WXS_ST(wxSYSTEM_MENU)
        WXS_ST(wxRESIZE_BORDER)
        WXS_ST(wxRESIZE_BOX)
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
        WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
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
            Code << Codef(Language,_T("%C(%W,%I,%t,%P,%S,%T,%N);\n"),Title.c_str());
            SetupWindowCode(Code,Language);
            // TODO: Setup Icon

            AddChildrenCode(Code,wxsCPP);
            if ( Centered )
            {
                Code << Codef(Language,_T("%ACenter();\n"));
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
    wxWindow* NewItem = NULL;
    wxFrame* Frm = NULL;

    // In case of frame and dialog when in "Exact" mode, we do not create
    // new object, but use Parent and call Create for it.
    if ( Flags & pfExact )
    {
        Frm = wxDynamicCast(Parent,wxFrame);
        if ( Frm )
        {
            Frm->Create(NULL,GetId(),Title,Pos(wxTheApp->GetTopWindow()),Size(wxTheApp->GetTopWindow()),Style());
        }
        NewItem = Frm;
    }
    else
    {
        // In preview we simulate dialog using panel
        // TODO: Use grid-viewing panel
        NewItem = new wxPanel(Parent,GetId(),wxDefaultPosition,wxDefaultSize,0/*wxRAISED_BORDER*/);
    }

    SetupWindow(NewItem,Flags);
    AddChildrenPreview(NewItem,Flags);

    if ( Frm && Centered )
    {
        Frm->Centre();
    }

    return NewItem;
}

void wxsFrame::OnEnumContainerProperties(long Flags)
{
    WXS_STRING(wxsFrame,Title,0,_("Title"),_T("title"),_T(""),false,false)
    WXS_BOOL  (wxsFrame,Centered,0,_("Centered"),_T("centered"),false);
    WXS_ICON  (wxsFrame,Icon,0,_T("Icon"),_T("icon"),_T("wxART_FRAME_ICON"));
}


void wxsFrame::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/frame.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsFrame::OnEnumDeclFiles"),Language);
    }
}
