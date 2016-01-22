/** \file wxstextentrydialog.cpp
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
#include "wxstextentrydialog.h"
#include <wx/textdlg.h>


namespace
{
    wxsRegisterItem<wxsTextEntryDialog> Reg(
        _T("TextEntryDialog"),               // Class base name
        wxsTTool,                               // Item type
        _T("Dialogs"),                          // Category in palette
        50,                                         // Priority in palette
        false);                                     // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsTextEntryDialogStyles, wxT("wxOK | wxCANCEL | wxCENTRE | wxWS_EX_VALIDATE_RECURSIVELY"));
        WXS_ST_CATEGORY("wxTextEntryDialog")
        WXS_ST(wxOK)
        WXS_ST(wxCANCEL)
        WXS_ST(wxCENTRE)
        WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
        WXS_ST(wxTE_MULTILINE)
        WXS_ST(wxTE_PASSWORD)
        WXS_ST(wxTE_READONLY)
        WXS_ST(wxTE_RICH)
        WXS_ST(wxTE_RICH2)
        WXS_ST(wxTE_NOHIDESEL)
        WXS_ST(wxHSCROLL)
        WXS_ST(wxTE_CENTRE)
        WXS_ST(wxTE_RIGHT)
        WXS_ST(wxTE_CHARWRAP)
        WXS_ST(wxTE_WORDWRAP)
        WXS_ST(wxTE_BESTWRAP)
        WXS_ST(wxTE_CAPITALIZE)
    WXS_ST_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsTextEntryDialog::wxsTextEntryDialog(wxsItemResData *Data):
    wxsTool(Data,
            &Reg.Info,
            NULL,
            wxsTextEntryDialogStyles,
            (flVariable | flId | flSubclass | flExtraCode)),
            m_sCaption(wxGetTextFromUserPromptStr),
            m_sMessage(wxEmptyString),
            m_sDefaultValue(wxEmptyString)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsTextEntryDialog::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/textdlg.h>"), GetInfo().ClassName, 0);
            Codef(_T("%C(%W, %t, %t, %t, %T, %P);\n"), m_sMessage.wx_str(), m_sCaption.wx_str(), m_sDefaultValue.wx_str());
            BuildSetupWindowCode();
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsTextEntryDialog::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsTextEntryDialog::OnEnumToolProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsTextEntryDialog, m_sCaption, _("Caption"), _T("caption"), wxGetTextFromUserPromptStr, false);
    WXS_SHORT_STRING(wxsTextEntryDialog, m_sMessage, _("Message"), _T("message"), wxEmptyString, false);
    WXS_SHORT_STRING(wxsTextEntryDialog, m_sDefaultValue, _("Default Value"), _T("default_value"), wxEmptyString, false);
}
