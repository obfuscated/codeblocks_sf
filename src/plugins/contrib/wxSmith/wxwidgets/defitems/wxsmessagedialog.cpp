/** \file wxsmessagedialog.cpp
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
#include "wxsmessagedialog.h"
#include <wx/msgdlg.h>


namespace
{
    wxsRegisterItem<wxsMessageDialog> Reg(
        _T("MessageDialog"),               // Class base name
        wxsTTool,                                   // Item type
        _T("Dialogs"),                          // Category in palette
        150,                                             // Priority in palette
        false);                                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsMessageDialogStyles, wxT("wxOK | wxCANCEL"));
        WXS_ST_CATEGORY("wxMessageDialog")
        WXS_ST(wxOK)
        WXS_ST(wxCANCEL)
        WXS_ST(wxYES_NO)
        WXS_ST(wxYES_DEFAULT)
        WXS_ST(wxNO_DEFAULT)
        WXS_ST(wxICON_EXCLAMATION)
        WXS_ST(wxICON_HAND)
        WXS_ST(wxICON_ERROR)
        WXS_ST(wxICON_QUESTION)
        WXS_ST(wxICON_INFORMATION)
        // This style is Windows only.
        if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
            WXS_ST(wxSTAY_ON_TOP)
        }
    WXS_ST_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsMessageDialog::wxsMessageDialog(wxsItemResData *Data):
    wxsTool(Data,
            &Reg.Info,
            NULL,
            wxsMessageDialogStyles,
            (flVariable | flId | flSubclass | flExtraCode)),
            m_sCaption(wxMessageBoxCaptionStr),
            m_sMessage(wxEmptyString)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsMessageDialog::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/msgdlg.h>"), GetInfo().ClassName, 0);
            Codef(_T("%C(%W, %t, %t, %T, %P);\n"), m_sMessage.wx_str(), m_sCaption.wx_str());
            BuildSetupWindowCode();
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsMessageDialog::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsMessageDialog::OnEnumToolProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsMessageDialog, m_sCaption, _("Caption"), _T("caption"), _T(""), true);
    WXS_SHORT_STRING(wxsMessageDialog, m_sMessage, _("Message"), _T("message"), wxMessageBoxCaptionStr, true);
}
