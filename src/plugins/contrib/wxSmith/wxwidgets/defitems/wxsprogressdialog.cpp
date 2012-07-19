/** \file wxsprogressdialog.cpp
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
#include "wxsprogressdialog.h"
#include <wx/progdlg.h>


namespace
{
    wxsRegisterItem<wxsProgressDialog> Reg(
        _T("ProgressDialog"),               // Class base name
        wxsTTool,                                   // Item type
        _T("Dialogs"),                          // Category in palette
        100,                                             // Priority in palette
        false);                                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsProgressDialogStyles, wxT("wxPD_AUTO_HIDE | wxPD_APP_MODAL"));
        WXS_ST_CATEGORY("wxProgressDialog")
        WXS_ST(wxPD_APP_MODAL)
        WXS_ST(wxPD_AUTO_HIDE)
        WXS_ST(wxPD_CAN_ABORT)
        WXS_ST(wxPD_CAN_SKIP)
        WXS_ST(wxPD_ELAPSED_TIME)
        WXS_ST(wxPD_ESTIMATED_TIME)
        WXS_ST(wxPD_REMAINING_TIME)
        WXS_ST(wxPD_SMOOTH)
    WXS_ST_END()

}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsProgressDialog::wxsProgressDialog(wxsItemResData *Data):
    wxsTool(Data,
            &Reg.Info,
            NULL,
            wxsProgressDialogStyles,
            (flVariable | flId | flSubclass | flExtraCode)),
            m_sTitle(wxEmptyString),
            m_sMessage(wxEmptyString),
            m_iMaxValue(100),
            m_bRunAtStartup(false)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 * This function either creates an uninitialised pointer or initialises and runs the dialogue at start-up,
 * depending on the state of m_bRunAtStartup.
 * There is no way to initialise the dialogue without having it run at the same time, by design.
 */
void wxsProgressDialog::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/progdlg.h>"), GetInfo().ClassName, 0);

            if(m_bRunAtStartup){
                // Initialise and display the dialogue at application start-up.
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("%C(%t, %t, %d, %W, %T);\n"), m_sTitle.wx_str(), m_sMessage.wx_str(), m_iMaxValue);
                #else
                Codef(_T("%C(%t, %t, %d, %W, %T);\n"), m_sTitle.c_str(), m_sMessage.c_str(), m_iMaxValue);
                #endif
            }

            BuildSetupWindowCode();
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsProgressDialog::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsProgressDialog::OnEnumToolProperties(long Flags)
{
    WXS_BOOL(wxsProgressDialog, m_bRunAtStartup, _("Run At Startup"), _T("run_at_startup"), false)
    WXS_SHORT_STRING(wxsProgressDialog, m_sTitle, _("Title"), _T("title"), wxEmptyString, true);
    WXS_SHORT_STRING(wxsProgressDialog, m_sMessage, _("Message"), _T("message"), wxEmptyString, true);
    WXS_LONG(wxsProgressDialog, m_iMaxValue,  _("Max. Value"), _T("max_value"), 100)
}
