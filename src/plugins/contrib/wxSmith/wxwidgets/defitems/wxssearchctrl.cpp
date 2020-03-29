/** \file wxssearchctrl.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010  Gary Harris
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

#include "wxssearchctrl.h"
#include "wx/srchctrl.h"


namespace
{
    wxsRegisterItem<wxsSearchCtrl> Reg(_T("SearchCtrl"), wxsTWidget, _T("Standard"), 130);

    WXS_ST_BEGIN(wxsSearchCtrlStyles, wxEmptyString)
        WXS_ST_CATEGORY("wxSearchCtrl")
        WXS_ST(wxTE_PROCESS_ENTER)
        WXS_ST(wxTE_PROCESS_TAB)
        WXS_ST(wxTE_NOHIDESEL)
        WXS_ST(wxTE_LEFT)
        WXS_ST(wxTE_CENTRE)
        WXS_ST(wxTE_RIGHT)
        WXS_ST(wxTE_CAPITALIZE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSearchCtrlEvents)
        WXS_EVI(EVT_TEXT, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEvent, Text)
        WXS_EVI(EVT_TEXT_ENTER, wxEVT_COMMAND_TEXT_ENTER, wxCommandEvent, TextEnter)
        WXS_EVI(EVT_SEARCHCTRL_SEARCH_BTN, wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEvent, SearchClicked)
        WXS_EVI(EVT_SEARCHCTRL_CANCEL_BTN, wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEvent, CancelClicked)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsSearchCtrl::wxsSearchCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSearchCtrlEvents,
        wxsSearchCtrlStyles),
        m_sValue(wxEmptyString),
        m_bShowSearchBtn(true),
        m_bShowCancelBtn(false)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsSearchCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/srchctrl.h>"),GetInfo().ClassName,hfInPCH);

            Codef(_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"), m_sValue.wx_str());

            if(!m_bShowSearchBtn){
                Codef(_T("%AShowSearchButton(%b);\n"), m_bShowSearchBtn);
            }
            if(m_bShowCancelBtn){
                Codef(_T("%AShowCancelButton(%b);\n"), m_bShowCancelBtn);
            }

            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSearchCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsSearchCtrl::OnBuildPreview(wxWindow* parent,long flags)
{
    wxSearchCtrl* preview = new wxSearchCtrl(parent, GetId(), m_sValue, Pos(parent), Size(parent), Style());

    if(!m_bShowSearchBtn){
        preview->ShowSearchButton(m_bShowSearchBtn);
    }
    if(m_bShowCancelBtn){
        preview->ShowCancelButton(m_bShowCancelBtn);
    }

    return SetupWindow(preview,flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsSearchCtrl::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsSearchCtrl, m_sValue, _("Value"), _T("value"), wxEmptyString, false)
    WXS_BOOL(wxsSearchCtrl, m_bShowSearchBtn, _("Search Button"), _T("search_button"), true)
    WXS_BOOL(wxsSearchCtrl, m_bShowCancelBtn, _("Cancel Button"), _T("cancel_button"), false)
}

