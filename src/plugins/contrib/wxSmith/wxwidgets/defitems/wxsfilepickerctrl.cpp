/**  \file wxsfilepickerctrl.cpp
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

#include "wxsfilepickerctrl.h"

#include <wx/filepicker.h>

namespace
{
    wxsRegisterItem<wxsFilePickerCtrl> Reg(_T("FilePickerCtrl"), wxsTWidget, _T("Advanced"), 110);

#ifdef __WXGTK__
    // GTK apps usually don't have a textctrl next to the picker
    WXS_ST_BEGIN(wxsFilePickerCtrlStyles,_T("wxFLP_OPEN|wxFLP_FILE_MUST_EXIST"))
#else
    WXS_ST_BEGIN(wxsFilePickerCtrlStyles,_T("wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST"))
#endif
        WXS_ST_CATEGORY("wxFilePickerCtrl")
        WXS_ST(wxFLP_CHANGE_DIR)
        WXS_ST(wxFLP_FILE_MUST_EXIST)
        WXS_ST(wxFLP_OPEN)
        WXS_ST(wxFLP_OVERWRITE_PROMPT)
        WXS_ST(wxFLP_SAVE)
        WXS_ST(wxFLP_USE_TEXTCTRL)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsFilePickerCtrlEvents)
        WXS_EVI(EVT_FILEPICKER_CHANGED, wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEvent , FileChanged)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsFilePickerCtrl::wxsFilePickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsFilePickerCtrlEvents,
        wxsFilePickerCtrlStyles),
        m_sMessage(wxFileSelectorPromptStr),
        m_sPath(wxEmptyString),
        m_sWildcard(wxFileSelectorDefaultWildcardStr)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsFilePickerCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/filepicker.h>"),GetInfo().ClassName,0);
            Codef(_T("%C(%W, %I, %n, %t, %n, %P, %S, %T, %V, %N);\n"), m_sPath.wx_str(), m_sMessage.wx_str(), m_sWildcard.wx_str());
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFilePickerCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief    Build the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                    The control flags.
 * \return wxObject*                     The constructed control.
 *
 */
wxObject* wxsFilePickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxFilePickerCtrl* Preview;
    Preview = new wxFilePickerCtrl(Parent, GetId(), m_sPath, m_sMessage, m_sWildcard, Pos(Parent), Size(Parent), Style());
    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsFilePickerCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_SHORT_STRING(wxsFilePickerCtrl, m_sMessage, _("Message"), _T("message"), wxFileSelectorPromptStr, false)
    WXS_SHORT_STRING(wxsFilePickerCtrl, m_sPath, _("Path"), _T("path"), wxEmptyString, false)
    WXS_SHORT_STRING(wxsFilePickerCtrl, m_sWildcard, _("Wildcard"), _T("wildcard"), wxFileSelectorDefaultWildcardStr, false)
}
