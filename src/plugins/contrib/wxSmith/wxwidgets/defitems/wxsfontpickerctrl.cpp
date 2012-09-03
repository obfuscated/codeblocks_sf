/**  \file wxsfontpickerctrl.cpp
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

#include "wxsfontpickerctrl.h"

#include <wx/fontpicker.h>

namespace
{
    wxsRegisterItem<wxsFontPickerCtrl> Reg(_T("FontPickerCtrl"), wxsTWidget, _T("Advanced"), 100);

    WXS_ST_BEGIN(wxsFontPickerCtrlStyles,_T("wxFNTP_FONTDESC_AS_LABEL|wxFNTP_USEFONT_FOR_LABEL"))
        WXS_ST_CATEGORY("wxFontPickerCtrl")
        WXS_ST(wxFNTP_FONTDESC_AS_LABEL)
        WXS_ST(wxFNTP_USEFONT_FOR_LABEL)
        WXS_ST(wxFNTP_USE_TEXTCTRL)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsFontPickerCtrlEvents)
        WXS_EVI(EVT_FONTPICKER_CHANGED, wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEvent, FontChanged)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsFontPickerCtrl::wxsFontPickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsFontPickerCtrlEvents,
        wxsFontPickerCtrlStyles)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsFontPickerCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/fontpicker.h>"),GetInfo().ClassName,0);

            wxString sFntName = GetCoderContext()->GetUniqueName(_T("PickerFont"));
            wxString sFnt = m_fdFont.BuildFontCode(sFntName, GetCoderContext());
            if(!sFnt.Len() > 0){
                sFntName = wxT("wxNullFont");
            }
            Codef(_T("%s"), sFnt.wx_str());
            Codef(_T("%C(%W, %I, %s, %P, %S, %T, %V, %N);\n"), sFntName.wx_str());
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFontPickerCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsFontPickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxFontPickerCtrl* Preview;
    wxFont fnt = m_fdFont.BuildFont();
    Preview = new wxFontPickerCtrl(Parent, GetId(), fnt, Pos(Parent), Size(Parent), Style());
    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsFontPickerCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_FONT(wxsFontPickerCtrl, m_fdFont, _("Font"), _T("font"))
}
