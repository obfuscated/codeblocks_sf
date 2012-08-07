/** \file wxsrichtextstylecomboctrl.cpp
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

#include <wx/richtext/richtextstyles.h>
#include "wxsrichtextstylecomboctrl.h"

namespace
{
    wxsRegisterItem<wxsRichTextStyleComboCtrl> Reg(_T("RichTextStyleComboCtrl"),wxsTWidget,_T("Standard"),157);


    WXS_ST_BEGIN(wxsRichTextStyleComboCtrlStyles,_T(""))
        WXS_ST_CATEGORY("wxRichTextStyleComboCtrl")
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsRichTextStyleComboCtrl::wxsRichTextStyleComboCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        wxsRichTextStyleComboCtrlStyles),
    m_sControl(wxEmptyString),
    m_sStyleSheet(wxEmptyString)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsRichTextStyleComboCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/richtext/richtextstyles.h>"),GetInfo().ClassName,hfInPCH);

            Codef(_T("%C(%W, %I, %P, %S, %T);\n"));

            if(!m_sControl.IsEmpty()){
                Codef( _T("%ASetRichTextCtrl(%s);\n"), m_sControl.wx_str());
            }
            if(!m_sStyleSheet.IsEmpty()){
                Codef( _T("%ASetStyleSheet(%s);\n"), m_sStyleSheet.wx_str());
                Codef( _T("%AUpdateStyles();\n"));
            }

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsRichTextStyleComboCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief    Build the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                The control flags.
 * \return wxObject*                 The constructed control.
 *
 */
wxObject* wxsRichTextStyleComboCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxRichTextStyleComboCtrl* Preview = new wxRichTextStyleComboCtrl(Parent, GetId(), Pos(Parent), Size(Parent), Style());

    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsRichTextStyleComboCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_SHORT_STRING(wxsRichTextStyleComboCtrl, m_sControl, _("Control"), _T("control"), wxEmptyString, true)
    WXS_SHORT_STRING(wxsRichTextStyleComboCtrl, m_sStyleSheet, _("Style Sheet"), _T("style_sheet"), wxEmptyString, true)
}
