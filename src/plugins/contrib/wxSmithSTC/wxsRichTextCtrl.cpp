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

#include "wxsRichTextCtrl.h"

//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "RTF16.xpm"
    #include "RTF32.xpm"

    wxsRegisterItem<wxsRichTextCtrl> Reg(
        _T("wxRichTextCtrl"),           // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("Styled Text"),                // Category in palette
        80,                             // Priority in palette
        _T("RTF"),                      // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(RTF32_xpm),            // 32x32 bitmap
        wxBitmap(RTF16_xpm),            // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsRichTextCtrlStyles,_T("wxRE_MULTILINE|wxRAISED_BORDER|wxWANTS_CHARS"))
        WXS_ST_CATEGORY("wxRichTextCtrl")
        WXS_ST(wxRE_READONLY )
        WXS_ST(wxRE_MULTILINE )
        WXS_ST(wxRE_CENTER_CARET )
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsRichTextCtrlEvents)
       WXS_EVI(EVT_RICHTEXT_CHARACTER,            wxEVT_COMMAND_RICHTEXT_CHARACTER,            wxRichTextEvent, EvCharacter)
       WXS_EVI(EVT_RICHTEXT_DELETE,               wxEVT_COMMAND_RICHTEXT_DELETE,               wxRichTextEvent, EvDelete)
       WXS_EVI(EVT_RICHTEXT_RETURN,               wxEVT_COMMAND_RICHTEXT_RETURN,               wxRichTextEvent, EvReturn)
       WXS_EVI(EVT_RICHTEXT_STYLE_CHANGED,        wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED,        wxRichTextEvent, EvStyleChanged)
       WXS_EVI(EVT_RICHTEXT_STYLESHEET_CHANGED,   wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING,  wxRichTextEvent, EvStylesheetChanging)
       WXS_EVI(EVT_RICHTEXT_STYLESHEET_REPLACING, wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, wxRichTextEvent, EvStylesheetReplacing)
       WXS_EVI(EVT_RICHTEXT_STYLESHEET_REPLACED,  wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED,  wxRichTextEvent, EvStylesheetReplaced)
       WXS_EVI(EVT_RICHTEXT_CONTENT_INSERTED,     wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED,     wxRichTextEvent, EvContentInserted)
       WXS_EVI(EVT_RICHTEXT_CONTENT_DELETED,      wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED,      wxRichTextEvent, EvContentDeleted)
       WXS_EVI(EVT_RICHTEXT_BUFFER_RESET,         wxEVT_COMMAND_RICHTEXT_BUFFER_RESET,         wxRichTextEvent, EvBufferReset)

       WXS_EVI(EVT_TEXT_ENTER,                    wxEVT_COMMAND_TEXT_ENTER,                    wxCommandEvent,  EvTextEnter)
       WXS_EVI(EVT_TEXT,                          wxEVT_COMMAND_TEXT_UPDATED,                  wxCommandEvent,  EvTextUpdated)
       WXS_EVI(EVT_TEXT_URL,                      wxEVT_COMMAND_TEXT_URL,                      wxTextUrlEvent,  EvTextURL)

        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsRichTextCtrl::wxsRichTextCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsRichTextCtrlEvents,
        wxsRichTextCtrlStyles)
{
    mSize.Set(32, 32);
    mText.Clear();
    mVirtualSize.IsDefault = true;

}

//------------------------------------------------------------------------------

void wxsRichTextCtrl::OnBuildCreatingCode() {
wxString            vname;
wxString            aname;
wxString            ss, tt;
wxsBaseProperties   *bp;
wxColour            fg;

// valid language?

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsRichTextCtrl::OnBuildCreatingCode"),GetLanguage());

// who we are

    vname = GetVarName();
    aname = vname + _T("_Attr");

// include files

    AddHeader(_("<wx/richtext/richtextctrl.h>"), GetInfo().ClassName, 0);

// make our own size specifier with our default values

    mSize.SetDefaults(wxSize(32, 32));
    ss.Printf(_T("wxSize(%d, %d)"), mSize.GetWidth(), mSize.GetHeight());

// create the panel

    Codef(_T("%C(%W, %I, wxEmptyString, %P, %s, %T, wxDefaultValidator, %N);\n"), ss.c_str());


// text color via an attribute record

    bp = GetBaseProps();
    fg = bp->m_Fg.GetColour();

    if (fg.Ok()) {
        Codef(_T("wxRichTextAttr  %s;\n"), aname.c_str());
        Codef(_T("%s.SetTextColour(wxColour(%d, %d, %d));\n"), aname.c_str(), fg.Red(), fg.Green(), fg.Blue());
        Codef(_T("%s.SetFlags(wxTEXT_ATTR_TEXT_COLOUR);\n"), aname.c_str());
        Codef(_T("%s->SetBasicStyle(%s);\n"), vname.c_str(), aname.c_str());
    };

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

wxObject* wxsRichTextCtrl::OnBuildPreview(wxWindow* Parent, long Flags) {
wxRichTextCtrl      *rtf;
wxRichTextAttr      attr;
wxsBaseProperties   *bp;
wxColour            fg;
wxString            ss;
wxSize              zz;

// the default size of this widget is microscopic
// use this to make a reasonable default size

    mSize = Size(Parent);
    mSize.SetDefaults(wxSize(32, 32));

// make the basic widget

    rtf = new wxRichTextCtrl(Parent, GetId(), wxEmptyString, Pos(Parent), mSize, Style(), wxDefaultValidator);

// text color

    bp = GetBaseProps();
    fg = bp->m_Fg.GetColour();

    if (fg.Ok()) {
        attr.SetTextColour(fg);
        attr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR);
        rtf->SetBasicStyle(attr);
    };

// the rest of the attributtes

    SetupWindow(rtf, Flags);

// a virtual size

    if (! mVirtualSize.IsDefault) {
        zz = mVirtualSize.GetSize(Parent);
        rtf->SetVirtualSize(zz);
    };

// add in initial text

    for(size_t i=0; i<mText.GetCount(); i++) {
        ss  = mText.Item(i);
        ss += _T("\n");
        rtf->AppendText(ss);
    };

// done

    return rtf;
}

//------------------------------------------------------------------------------

void wxsRichTextCtrl::OnEnumWidgetProperties(long Flags) {

// initial text contents

    WXS_ARRAYSTRING(wxsRichTextCtrl, mText, _("Text"), _("mText"), _("text"));
    WXS_SIZE(       wxsRichTextCtrl, mVirtualSize,   _T("Default Virtual Size?"),           _T("Virtual Width"),    _T("Virtual Height"), _T("Use Dialog Units?"), _T("mVirtualSize"));

};



