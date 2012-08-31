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

#include "wxsAxis.h"
#include "../wxsitemresdata.h"


//------------------------------------------------------------------------------

namespace
{
//    wxsRegisterItem<wxsStaticText> Reg(_T("StaticText"),wxsTWidget,_T("Standard"),80);

    #include "axis16.xpm"
    #include "axis32.xpm"

    wxsRegisterItem<wxsAxis> Reg(
        _T("mpAxis"),                   // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("MathPlot"),                 // Category in palette
        80,                             // Priority in palette
        _T("Axis"),                     // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(axis32_xpm),           // 32x32 bitmap
        wxBitmap(axis16_xpm),           // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files

    WXS_ST_BEGIN(wxsAxisStyles,_T(""))
        WXS_ST_CATEGORY("mpAxis")
        WXS_ST(wxST_NO_AUTORESIZE)
        WXS_ST(wxALIGN_LEFT)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST(wxALIGN_CENTRE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsAxisEvents)
    WXS_EV_END()

}

//------------------------------------------------------------------------------

wxsAxis::wxsAxis(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsAxisEvents,
        wxsAxisStyles)
{
    mType  = 0;
    mLabel = _("XY");
    mAlign = mpALIGN_CENTER;
    mTics  = true;

}

//------------------------------------------------------------------------------

void wxsAxis::OnBuildCreatingCode() {
wxString    vname;
wxString    pname;
wxString    cname;
wxString    fname;
wxString    dtext;

// we only know C++ language

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsAxis::OnBuildCreatingCode"),GetLanguage());

// usefull names

    vname = GetVarName();
    pname = GetParent()->GetVarName();
    cname = vname + _("_PEN");
    fname = vname + _("_FONT");

// the header for mathplot

    AddHeader(_T("<mathplot.h>"),GetInfo().ClassName,hfInPCH);

// create the axis -- but not the setup code

    if (mType == 0) Codef(_T("%s = new mpScaleX(_(\"%s\"), %d, %b);\n"), vname.c_str(), mLabel.c_str(), mAlign, mTics);
    else            Codef(_T("%s = new mpScaleY(_(\"%s\"), %d, %b);\n"), vname.c_str(), mLabel.c_str(), mAlign, mTics);
//  BuildSetupWindowCode();

// assign a pen to the layer

    dtext = mPenColour.BuildCode(GetCoderContext());
    if (dtext.Len() > 0) {
        Codef(_T("wxPen   %s(%s);\n"), cname.c_str(), dtext.c_str());
        Codef(_T("%s->SetPen(%s);\n"), vname.c_str(), cname.c_str());
    };

// assign a font to the layer

    dtext = mPenFont.BuildFontCode(fname, GetCoderContext());
    if (dtext.Len() > 0) {
        Codef(_T("%s"), dtext.c_str());
        Codef(_T("%s->SetFont(%s);\n"), vname.c_str(), fname.c_str());
    };

// add to parent window -- should be a mpWindow

    if ((GetPropertiesFlags() & flHidden) && GetBaseProps()->m_Hidden)
        ; // do nothing
    else
        Codef(_T("%s->AddLayer(%s);\n"), pname.c_str(), vname.c_str());
}

//------------------------------------------------------------------------------

wxObject* wxsAxis::OnBuildPreview(wxWindow* Parent,long Flags) {
wxStaticText    *Preview;
mpWindow        *mp;
mpScaleX        *xx;
mpScaleY        *yy;
wxPen           pen;
wxColour        cc;
wxFont          ff;
bool            hide;

// if parent is not an mpWindow, then exit out

    if (! Parent->IsKindOf(CLASSINFO(mpWindow))) return NULL;
    mp = (mpWindow *) Parent;

// hide this axis?

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

// make the axis

    if (mType == 0) {
        xx = new mpScaleX(mLabel, mAlign, mTics);
        xx->SetPen(pen);
        xx->SetFont(ff);
        if (! hide) mp->AddLayer(xx);
    }
    else {
        yy = new mpScaleY(mLabel, mAlign, mTics);
        yy->SetPen(pen);
        yy->SetFont(ff);
        if (! hide) mp->AddLayer(yy);
    };

// done

    return Preview;
}

//------------------------------------------------------------------------------
// declare the var as a simple wxPanel

void wxsAxis::OnBuildDeclarationsCode() {

    if (GetLanguage() == wxsCPP) {
        if (mType == 0) AddDeclaration(_T("mpScaleX   *") + GetVarName() + _T(";"));
        else            AddDeclaration(_T("mpScaleY   *") + GetVarName() + _T(";"));
    }
    else {
        wxsCodeMarks::Unknown(_T("wxsAxis::OnBuildDeclarationsCode"),GetLanguage());
    };
}


//------------------------------------------------------------------------------

void wxsAxis::OnEnumWidgetProperties(long Flags) {
static const long    TypeValues[]  = {    0,            1,        0};
static const wxChar* TypeNames[]   = {_T("X-Axis"), _T("Y-Axis"), 0};
static const long    AlignValues[] = {    mpALIGN_BORDER_LEFT,       mpALIGN_BORDER_TOP,       mpALIGN_LEFT,       mpALIGN_TOP,       mpALIGN_CENTER,       mpALIGN_RIGHT,       mpALIGN_BOTTOM,       mpALIGN_BORDER_RIGHT,       mpALIGN_BORDER_BOTTOM,   0};
static const wxChar* AlignNames[]  = {_T("mpALIGN_BORDER_LEFT"), _T("mpALIGN_BORDER_TOP"), _T("mpALIGN_LEFT"), _T("mpALIGN_TOP"), _T("mpALIGN_CENTER"), _T("mpALIGN_RIGHT"), _T("mpALIGN_BOTTOM"), _T("mpALIGN_BORDER_RIGHT"), _T("mpALIGN_BORDER_BOTTOM"), 0};



    WXS_ENUM(  wxsAxis, mType,       _("Axis Type"),      _("mType"),   TypeValues,  TypeNames, 0);
    WXS_STRING(wxsAxis, mLabel,      _("Label"),          _T("mLabel"), _T("axis"),  true);
    WXS_ENUM(  wxsAxis, mAlign,      _("Axis Location"),  _("mAlign"),  AlignValues, AlignNames, mpALIGN_CENTER);
    WXS_BOOL(  wxsAxis, mTics,       _("Show Tic Marks"), _("mTics"),   true);
    WXS_COLOUR(wxsAxis, mPenColour,  _("Pen Colour"),     _("mPenColour"));
    WXS_FONT(  wxsAxis, mPenFont,    _("Pen Font"),       _("mPenFont"));

}
