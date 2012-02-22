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

#include "wxsSTEditor.h"

//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "STE16.xpm"
    #include "STE32.xpm"

    wxsRegisterItem<wxsSTEditor> Reg(
        _T("wxSTEditor"),           // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("Styled Text"),              // Category in palette
        55,                             // Priority in palette
        _T("STE"),                      // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(STE32_xpm),            // 32x32 bitmap
        wxBitmap(STE16_xpm),            // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsSTEditorStyles,_T("wxRE_MULTILINE|wxRAISED_BORDER|wxWANTS_CHARS"))
        WXS_ST_CATEGORY("wxSTEditor")

        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSTEditorEvents)

       WXS_EVI(EVT_TEXT_ENTER,                    wxEVT_COMMAND_TEXT_ENTER,                    wxCommandEvent,  EvTextEnter)
       WXS_EVI(EVT_TEXT,                          wxEVT_COMMAND_TEXT_UPDATED,                  wxCommandEvent,  EvTextUpdated)
       WXS_EVI(EVT_TEXT_URL,                      wxEVT_COMMAND_TEXT_URL,                      wxTextUrlEvent,  EvTextURL)

        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsSTEditor::wxsSTEditor(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSTEditorEvents,
        wxsSTEditorStyles)
{
    mSize.Set(32, 32);
    mText.Clear();
    mVirtualSize.IsDefault = true;

}

//------------------------------------------------------------------------------

void wxsSTEditor::OnBuildCreatingCode() {
wxString            vname;
wxString            aname;
wxString            ss, tt;
wxColour            fg;

// valid language?

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsSTEditor::OnBuildCreatingCode"),GetLanguage());

// who we are

    vname = GetVarName();
    aname = vname + _T("_Attr");

// include files

    AddHeader(_("\"stedit.h\""), GetInfo().ClassName, 0);

// make our own size specifier with our default values

    mSize.SetDefaults(wxSize(32, 32));
    ss.Printf(_T("wxSize(%d, %d)"), mSize.GetWidth(), mSize.GetHeight());

// create the panel

    Codef(_T("%C(%W, %I, %P, %s, %T, %N);\n"), ss.c_str());

// other declarations

    BuildSetupWindowCode();

// a virtual size

    if (! mVirtualSize.IsDefault) {
        ss = mVirtualSize.GetSizeCode(GetCoderContext());
        Codef(_T("%ASetVirtualSize(%s);\n"), ss.c_str());
    };

// initial text

    for(size_t i=0; i<mText.GetCount(); i++) {
        ss  = mText.Item(i);
        ss += _T("\n");
        Codef(_T("%AAppendText(%t);\n"), ss.c_str());
    };
}

//------------------------------------------------------------------------------

wxObject* wxsSTEditor::OnBuildPreview(wxWindow* Parent, long Flags) {
wxSTEditor          *ste;
wxColour            fg;
wxString            ss;
wxSize              zz;

// the default size of this widget is microscopic
// use this to make a reasonable default size

    mSize = Size(Parent);
    mSize.SetDefaults(wxSize(32, 32));

// make the basic widget

    ste = new wxSTEditor(Parent, GetId(), Pos(Parent), mSize, Style());

// the rest of the attributtes

    SetupWindow(ste, Flags);

// a virtual size

    if (! mVirtualSize.IsDefault) {
        zz = mVirtualSize.GetSize(Parent);
        ste->SetVirtualSize(zz);
    };

// add in initial text

    for(size_t i=0; i<mText.GetCount(); i++) {
        ss  = mText.Item(i);
        ss += _T("\n");
        ste->AppendText(ss);
    };

// done

    return ste;
}

//------------------------------------------------------------------------------

void wxsSTEditor::OnEnumWidgetProperties(long Flags) {

// initial text contents

    WXS_ARRAYSTRING(wxsSTEditor, mText, _("Text"), _("mText"), _("text"));
    WXS_SIZE(       wxsSTEditor, mVirtualSize,   _T("Default Virtual Size?"),           _T("Virtual Width"),    _T("Virtual Height"), _T("Use Dialog Units?"), _T("mVirtualSize"));

};



