/**  \file wxscolourpickerctrl.cpp
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

#include "wxscolourpickerctrl.h"

#include <wx/clrpicker.h>

namespace
{
    wxsRegisterItem<wxsColourPickerCtrl> Reg(_T("ColourPickerCtrl"), wxsTWidget, _T("Advanced"), 140);

    WXS_ST_BEGIN(wxsColourPickerCtrlStyles, _T(""))
        WXS_ST_CATEGORY("wxColourPickerCtrl")
        WXS_ST(wxCLRP_SHOW_LABEL)
        WXS_ST(wxCLRP_USE_TEXTCTRL)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsColourPickerCtrlEvents)
        WXS_EVI(EVT_COLOURPICKER_CHANGED, wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEvent, ColourChanged)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsColourPickerCtrl::wxsColourPickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsColourPickerCtrlEvents,
        wxsColourPickerCtrlStyles),
        m_cdColour(*wxBLACK)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsColourPickerCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/clrpicker.h>"),GetInfo().ClassName,0);

            wxString ss = m_cdColour.BuildCode(GetCoderContext());
            Codef(_T("%C(%W, %I, %s, %P, %S, %T, %V, %N);\n"), ss.wx_str());

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsColourPickerCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsColourPickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxColourPickerCtrl* Preview;
    wxColour clr = m_cdColour.GetColour();
    if(clr.IsOk()){
        Preview = new wxColourPickerCtrl(Parent, GetId(), clr, Pos(Parent), Size(Parent), Style());
    }
    else{
        Preview = new wxColourPickerCtrl(Parent, GetId(), *wxBLACK, Pos(Parent), Size(Parent), Style());
    }
    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsColourPickerCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_COLOUR(wxsColourPickerCtrl, m_cdColour, _("Colour"), _T("colour"));
}
