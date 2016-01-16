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
*/


#include "wxsMathPlot.h"



//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "images/plot16.xpm"
    #include "images/plot32.xpm"

    wxsRegisterItem<wxsMathPlot> Reg(
        _T("mpWindow"),                 // Class name
        wxsTContainer,                  // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("MathPlot"),                 // Category in palette
        90,                             // Priority in palette
        _T("MathPlot"),                 // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(plot32_xpm),           // 32x32 bitmap
        wxBitmap(plot16_xpm),           // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsMathPlotStyles,_T("wxRAISED_BORDER|wxTAB_TRAVERSAL"))
        WXS_ST_CATEGORY("wxMathPlot")
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsMathPlotEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsMathPlot::wxsMathPlot(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsMathPlotEvents,
        wxsMathPlotStyles)
{

// these not used yet ... in the future

    mXScale = 1.0;
    mYScale = 1.0;

}

//------------------------------------------------------------------------------
// set axis color and font and line style

void wxsMathPlot::OnBuildCreatingCode() {
wxString    inc;
wxString    vname;
wxString    aname;
wxString    cname;

// we only know C++ language

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsMathPlot::OnBuildCreatingCode"),GetLanguage());

// who we are

    vname = GetVarName();

// include files

    AddHeader(_("<mathplot.h>"), GetInfo().ClassName, 0);

// create the panel

    Codef(_T("%s = new mpWindow(%W, %I, %P, %S, %T);\n"), vname.wx_str());
    BuildSetupWindowCode();

// add children

    AddChildrenCode();

// and do it all

    Codef(_T("%AUpdateAll();\n"));
    Codef(_T("%AFit();\n"));
}

//------------------------------------------------------------------------------

wxObject* wxsMathPlot::OnBuildPreview(wxWindow* Parent, long Flags) {
mpWindow        *mp;

// make a panel

    mp = new mpWindow(Parent, GetId(), Pos(Parent), Size(Parent), Style());
    if (mp == NULL) return NULL;
    SetupWindow(mp, Flags);

// add kids

    AddChildrenPreview(mp, Flags);

// and update the display

    mp->UpdateAll();
    mp->Fit();

// done

    return mp;

}

//------------------------------------------------------------------------------

void wxsMathPlot::OnEnumContainerProperties(long Flags) {


};


//------------------------------------------------------------------------------

bool wxsMathPlot::OnCanAddChild(wxsItem* Item, bool ShowMessage) {

/**
    b = (Item->GetClassName() == _("mpFXYVector")) ||
        (Item->GetClassName() == _("mpScaleX")) ||
        (Item->GetClassName() == _("mpScaleY")) ||
        (Item->GetClassName() == _("wxButton"));

    if ( !b) {
        if ( ShowMessage ) wxMessageBox(_("May only add MathPlot components to this surface"));
        return false;
    };
**/

    return true;
}



