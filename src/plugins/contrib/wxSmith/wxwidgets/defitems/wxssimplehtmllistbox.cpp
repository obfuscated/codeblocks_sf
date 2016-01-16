/** \file wxssimplehtmllistbox.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2010 Gary Harris
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

#include <wx/htmllbox.h>
#include "wxssimplehtmllistbox.h"

namespace
{
    wxsRegisterItem<wxsSimpleHtmlListBox> Reg(_T("SimpleHtmlListBox"), wxsTWidget, _T("Standard"), 120);

    WXS_ST_BEGIN(wxsSimpleHtmlListBoxStyles, wxT("wxHLB_DEFAULT_STYLE"))
    WXS_ST_CATEGORY("wxSimpleHtmlListBox")
    WXS_ST(wxHLB_DEFAULT_STYLE)
    WXS_ST(wxHLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_HSCROLL)
    WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSimpleHtmlListBoxEvents)
    WXS_EVI(EVT_LISTBOX, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEvent, Select)
    WXS_EVI(EVT_LISTBOX_DCLICK, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEvent, DClick)
    WXS_EVI(EVT_HTML_CELL_CLICKED, wxEVT_COMMAND_HTML_CELL_CLICKED, wxHtmlCellEvent, CellClicked)
    WXS_EVI(EVT_HTML_CELL_HOVER, wxEVT_COMMAND_HTML_CELL_HOVER, wxHtmlCellEvent, CellHover)
    WXS_EVI(EVT_HTML_LINK_CLICKED, wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEvent, LinkClicked)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsSimpleHtmlListBox::wxsSimpleHtmlListBox(wxsItemResData *Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSimpleHtmlListBoxEvents,
        wxsSimpleHtmlListBoxStyles),
    DefaultSelection(-1)
{
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsSimpleHtmlListBox::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
            {
                AddHeader(_T("<wx/htmllbox.h>"), GetInfo().ClassName, hfInPCH);
                Codef(_T("%C(%W, %I, %P, %S, 0, 0, %T, %V, %N);\n"));
                for(size_t i = 0; i <  ArrayChoices.GetCount(); ++i)
                {
                    if(DefaultSelection == (int)i)
                    {
                        Codef(_T("%ASetSelection( "));
                    }
                    Codef(_T("%AAppend(%t)"), ArrayChoices[i].wx_str());
                    if(DefaultSelection == (int)i)
                    {
                        Codef(_T(" )"));
                    }
                    Codef(_T(";\n"));
                }

                BuildSetupWindowCode();
                return;
            }

        default:
            {
                wxsCodeMarks::Unknown(_T("wxsSimpleHtmlListBox::OnBuildCreatingCode"), GetLanguage());
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
wxObject *wxsSimpleHtmlListBox::OnBuildPreview(wxWindow *Parent, long Flags)
{
    wxSimpleHtmlListBox *Preview = new wxSimpleHtmlListBox(Parent, GetId(), Pos(Parent), Size(Parent), 0, 0, Style());
    for(size_t i = 0; i <  ArrayChoices.GetCount(); ++i)
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if((int)i == DefaultSelection)
        {
            Preview->SetSelection(Val);
        }
    }

    return SetupWindow(Preview, Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsSimpleHtmlListBox::OnEnumWidgetProperties(long Flags)
{
    WXS_ARRAYSTRING(wxsSimpleHtmlListBox, ArrayChoices, _("Choices"), _T("content"), _T("item"))
    WXS_LONG(wxsSimpleHtmlListBox, DefaultSelection, _("Default"), _T("default"), 0)
}
