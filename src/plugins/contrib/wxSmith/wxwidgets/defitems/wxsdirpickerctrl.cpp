/**  \file wxsdirpickerctrl.cpp
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

#include "wxsdirpickerctrl.h"
#include <wx/filepicker.h>

namespace
{
    wxsRegisterItem<wxsDirPickerCtrl> Reg(_T("DirPickerCtrl"), wxsTWidget, _T("Advanced"), 120);

#ifdef __WXGTK__
    // GTK apps usually don't have a textctrl next to the picker
    WXS_ST_BEGIN(wxsDirPickerCtrlStyles,_T("wxDIRP_DIR_MUST_EXIST"))
#else
    WXS_ST_BEGIN(wxsDirPickerCtrlStyles,_T("wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST"))
#endif
        WXS_ST_CATEGORY("wxDirPickerCtrl")
        WXS_ST(wxDIRP_CHANGE_DIR)
        WXS_ST(wxDIRP_DIR_MUST_EXIST)
        WXS_ST(wxDIRP_USE_TEXTCTRL)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsDirPickerCtrlEvents)
        WXS_EVI(EVT_DIRPICKER_CHANGED, wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEvent , DirChanged)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsDirPickerCtrl::wxsDirPickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsDirPickerCtrlEvents,
        wxsDirPickerCtrlStyles),
        m_sMessage(wxDirSelectorPromptStr),
        m_sPath(wxEmptyString)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsDirPickerCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/filepicker.h>"),GetInfo().ClassName,0);
            Codef(_T("%C(%W, %I, %n, %t, %P, %S, %T, %V, %N);\n"), m_sPath.wx_str(), m_sMessage.wx_str());
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDirPickerCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsDirPickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxDirPickerCtrl* Preview;
    Preview = new wxDirPickerCtrl(Parent, GetId(), m_sPath, m_sMessage, Pos(Parent), Size(Parent), Style());
    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsDirPickerCtrl::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsDirPickerCtrl, m_sMessage, _("Message"), _T("message"), wxDirSelectorPromptStr, false)
    WXS_SHORT_STRING(wxsDirPickerCtrl, m_sPath, _("Path"), _T("path"), wxEmptyString, false)
}
