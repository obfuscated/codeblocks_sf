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

#include "wxsVector.h"
#include <wxwidgets/wxsitemresdata.h>

//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "images/vector16.xpm"
    #include "images/vector32.xpm"

    wxsRegisterItem<wxsVector> Reg(
        _T("mpVector"),                 // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("MathPlot"),                 // Category in palette
        70,                             // Priority in palette
        _T("Vector"),                   // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(vector32_xpm),         // 32x32 bitmap
        wxBitmap(vector16_xpm),         // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsVectorStyles,_T(""))
        WXS_ST_CATEGORY("mpVector")
        WXS_ST(wxST_NO_AUTORESIZE)
        WXS_ST(wxALIGN_LEFT)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST(wxALIGN_CENTRE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsVectorEvents)
    WXS_EV_END()

}

//------------------------------------------------------------------------------

wxsVector::wxsVector(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsVectorEvents,
        wxsVectorStyles)
{
    mLabel      = _("Vector");
    mAlign      = mpALIGN_NE;
    mContinuous = true;

    mXYData.Clear();
    mXYData.Add(_T("! Enter X and Y data here, as pairs of numbers separated by commas."));
    mXYData.Add(_T("! For example:"));
    mXYData.Add(_T("!   1.5, 2.3"));
    mXYData.Add(_T("!   100, 300"));
    mXYData.Add(_T("! Blank lines and lines starting with \"!\" will be ignored."));
    mXYData.Add(_T("! All white-space will be ignored."));
    mXYData.Add(_T(""));

}

//------------------------------------------------------------------------------
//
// need to set line color and style

void wxsVector::OnBuildCreatingCode() {
int         i,n;
wxString    vname;
wxString    pname;
wxString    cname;
wxString    fname;
wxString    xname;
wxString    yname;
wxString    dtext;
wxString    s;

// we only know C++ language

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsVector::OnBuildCreatingCode"),GetLanguage());

// usefull names

    vname = GetVarName();
    pname = GetParent()->GetVarName();
    cname = vname + _("_PEN");
    fname = vname + _("_FONT");
    xname = vname + _("_X");
    yname = vname + _("_Y");

// the header for mathplot

    AddHeader(_T("<mathplot.h>"),GetInfo().ClassName,hfInPCH);

// create the vector -- but not the setup code

    Codef(_T("%s = new mpFXYVector(_(\"%s\"), %d);\n"), vname.wx_str(), mLabel.wx_str(), mAlign);
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

// define the arrays

    dtext = _("std::vector<double>  ") + xname + _(";");
    AddDeclaration(dtext);
    dtext = _("std::vector<double>  ") + yname + _(";");
    AddDeclaration(dtext);

// assign the data

    ParseXY();
    n = mXs.GetCount();
    if (n > 0) {
        for(i=0; i<n; i++) {
            Codef(_T("%s.push_back(%s);   %s.push_back(%s);\n"), xname.wx_str(), mXs[i].wx_str(),
                                                                 yname.wx_str(), mYs[i].wx_str());
        };

        Codef(_T("%ASetData(%s, %s);\n"), xname.wx_str(), yname.wx_str());
    };

// draw as points or a continuous line

    Codef(_T("%ASetContinuity(%b);\n"), mContinuous);

// add to parent window -- should be a mpWindow

    if ((GetPropertiesFlags() & flHidden) && GetBaseProps()->m_Hidden) n = 0;        // do nothing
    else Codef(_T("%s->AddLayer(%s);\n"), pname.wx_str(), vname.wx_str());


}

//------------------------------------------------------------------------------
// parse the mXYData into strings and doubles

void    wxsVector::ParseXY(void) {
int         i,n;
int         j;
double      d;
wxString    s,t;

// clear old junk

    mXs.Clear();
    mYs.Clear();
    mXf.clear();
    mYf.clear();

// get each string and do something with it

    n = mXYData.GetCount();
    for(i=0; i<n; i++) {
        s = mXYData[i];

// skip blank lines and "!" comments

        s.Trim(true);
        s.Trim(false);
        if (s.IsEmpty()) continue;
        if (s[0] == '!') continue;

// get the first number

        j = s.Find(_T(","));
        if (j != wxNOT_FOUND) {
            t = s.Left(j);
            s.Remove(0, j+1);
        }
        else {
            t = s;
            s = _T("");
        };

        t.Trim(true);
        t.Trim(false);
        t.ToDouble(&d);
        mXs.Add(t);
        mXf.push_back(d);

// the second number

        t = s;
        t.Trim(true);
        t.Trim(false);
        t.ToDouble(&d);
        mYs.Add(t);
        mYf.push_back(d);
    };

// there is a problem with mpFXYVector getting the count of items wrong,
// so we need to dulicate the last item in each list again

    n = mXs.GetCount();
    if (n > 0) {
        s = mXs[n-1];
        mXs.Add(s);
        s = mYs[n-1];
        mYs.Add(s);

        d = mXf[n-1];
        mXf.push_back(d);
        d = mYf[n-1];
        mYf.push_back(d);
    };
}

//------------------------------------------------------------------------------

wxObject* wxsVector::OnBuildPreview(wxWindow* Parent, long Flags) {
int             n;
wxStaticText    *Preview;
mpFXYVector     *vec;
mpWindow        *mp;
wxPen           pen;
wxColour        cc;
wxFont          ff;
bool            hide;

// if parent is not an mpWindow, then exit out

    if (! Parent->IsKindOf(CLASSINFO(mpWindow))) return NULL;
    mp = (mpWindow *) Parent;

// hide this vector

    hide = ((Flags & pfExact) && (GetPropertiesFlags() & flHidden) && GetBaseProps()->m_Hidden);

// make the place-holder

    Preview = new wxStaticText(Parent, GetId(), mLabel, Pos(Parent), Size(Parent), (wxSUNKEN_BORDER|Style()));
    Preview->SetForegroundColour(wxColour(255,255,255));
    Preview->SetBackgroundColour(wxColour(0,0,128));
    SetupWindow(Preview,Flags);
    if (Flags & pfExact) Preview->Hide();

// the actual vector

    vec = new mpFXYVector(mLabel, mAlign);

// pen color

    cc = mPenColour.GetColour();
    if (cc.IsOk()) pen.SetColour(cc);
    vec->SetPen(pen);

// text font

    ff = mPenFont.BuildFont();
    vec->SetFont(ff);

// update the place-holder

    if (cc.IsOk()) Preview->SetBackgroundColour(cc);
    Preview->SetFont(ff);

// fill in the data

    ParseXY();
    n = mXs.GetCount();
    if (n > 0) {
        vec->SetData(mXf, mYf);
    };

// points or lines?

    vec->SetContinuity(mContinuous);

// and add layer to parent

    if (! hide) mp->AddLayer(vec);

// done

    return Preview;
}

//------------------------------------------------------------------------------
// declare the var as a simple wxPanel

void wxsVector::OnBuildDeclarationsCode() {

    if (GetLanguage() == wxsCPP) {
        AddDeclaration(_T("mpFXYVector   *") + GetVarName() + _T(";"));
    }
    else {
        wxsCodeMarks::Unknown(_T("wxsVector::OnBuildDeclarationsCode"),GetLanguage());
    };
}




//------------------------------------------------------------------------------

void wxsVector::OnEnumWidgetProperties(long Flags) {
static const long    AlignValues[] = {    mpALIGN_NE,       mpALIGN_NW,       mpALIGN_SW,       mpALIGN_SE,    0};
static const wxChar* AlignNames[]  = {_T("mpALIGN_NE"), _T("mpALIGN_NW"), _T("mpALIGN_SW"), _T("mpALIGN_SE"),  0};

    WXS_SHORT_STRING(wxsVector, mLabel,      _("Label Text"),      _("mLabelText"),  _("Label"), true);
    WXS_ENUM(        wxsVector, mAlign,      _("Label Alignment"), _("mAlign"),      AlignValues, AlignNames, mpALIGN_NE);
    WXS_BOOL(        wxsVector, mContinuous, _("Continuous Line"), _("mContinuous"), true);
    WXS_COLOUR(      wxsVector, mPenColour,  _("Pen Colour"),      _("mPenColour"));
    WXS_FONT(        wxsVector, mPenFont,    _("Pen Font"),        _("mPenFont"));
    WXS_ARRAYSTRING( wxsVector, mXYData,     _("X,Y Data"),        _("mXYData"),     _("nums"));

}
