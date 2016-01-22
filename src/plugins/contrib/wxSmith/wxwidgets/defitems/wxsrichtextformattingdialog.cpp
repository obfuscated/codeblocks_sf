/** \file wxsrichtextformattingdialog.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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

#include "../wxsitemresdata.h"
#include "wxsrichtextformattingdialog.h"
#include <wx/richtext/richtextformatdlg.h>


namespace
{
    wxsRegisterItem<wxsRichTextFormattingDialog> Reg(
        _T("RichTextFormattingDialog"),      // Class base name
        wxsTTool,                                               // Item type
        _T("Dialogs"),                                      // Category in palette
        90,                                                             // Priority in palette
        false);                                                     // We do not allow this item inside XRC files

    static const long arrPageValues[] = {
        wxRICHTEXT_FORMAT_BULLETS,
        wxRICHTEXT_FORMAT_FONT,
        wxRICHTEXT_FORMAT_HELP_BUTTON,
        wxRICHTEXT_FORMAT_INDENTS_SPACING,
        wxRICHTEXT_FORMAT_LIST_STYLE,
        wxRICHTEXT_FORMAT_STYLE_EDITOR,
        wxRICHTEXT_FORMAT_TABS
    };

    static const wxChar* arrPageValueNames[] = {
        _T("wxRICHTEXT_FORMAT_BULLETS"),
        _T("wxRICHTEXT_FORMAT_FONT"),
        _T("wxRICHTEXT_FORMAT_HELP_BUTTON"),
        _T("wxRICHTEXT_FORMAT_INDENTS_SPACING"),
        _T("wxRICHTEXT_FORMAT_LIST_STYLE"),
        _T("wxRICHTEXT_FORMAT_STYLE_EDITOR"),
        _T("wxRICHTEXT_FORMAT_TABS"),
        NULL
    };
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsRichTextFormattingDialog::wxsRichTextFormattingDialog(wxsItemResData *Data):
    wxsTool(Data,
            &Reg.Info,
            NULL,
            NULL,
            (flVariable | flId | flSubclass | flExtraCode)),
            m_sTitle(_T("Formatting")),
            m_iFlags(wxRICHTEXT_FORMAT_FONT|wxRICHTEXT_FORMAT_TABS|wxRICHTEXT_FORMAT_BULLETS|wxRICHTEXT_FORMAT_INDENTS_SPACING)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsRichTextFormattingDialog::OnBuildCreatingCode()
{
    wxString sFlags;
    switch(GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/richtext/richtextformatdlg.h>"), GetInfo().ClassName, 0);

            for(int i = 0;arrPageValueNames[i];i++){
                if((m_iFlags & arrPageValues[i]) == arrPageValues[i]){
                    sFlags << arrPageValueNames[i] << _T("|");
                }
            }
            if(sFlags.IsEmpty()){
                sFlags = _T("0");
            }
            else{
                sFlags.RemoveLast();
            }

            Codef(_T("%C(%s, %W, %t, %I, %P, %S);\n"), sFlags.wx_str(), m_sTitle.wx_str());
            BuildSetupWindowCode();
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsRichTextFormattingDialog::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsRichTextFormattingDialog::OnEnumToolProperties(cb_unused long Flags)
{
    WXS_FLAGS(wxsRichTextFormattingDialog, m_iFlags, _("Page Flags"), _T("page_flags"), arrPageValues, arrPageValueNames,
                                wxRICHTEXT_FORMAT_FONT|wxRICHTEXT_FORMAT_TABS|wxRICHTEXT_FORMAT_BULLETS|wxRICHTEXT_FORMAT_INDENTS_SPACING )
    WXS_SHORT_STRING(wxsRichTextFormattingDialog, m_sTitle, _("Title"), _T("title"), _("Formatting"), true);
}
