/*
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

#include <wx/cshelp.h>
#include "wxscontexthelpbutton.h"

namespace
{
    wxsRegisterItem<wxsContextHelpButton> Reg(_T("ContextHelpButton"),wxsTWidget,_T("Standard"), 280);

    WXS_ST_BEGIN(wxsContextHelpButtonStyles, wxT("wxBU_AUTODRAW"))
        WXS_ST_CATEGORY("wxContextHelpButton")
         WXS_ST(wxBU_AUTODRAW)
         WXS_ST(wxBU_BOTTOM)
         WXS_ST(wxBU_LEFT)
         WXS_ST(wxBU_RIGHT)
         WXS_ST(wxBU_TOP)
         WXS_ST(wxBU_EXACTFIT)
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsContextHelpButton::wxsContextHelpButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        wxsContextHelpButtonStyles)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 * \note The control's ID must be wxID_CONTEXT_HELP.
 * \note A help provider must be created before this control and any controls that use context-sensitive help
 *  are declared. E.g. wxHelpProvider::Set(new wxSimpleHelpProvider);
 */
void wxsContextHelpButton::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/cshelp.h>"), GetInfo().ClassName, hfInPCH);

            Codef(_T("%C(%W, wxID_CONTEXT_HELP, %P, %S, %T);\n"));

            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsContextHelpButton::OnBuildCreatingCode"), GetLanguage());
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
wxObject* wxsContextHelpButton::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxContextHelpButton* Preview = new wxContextHelpButton(Parent, GetId(), Pos(Parent), Size(Parent), Style());

    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsContextHelpButton::OnEnumWidgetProperties(cb_unused long Flags)
{
}
