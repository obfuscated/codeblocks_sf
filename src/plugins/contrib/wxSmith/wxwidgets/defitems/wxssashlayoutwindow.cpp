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

#include "wxssashlayoutwindow.h"
#include <wx/sashwin.h>
#include <wx/laywin.h>


//------------------------------------------------------------------------------

namespace
{

    wxsRegisterItem<wxsSashLayoutWindow> Reg(
        _T("SashLayoutWindow"),
        wxsTContainer,
        _T("Layout"),
        10,
        false);


    WXS_ST_BEGIN(wxsSashLayoutWindowStyles,_T("wxSW_3D|wxCLIP_CHILDREN"))
        WXS_ST_CATEGORY("wxSashLayoutWindow")
        WXS_ST(wxSW_3D)
        WXS_ST(wxSW_3DSASH)
        WXS_ST(wxSW_3DBORDER)
        WXS_ST(wxSW_BORDER)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSashLayoutWindowEvents)
        WXS_EVI(EVT_SASH_DRAGGED,      wxEVT_SASH_DRAGGED,      wxSashEvent, SashDragged)
        WXS_EVI(EVT_QUERY_LAYOUT_INFO, wxEVT_QUERY_LAYOUT_INFO, wxQueryLayoutInfoEvent, SashQueryLayout)
        WXS_EVI(EVT_CALCULATE_LAYOUT,  wxEVT_CALCULATE_LAYOUT,  wxCalculateLayoutEvent, SashCalculateLayout)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsSashLayoutWindow::wxsSashLayoutWindow(wxsItemResData* Data):
    wxsContainer(Data,&Reg.Info,wxsSashLayoutWindowEvents,wxsSashLayoutWindowStyles)
{
    mTop    = true;
    mBottom = true;
    mLeft   = true;
    mRight  = true;
    mAlign  = wxLAYOUT_LEFT;
    mOrient = wxLAYOUT_HORIZONTAL;
}

//------------------------------------------------------------------------------

wxObject* wxsSashLayoutWindow::OnBuildPreview(wxWindow* Parent,long Flags) {
wxSashLayoutWindow    *swin;

// make a thing to display

    swin = new wxSashLayoutWindow(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    SetupWindow(swin, Flags);

// for now, a sash on all edges

    swin->SetSashVisible(wxSASH_TOP,    mTop);
    swin->SetSashVisible(wxSASH_BOTTOM, mBottom);
    swin->SetSashVisible(wxSASH_LEFT,   mLeft);
    swin->SetSashVisible(wxSASH_RIGHT,  mRight);

// set the alignment

    if      (mAlign == wxLAYOUT_TOP)    swin->SetAlignment(wxLAYOUT_TOP);
    else if (mAlign == wxLAYOUT_BOTTOM) swin->SetAlignment(wxLAYOUT_BOTTOM);
    else if (mAlign == wxLAYOUT_LEFT)   swin->SetAlignment(wxLAYOUT_LEFT);
    else if (mAlign == wxLAYOUT_RIGHT)  swin->SetAlignment(wxLAYOUT_RIGHT);

// orientation

    if (mOrient == wxLAYOUT_HORIZONTAL) swin->SetOrientation(wxLAYOUT_HORIZONTAL);
    else                                swin->SetOrientation(wxLAYOUT_VERTICAL);

// don't forget the kids

    AddChildrenPreview(swin, Flags);

// done

    return swin;
}

//------------------------------------------------------------------------------

void wxsSashLayoutWindow::OnBuildCreatingCode()
{
wxString    vname;

    if (GetLanguage() == wxsCPP) {
        AddHeader(_T("<wx/sashwin.h>"),GetInfo().ClassName, 0);
        AddHeader(_T("<wx/laywin.h>"), GetInfo().ClassName, 0);
        Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
        BuildSetupWindowCode();
        AddChildrenCode();

        vname = GetVarName();
        if (mTop)    Codef(_T("%s->SetSashVisible(wxSASH_TOP,     true);\n"), vname.c_str());
        else         Codef(_T("%s->SetSashVisible(wxSASH_TOP,    false);\n"), vname.c_str());

        if (mBottom) Codef(_T("%s->SetSashVisible(wxSASH_BOTTOM,  true);\n"), vname.c_str());
        else         Codef(_T("%s->SetSashVisible(wxSASH_BOTTOM, false);\n"), vname.c_str());

        if (mLeft)   Codef(_T("%s->SetSashVisible(wxSASH_LEFT,    true);\n"), vname.c_str());
        else         Codef(_T("%s->SetSashVisible(wxSASH_LEFT,   false);\n"), vname.c_str());

        if (mRight)  Codef(_T("%s->SetSashVisible(wxSASH_RIGHT,   true);\n"), vname.c_str());
        else         Codef(_T("%s->SetSashVisible(wxSASH_RIGHT,  false);\n"), vname.c_str());

        if      (mAlign == wxLAYOUT_TOP)    Codef(_T("%s->SetAlignment(wxLAYOUT_TOP);\n"),    vname.c_str());
        else if (mAlign == wxLAYOUT_BOTTOM) Codef(_T("%s->SetAlignment(wxLAYOUT_BOTTOM);\n"), vname.c_str());
        else if (mAlign == wxLAYOUT_LEFT)   Codef(_T("%s->SetAlignment(wxLAYOUT_LEFT);\n"),   vname.c_str());
        else if (mAlign == wxLAYOUT_RIGHT)  Codef(_T("%s->SetAlignment(wxLAYOUT_RIGHT);\n"),  vname.c_str());

        if (mOrient == wxLAYOUT_HORIZONTAL) Codef(_T("%s->SetOrientation(wxLAYOUT_HORIZONTAL);\n"), vname.c_str());
        else                                Codef(_T("%s->SetOrientation(wxLAYOUT_VERTICAL);\n"),   vname.c_str());

    }
    else {
        wxsCodeMarks::Unknown(_T("wxsSashLayoutWindow::OnBuildCreatingCode"),GetLanguage());
    };
}

//------------------------------------------------------------------------------

void wxsSashLayoutWindow::OnEnumContainerProperties(long Flags) {
static const long    valign[] = {    wxLAYOUT_TOP,       wxLAYOUT_LEFT,       wxLAYOUT_RIGHT,       wxLAYOUT_BOTTOM,   0};
static const wxChar *nalign[] = {_T("wxLAYOUT_TOP"), _T("wxLAYOUT_LEFT"), _T("wxLAYOUT_RIGHT"), _T("wxLAYOUT_BOTTOM"), 0};

static const long    vorient[] = {    wxLAYOUT_HORIZONTAL,       wxLAYOUT_VERTICAL,   0};
static const wxChar *norient[] = {_T("wxLAYOUT_HORIZONTAL"), _T("wxLAYOUT_VERTICAL"), 0};


    WXS_BOOL(wxsSashLayoutWindow, mTop,    _("Drag Top"),    _("dragtop"),    true);
    WXS_BOOL(wxsSashLayoutWindow, mBottom, _("Drag Bottom"), _("dragbottom"), true);
    WXS_BOOL(wxsSashLayoutWindow, mLeft,   _("Drag Left"),   _("dragleft"),   true);
    WXS_BOOL(wxsSashLayoutWindow, mRight,  _("Drag Right"),  _("dragright"),  true);

    WXS_ENUM(wxsSashLayoutWindow, mAlign,  _("Alignment"),   _T("alignment"),   valign,  nalign,  wxLAYOUT_LEFT);
    WXS_ENUM(wxsSashLayoutWindow, mOrient, _("Orientation"), _T("rrientation"), vorient, norient, wxLAYOUT_HORIZONTAL);

}

//------------------------------------------------------------------------------

bool wxsSashLayoutWindow::OnCanAddChild(wxsItem* Item,bool ShowMessage) {
    // TODO: Allow more tools


    return true;
}
