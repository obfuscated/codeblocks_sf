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

#include "wxsMarker.h"
#include <wxwidgets/wxsitemresdata.h>

//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "images/marker16.xpm"
    #include "images/marker32.xpm"

    wxsRegisterItem<wxsMarker> Reg(
        _T("mpMarker"),                 // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("MathPlot"),                 // Category in palette
        60,                             // Priority in palette
        _T("Marker"),                   // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(marker32_xpm),         // 32x32 bitmap
        wxBitmap(marker16_xpm),         // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsMarkerStyles,_T(""))
        WXS_ST_CATEGORY("mpMarker")
        WXS_ST(wxST_NO_AUTORESIZE)
        WXS_ST(wxALIGN_LEFT)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST(wxALIGN_CENTRE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsMarkerEvents)
    WXS_EV_END()

}

//------------------------------------------------------------------------------

wxsMarker::wxsMarker(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsMarkerEvents,
        wxsMarkerStyles)
{
    mLabel = _("*");
    mXpos  = _("0.0");
    mYpos  = _("0.0");
}

//------------------------------------------------------------------------------
//
// need to set line color and style

void wxsMarker::OnBuildCreatingCode() {
wxString    vname;
wxString    pname;
wxString    cname;
wxString    fname;
wxString    xname;
wxString    yname;
wxString    dtext;
wxString    s;

// we only know C++ language

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsMarker::OnBuildCreatingCode"),GetLanguage());

// usefull names

    vname = GetVarName();
    pname = GetParent()->GetVarName();
    cname = vname + _("_PEN");
    fname = vname + _("_FONT");

// the header for mathplot

    AddHeader(_T("<mathplot.h>"),GetInfo().ClassName,hfInPCH);

// create the vector -- but not the setup code

    Codef(_T("%s = new mpMarker(%t, %s, %s);\n"), vname.wx_str(), mLabel.wx_str(), mXpos.wx_str(), mYpos.wx_str());
//  BuildSetupWindowCode();

// assign a pen to the layer

    dtext = mPenColour.BuildCode(GetCoderContext());
    if (dtext.Len() > 0) {
        Codef(_T("wxPen   %s(%s);\n"), cname.wx_str(), dtext.wx_str());
        Codef(_T("%s->SetPen(%s);\n"), vname.wx_str(), cname.wx_str());
    };

// assign a font to the layer

    dtext = mPenFont.BuildFontCode(fname, GetCoderContext());
    if (dtext.Len() > 0) {
        Codef(_T("%s"), dtext.wx_str());
        Codef(_T("%s->SetFont(%s);\n"), vname.wx_str(), fname.wx_str());
    };

// add to parent window -- should be a mpWindow

    if ((GetPropertiesFlags() & flHidden) && GetBaseProps()->m_Hidden)
        ; // do nothing
    else
        Codef(_T("%s->AddLayer(%s);\n"), pname.wx_str(), vname.wx_str());


}

//------------------------------------------------------------------------------

wxObject* wxsMarker::OnBuildPreview(wxWindow* Parent, long Flags) {
wxStaticText    *Preview;
mpMarker        *mk;
mpWindow        *mp;
wxPen           pen;
wxColour        cc;
wxFont          ff;
bool            hide;
double          xp, yp;

// if parent is not an mpWindow, then exit out

    if (! Parent->IsKindOf(CLASSINFO(mpWindow))) return NULL;
    mp = (mpWindow *) Parent;

// hide this marker

    hide = ((Flags & pfExact) && (GetPropertiesFlags() & flHidden) && GetBaseProps()->m_Hidden);

// make the place-holder

    Preview = new wxStaticText(Parent, GetId(), mLabel, Pos(Parent), Size(Parent), (wxSUNKEN_BORDER|Style()));
    Preview->SetForegroundColour(wxColour(255,255,255));
    Preview->SetBackgroundColour(wxColour(0,128,0));
    SetupWindow(Preview,Flags);
    if (Flags & pfExact) Preview->Hide();

// pen color

    cc = mPenColour.GetColour();
    if (cc.IsOk()) pen.SetColour(cc);

// text font

    ff = mPenFont.BuildFont();

// update the place-holder

    if (cc.IsOk()) Preview->SetBackgroundColour(cc);
    Preview->SetFont(ff);

// X & Y position

    if (! mXpos.ToDouble(&xp)) {
        xp = 0.0;
        mXpos = _("0.0");
    };
    if (! mYpos.ToDouble(&yp)) {
        yp = 0.0;
        mYpos = _("0.0");
    };

// the actual marker

    mk = new mpMarker(mLabel, xp, yp);
    mk->SetPen(pen);
    mk->SetFont(ff);

// and add layer to parent

    if (! hide) mp->AddLayer(mk);

// done

    return Preview;
}

//------------------------------------------------------------------------------
// declare the var as a simple wxPanel

void wxsMarker::OnBuildDeclarationsCode() {

    if (GetLanguage() == wxsCPP) {
        AddDeclaration(_T("mpMarker   *") + GetVarName() + _T(";"));
    }
    else {
        wxsCodeMarks::Unknown(_T("wxsMarker::OnBuildDeclarationsCode"),GetLanguage());
    };
}




//------------------------------------------------------------------------------

void wxsMarker::OnEnumWidgetProperties(long Flags) {

    WXS_SHORT_STRING(wxsMarker, mLabel,      _("Marker Text"),     _("mLabelText"),  _("*"), true);
    WXS_SHORT_STRING(wxsMarker, mXpos,       _("X Position"),      _("mXpos"),       _("0.0"), true);
    WXS_SHORT_STRING(wxsMarker, mYpos,       _("Y Position"),      _("mYpos"),       _("0.0"), true);
    WXS_COLOUR(      wxsMarker, mPenColour,  _("Pen Colour"),      _("mPenColour"));
    WXS_FONT(        wxsMarker, mPenFont,    _("Pen Font"),        _("mPenFont"));
}
