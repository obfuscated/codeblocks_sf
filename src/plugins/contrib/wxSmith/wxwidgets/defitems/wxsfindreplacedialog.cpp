/** \file wxsfindreplacedialog.cpp
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
#include "wxsfindreplacedialog.h"
#include <wx/fdrepdlg.h>


namespace
{
    wxsRegisterItem<wxsFindReplaceDialog> Reg(
        _T("FindReplaceDialog"),           // Class base name
        wxsTTool,                                       // Item type
        _T("Dialogs"),                              // Category in palette
        170,                                                 // Priority in palette
        false);                                             // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsFindReplaceDialogStyles, NULL);
        WXS_ST_CATEGORY("wxFindReplaceDialog")
        WXS_ST(wxFR_REPLACEDIALOG)
        WXS_ST(wxFR_NOUPDOWN)
        WXS_ST(wxFR_NOMATCHCASE)
        WXS_ST(wxFR_NOWHOLEWORD)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsFindReplaceDialogEvents)
        WXS_EVI(EVT_FIND, wxEVT_COMMAND_FIND, wxFindDialogEvent, Find)
        WXS_EVI(EVT_FIND_NEXT, wxEVT_COMMAND_FIND_NEXT, wxFindDialogEvent, FindNext)
        WXS_EVI(EVT_FIND_REPLACE, wxEVT_COMMAND_FIND_REPLACE, wxFindDialogEvent, Replace)
        WXS_EVI(EVT_FIND_REPLACE_ALL, wxEVT_COMMAND_FIND_REPLACE_ALL, wxFindDialogEvent, ReplaceAll)
        WXS_EVI(EVT_FIND_CLOSE, wxEVT_COMMAND_FIND_CLOSE, wxFindDialogEvent, Close)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsFindReplaceDialog::wxsFindReplaceDialog(wxsItemResData *Data):
    wxsTool(Data,
            &Reg.Info,
            wxsFindReplaceDialogEvents,
            wxsFindReplaceDialogStyles,
            (flVariable | flId | flSubclass | flExtraCode)),
            m_sCaption(wxEmptyString)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsFindReplaceDialog::OnBuildCreatingCode()
{
    wxString sfindReplaceData;
    wxString sNote(_("// NOTE: In order for events to connect properly you must set the ID of this wxFindReplaceDialog to -1 in wxSmith."));
    switch(GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/fdrepdlg.h>"), GetInfo().ClassName, 0);

            sfindReplaceData = GetCoderContext()->GetUniqueName(_T("findReplaceData"));
            #if wxCHECK_VERSION(2, 9, 0)
            AddDeclaration(wxT("wxFindReplaceData %s;"), sfindReplaceData.wx_str());
            #else
            AddDeclaration(wxString::Format(wxT("wxFindReplaceData %s;"), sfindReplaceData.c_str()));
            #endif

            #if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("\t%s\n"), sNote.wx_str());
            Codef(_T("%C(%W, &%s, %t, %T);\n"), sfindReplaceData.wx_str(), m_sCaption.wx_str());
            #else
            Codef(_T("\t%s\n"), sNote.c_str());
            Codef(_T("%C(%W, &%s, %t, %T);\n"), sfindReplaceData.c_str(), m_sCaption.c_str());
            #endif

            BuildSetupWindowCode();
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsFindReplaceDialog::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsFindReplaceDialog::OnEnumToolProperties(long Flags)
{
    WXS_SHORT_STRING(wxsFindReplaceDialog, m_sCaption, _("Caption"), _T("caption"), wxEmptyString, false);
}
