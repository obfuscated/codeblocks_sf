/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2008 Ron Collins
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
*/

#include "wxssashwindow.h"
#include <wx/sashwin.h>

#include <prep.h>

//------------------------------------------------------------------------------

namespace
{
    wxsRegisterItem<wxsSashWindow> Reg(
        _T("SashWindow"),
        wxsTContainer,
        _T("Layout"),                   // Category in palette
        40,                             // Priority in palette
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsSashWindowStyles,_T("wxSW_3D|wxCLIP_CHILDREN"))
        WXS_ST_CATEGORY("wxSashWindow")
        WXS_ST(wxSW_3D)
        WXS_ST(wxSW_3DSASH)
        WXS_ST(wxSW_3DBORDER)
        WXS_ST(wxSW_BORDER)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSashWindowEvents)
        WXS_EVI(EVT_SASH_DRAGGED,wxEVT_SASH_DRAGGED,wxSashEvent,SashDragged)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsSashWindow::wxsSashWindow(wxsItemResData* Data):
    wxsContainer(Data,&Reg.Info,wxsSashWindowEvents,wxsSashWindowStyles)
{
    mTop    = true;
    mBottom = true;
    mLeft   = true;
    mRight  = true;
}

//------------------------------------------------------------------------------

wxObject* wxsSashWindow::OnBuildPreview(wxWindow* Parent,long Flags)
{
// make a thing to display

    wxSashWindow *swin = new wxSashWindow(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    SetupWindow(swin, Flags);

// for now, a sash on all edges

    swin->SetSashVisible(wxSASH_TOP,    mTop);
    swin->SetSashVisible(wxSASH_BOTTOM, mBottom);
    swin->SetSashVisible(wxSASH_LEFT,   mLeft);
    swin->SetSashVisible(wxSASH_RIGHT,  mRight);

// don't forget the kids

    AddChildrenPreview(swin, Flags);

// done

    return swin;
}

//------------------------------------------------------------------------------

void wxsSashWindow::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddHeader(_T("<wx/sashwin.h>"),GetInfo().ClassName, 0);

            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            AddChildrenCode();

            Codef( _T("%ASetSashVisible(wxSASH_TOP,    %b);\n"), mTop);
            Codef( _T("%ASetSashVisible(wxSASH_BOTTOM, %b);\n"), mBottom);
            Codef( _T("%ASetSashVisible(wxSASH_LEFT,   %b);\n"), mLeft);
            Codef( _T("%ASetSashVisible(wxSASH_RIGHT,  %b);\n"), mRight);

            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsSashWindow::OnBuildCreatingCode"),GetLanguage());
    }
}

//------------------------------------------------------------------------------

void wxsSashWindow::OnEnumContainerProperties(cb_unused long Flags)
{
    WXS_BOOL(wxsSashWindow, mTop,    _("Drag Top"),    _("dragtop"),    true);
    WXS_BOOL(wxsSashWindow, mBottom, _("Drag Bottom"), _("dragbottom"), true);
    WXS_BOOL(wxsSashWindow, mLeft,   _("Drag Left"),   _("dragleft"),   true);
    WXS_BOOL(wxsSashWindow, mRight,  _("Drag Right"),  _("dragright"),  true);
}

//------------------------------------------------------------------------------

bool wxsSashWindow::OnCanAddChild(cb_unused wxsItem* Item,cb_unused bool ShowMessage)
{
    return true;
}
