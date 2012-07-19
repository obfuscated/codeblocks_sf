/**  \file wxsdatepickerctrl.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsdatepickerctrl.h"
#include <wx/datectrl.h>

namespace
{
    wxsRegisterItem<wxsDatePickerCtrl> Reg(_T("DatePickerCtrl"),wxsTWidget,_T("Advanced"),130);

    WXS_ST_BEGIN(wxsDatePickerCtrlStyles,_T("wxDP_DEFAULT|wxDP_SHOWCENTURY"))
        WXS_ST_CATEGORY("wxDatePickerCtrl")
        WXS_ST(wxDP_DEFAULT)
        WXS_ST(wxDP_SPIN)
        WXS_ST(wxDP_DROPDOWN)
        WXS_ST(wxDP_ALLOWNONE)
        WXS_ST(wxDP_SHOWCENTURY)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsDatePickerCtrlEvents)
        WXS_EVI(EVT_DATE_CHANGED,wxEVT_DATE_CHANGED,wxDateEvent,Changed)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsDatePickerCtrl::wxsDatePickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsDatePickerCtrlEvents,
        wxsDatePickerCtrlStyles)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsDatePickerCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/datectrl.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/dateevt.h>"),_T("wxDateEvent"),0);
            Codef(_T("%C(%W, %I, wxDefaultDateTime, %P, %S, %T, %V, %N);\n"));
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDatePickerCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsDatePickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxDatePickerCtrl* Preview = new wxDatePickerCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsDatePickerCtrl::OnEnumWidgetProperties(long Flags)
{
}
