/** \file wxsrichtextstyleorganiserdialog.cpp
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
#include "wxsrichtextstyleorganiserdialog.h"
#include <wx/richtext/richtextstyledlg.h>


namespace
{
    wxsRegisterItem<wxsRichTextStyleOrganiserDialog> Reg(
        _T("RichTextStyleOrganiserDialog"),      // Class base name
        wxsTTool,                                                       // Item type
        _T("Dialogs"),                                              // Category in palette
        80,                                                                     // Priority in palette
        false);                                                             // We do not allow this item inside XRC files

    static const long arrStyleValues[] = {
        wxRICHTEXT_ORGANISER_APPLY_STYLES,
        wxRICHTEXT_ORGANISER_CREATE_STYLES,
        wxRICHTEXT_ORGANISER_DELETE_STYLES,
        wxRICHTEXT_ORGANISER_EDIT_STYLES,
        wxRICHTEXT_ORGANISER_OK_CANCEL,
        wxRICHTEXT_ORGANISER_RENAME_STYLES,
        wxRICHTEXT_ORGANISER_RENUMBER,
        wxRICHTEXT_ORGANISER_SHOW_ALL,
        wxRICHTEXT_ORGANISER_SHOW_CHARACTER,
        wxRICHTEXT_ORGANISER_SHOW_LIST,
        wxRICHTEXT_ORGANISER_SHOW_PARAGRAPH
    };

    static const wxChar* arrStyleValueNames[] = {
        _T("wxRICHTEXT_ORGANISER_APPLY_STYLES"),
        _T("wxRICHTEXT_ORGANISER_CREATE_STYLES"),
        _T("wxRICHTEXT_ORGANISER_DELETE_STYLES"),
        _T("wxRICHTEXT_ORGANISER_EDIT_STYLES"),
        _T("wxRICHTEXT_ORGANISER_OK_CANCEL"),
        _T("wxRICHTEXT_ORGANISER_RENAME_STYLES"),
        _T("wxRICHTEXT_ORGANISER_RENUMBER"),
        _T("wxRICHTEXT_ORGANISER_SHOW_ALL"),
        _T("wxRICHTEXT_ORGANISER_SHOW_CHARACTER"),
        _T("wxRICHTEXT_ORGANISER_SHOW_LIST"),
        _T("wxRICHTEXT_ORGANISER_SHOW_PARAGRAPH"),
        NULL
    };
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsRichTextStyleOrganiserDialog::wxsRichTextStyleOrganiserDialog(wxsItemResData *Data):
    wxsTool(Data,
            &Reg.Info,
            NULL,
            NULL,
            (flVariable | flId | flSubclass | flExtraCode)),
            m_sCaption(SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_TITLE),
            m_iFlags(wxRICHTEXT_ORGANISER_SHOW_ALL|wxRICHTEXT_ORGANISER_DELETE_STYLES|wxRICHTEXT_ORGANISER_CREATE_STYLES|
                                wxRICHTEXT_ORGANISER_APPLY_STYLES|wxRICHTEXT_ORGANISER_EDIT_STYLES|wxRICHTEXT_ORGANISER_RENAME_STYLES)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsRichTextStyleOrganiserDialog::OnBuildCreatingCode()
{
    wxString sFlags;
    wxString sStyleSheetName = GetCoderContext()->GetUniqueName(_T("richTextStyleSheet"));
    switch(GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T(" <wx/richtext/richtextstyledlg.h>"), GetInfo().ClassName, 0);

            for(int i = 0;arrStyleValueNames[i];i++){
                if((m_iFlags & arrStyleValues[i]) == arrStyleValues[i]){
                    sFlags << arrStyleValueNames[i] << _T("|");
                }
            }
            if(sFlags.IsEmpty()){
                sFlags = _T("0");
            }
            else{
                sFlags.RemoveLast();
            }

            AddDeclaration(wxString::Format(wxT("wxRichTextStyleSheet  *%s;"), sStyleSheetName.wx_str()));
            Codef(_T("\t%s = new wxRichTextStyleSheet;\n"), sStyleSheetName.wx_str());

            Codef(_T("%C(%s, %s, NULL, %W, %I, %t, ")
                  wxT("SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_POSITION, ")
                  wxT("SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_SIZE, ")
                  wxT("SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_STYLE);\n"),
                  sFlags.wx_str(), sStyleSheetName.wx_str(), m_sCaption.wx_str());

            BuildSetupWindowCode();
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsRichTextStyleOrganiserDialog::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsRichTextStyleOrganiserDialog::OnEnumToolProperties(long Flags)
{
    // Default to wxRICHTEXT_ORGANISER_ORGANISE.
    WXS_FLAGS(wxsRichTextStyleOrganiserDialog, m_iFlags, _("Style Flags"), _T("style_flags"), arrStyleValues, arrStyleValueNames,
                                                        wxRICHTEXT_ORGANISER_SHOW_ALL|wxRICHTEXT_ORGANISER_DELETE_STYLES|wxRICHTEXT_ORGANISER_CREATE_STYLES
                                                        |wxRICHTEXT_ORGANISER_APPLY_STYLES|wxRICHTEXT_ORGANISER_EDIT_STYLES|wxRICHTEXT_ORGANISER_RENAME_STYLES )
    WXS_SHORT_STRING(wxsRichTextStyleOrganiserDialog, m_sCaption, _("Caption"), _T("caption"), SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_TITLE, true);
}
