/*
* This file is part of a wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith and this file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* Ron Collins
* rcoll@theriver.com
* 4-Feb-2010
*
*/

#include "wxsSpeedButton.h"

//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "SpeedButton16.xpm"
    #include "SpeedButton32.xpm"

    wxsRegisterItem<wxsSpeedButton> Reg(
        _T("wxSpeedButton"),            // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("Contrib"),                  // Category in palette
        90,                             // Priority in palette
        _T("SpeedButton"),              // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(SpeedButton32_xpm),    // 32x32 bitmap
        wxBitmap(SpeedButton16_xpm),    // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files

    WXS_ST_BEGIN(wxsSpeedButtonStyles,_T("wxBORDER_NONE|wxTAB_TRAVERSAL"))
        WXS_ST_CATEGORY("wxSpeedButton")
        WXS_ST(wxBU_LEFT)
        WXS_ST(wxBU_TOP)
        WXS_ST(wxBU_RIGHT)
        WXS_ST(wxBU_BOTTOM)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSpeedButtonEvents)
        WXS_EVI(EVT_COMMAND_LEFT_CLICK,  wxEVT_COMMAND_LEFT_CLICK,  wxCommandEvent, LeftClick)
        WXS_EVI(EVT_COMMAND_RIGHT_CLICK, wxEVT_COMMAND_RIGHT_CLICK, wxCommandEvent, RightClick)
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsSpeedButton::wxsSpeedButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSpeedButtonEvents,
        wxsSpeedButtonStyles)
{

// start with a clean slate

    mLabel      = _("");
    mGlyphCount = 0;
    mMargin     = 2;
    mUseInclude = true;
    mGroupIndex = 0;
    mAllowAllUp = true;
    mUserData   = 0;
    mButtonType = 1;
    mButtonDown = false;

}

//------------------------------------------------------------------------------

void wxsSpeedButton::OnBuildCreatingCode() {
int         n;
wxString    inc;
wxString    vname;                  // name of this var
wxString    bname;                  // name of wxBitmap for the button
wxString    ss, tt;

// valid language?

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsSpeedButton::OnBuildCreatingCode"),GetLanguage());

// who we are

    vname = GetVarName();
    bname = vname + _("_BMP");

// include files

    AddHeader(_("<wxSpeedButton.h>"), GetInfo().ClassName, 0);

// create the bitmap used for button images

    BuildBitmap();

// fix the group index depending upon the button type

    if      (mButtonType == 0) n = 0;
    else if (mButtonType == 1) n = -1;
    else if (mButtonType == 2) n = -2;
    else if (mButtonType == 3) n = mGroupIndex;
    else                       n = -1;

// create the button

    Codef(_T("%s = new wxSpeedButton(%W, %I, %t, %s, %d, %d, %d, %b, %P, %S, %T, %V, %N);\n"),
        vname.wx_str(), mLabel.wx_str(), bname.wx_str(), mGlyphCount, mMargin, n, mAllowAllUp);
    BuildSetupWindowCode();

// and individual button settings

    if (mButtonDown) Codef(_T("%s->SetDown(true);\n"), vname.wx_str());
    Codef(_T("%s->SetUserData(%d);\n"), vname.wx_str(), mUserData);
}

//------------------------------------------------------------------------------
// if no bitmap is selected, make a NULL bitmap
// if art-provider is selected, make a bitmap from that
// if an XPM file is selected, and user selected to #include the XPM file,
//   then #include the file and make a bitmap from the data
// else it must be a file was selected, so make a bitmap from the file

void  wxsSpeedButton::BuildBitmap(void) {
wxString    s,v;

// make a name for the bitmap

    v = GetVarName() + _("_BMP");

// no image

    if ((mGlyph.Id.IsEmpty()) && (mGlyph.FileName.IsEmpty())) {
        Codef(_T("wxBitmap %s = wxNullBitmap;\n"), v.wx_str());
    }

// art-provider image

    else if (! mGlyph.Id.IsEmpty()) {
        Codef(_T("wxBitmap %s(%i);\n"), v.wx_str(), &mGlyph);
    }

// is it an XPM and do we want to #include it?

    else if ((IsImageXPM(mGlyph)) && (mUseInclude)) {
        s = mGlyph.FileName;
        s.Replace(_("\\"), _("/"), true);
        s = _T("\"") + s + _T("\"");
        AddHeader(s, GetInfo().ClassName, 0);

        s = GetXPMName(mGlyph);
        Codef(_T("wxBitmap %s(%s);\n"), v.wx_str(), s.wx_str());
    }

// else a normal image file

    else if (! mGlyph.FileName.IsEmpty()) {
        s = mGlyph.FileName;
        s.Replace(_("\\"), _("/"), true);

        Codef(_T("wxBitmap %s(%t, wxBITMAP_TYPE_ANY);\n"), v.wx_str(), s.wx_str());
    }

// an unknown and unexpected set of conditions

    else {
        Codef(_T("wxBitmap %s = wxNullBitmap;\n"), v.wx_str());
    };
}

//------------------------------------------------------------------------------
// did the user specify a XPM image file?

bool    wxsSpeedButton::IsImageXPM(wxsBitmapData &inData) {
wxString    s;

// a special case of no image at all

    if (inData.IsEmpty()) return false;

// or built-in wxWidgets art

    inData.Id.Trim();
    if (! inData.Id.IsEmpty()) return false;

// no filename given?

    inData.FileName.Trim();
    if (inData.FileName.IsEmpty()) return false;

// file must actually exist

    if (! wxFileName::FileExists(inData.FileName)) return false;

// last 4 chars of filename should be ".XPM"

    s = inData.FileName.Right(4);
    s.MakeUpper();
    if (s == _T(".XPM")) return true;

// must be something else

    return false;
}

//------------------------------------------------------------------------------
// if an image data buffer specifies an XPM file, then return the name
// of the static char array defined by the file

wxString    wxsSpeedButton::GetXPMName(wxsBitmapData &inData) {
    wxFileInputStream input( inData.FileName );
    wxTextInputStream txt( input );


// this will be our return value

    wxString v = _T("");

// read until EOF, keeping the last data name we find

    while (! input.Eof()) {

// next line

        wxString s = txt.ReadLine();
//std::cout << "line " << i++ << ") " << s.mb_str() << std::endl;

// the "static" keyword is optional under some compilers

        s.Trim(false);
        int n = s.Find(_T("static"));
        if (n == 0) s.erase(0, 6);

// but the "char" and "*" are required

        s.Trim(false);
        n = s.Find(_T("char"));
        if (n != 0) continue;
        s.erase(0, 4);

        s.Trim(false);
        n = s.Find(_T("*"));
        if (n != 0) continue;
        s.erase(0, 1);

// everything up to the "[" is the name of an array

        s.Trim(false);
        n = s.Find(_T("["));
        if (n == wxNOT_FOUND) n = s.Len();

        v = s.Left(n);
    };

// done with file

//    txt.Close();

// done

    return v;
}




//------------------------------------------------------------------------------

wxObject* wxsSpeedButton::OnBuildPreview(wxWindow* Parent, long Flags) {
int             n;
wxSpeedButton   *sb;
wxBitmap        bmp;

// make bitmap

    bmp = mGlyph.GetPreview(wxDefaultSize);

// fix the group index depending upon the button type

    if      (mButtonType == 0) n = 0;
    else if (mButtonType == 1) n = -1;
    else if (mButtonType == 2) n = -2;
    else if (mButtonType == 3) n = mGroupIndex;
    else                       n = -1;

// make a button

    sb = new wxSpeedButton(Parent, GetId(), mLabel, bmp, mGlyphCount,
        mMargin, n, mAllowAllUp, Pos(Parent), Size(Parent), Style(),
        wxDefaultValidator, _T("SpeedButton"));
    if (sb == NULL) return NULL;
    SetupWindow(sb, Flags);

    if (mButtonDown) sb->SetDown(mButtonDown);

// done

    return sb;
}

//------------------------------------------------------------------------------

void wxsSpeedButton::OnEnumWidgetProperties(long Flags) {
static const long    TypeValues[] = {    0,                   1,                   2,                     3,                 0};
static const wxChar* TypeNames[]  = {_T("Simple Button"), _T("Toggle Button"), _T("Group By Parent"), _T("Group By Index"),  0};

    WXS_STRING(wxsSpeedButton, mLabel,      _("Label"),               _T("label"),      _T(""), false);
    WXS_BITMAP(wxsSpeedButton, mGlyph,      _("Glyph"),               _T("glyph"),      _T("wxART_OTHER"));
    WXS_LONG(  wxsSpeedButton, mGlyphCount, _("Glyph Count"),         _T("glyphcount"), 0)
    WXS_BOOL(  wxsSpeedButton, mUseInclude, _("Use XPM As #include"), _T("useinclude"), true);
    WXS_LONG(  wxsSpeedButton, mMargin,     _("Margin Space"),        _T("margin"),     2)
    WXS_BOOL(  wxsSpeedButton, mButtonDown, _("Button DOWN"),         _("buttondown"),  false);
    WXS_ENUM(  wxsSpeedButton, mButtonType, _("Button Type"),         _T("buttontype"), TypeValues, TypeNames, 1);
    WXS_LONG(  wxsSpeedButton, mGroupIndex, _("Group Index"),         _T("groupindex"), 0)
    WXS_BOOL(  wxsSpeedButton, mAllowAllUp, _("Allow All Up"),        _T("allowallup"), true);
    WXS_LONG(  wxsSpeedButton, mUserData,   _("User Data"),           _T("userdata"),   0)

};



