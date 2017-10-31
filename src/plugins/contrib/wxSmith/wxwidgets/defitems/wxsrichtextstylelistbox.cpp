/** \file wxsrichtextstylelistbox.cpp
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
#include "wxsrichtextstylelistbox.h"

namespace
{
    wxsRegisterItem<wxsRichTextStyleListBox> Reg(_T("RichTextStyleListBox"),wxsTWidget,_T("Standard"),155);

    WXS_ST_BEGIN(wxsRichTextStyleListBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxsRichTextStyleListBox")
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsRichTextStyleListBox::wxsRichTextStyleListBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        wxsRichTextStyleListBoxStyles),
    m_sControl(wxEmptyString),
    m_sStyleSheet(wxEmptyString),
    m_iStyleType(wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH),
    m_bApplyOnSelection(false)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsRichTextStyleListBox::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/richtext/richtextstyles.h>"),GetInfo().ClassName,hfInPCH);

            Codef(_T("%C(%W, %I, %P, %S, %T);\n"));

            wxString sVarName = GetVarName();
            // Add the control and stylesheet setting calls at the bottom of the code, hopefully after the control and stylesheet have been declared.
            if (!m_sControl.IsEmpty()){
                AddEventCode(wxString::Format(_("// Set the wxRichtTextCtrl for %s. \n"), sVarName.wx_str()));
                AddEventCode(wxString::Format(wxT("%s->SetRichTextCtrl(%s);\n"), sVarName.wx_str(), m_sControl.wx_str()));
            }
            if (!m_sStyleSheet.IsEmpty()){
                AddEventCode(wxString::Format(_("// Set the wxRichTextStyleSheet for %s. \n"), sVarName.wx_str()));
                AddEventCode(wxString::Format(wxT("%s->SetStyleSheet(%s);\n"), sVarName.wx_str(), m_sStyleSheet.wx_str()));
                AddEventCode(wxString::Format(wxT("%s->UpdateStyles();\n"), sVarName.wx_str()));
            }

            // wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH is the default value.
            switch(m_iStyleType){
                case wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL:
                    Codef(_T("%ASetStyleType(%s);\n"), wxT("wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL"));
                    break;
                case wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER:
                    Codef(_T("%ASetStyleType(%s);\n"), wxT("wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER"));
                    break;
                case wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST:
                    Codef(_T("%ASetStyleType(%s);\n"), wxT("wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST"));
                    break;
                default:
                      break;
            }
            // Default is false.
            if (m_bApplyOnSelection){
                Codef( _T("%ASetApplyOnSelection(%b);\n"), m_bApplyOnSelection);
            }

            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsRichTextStyleListBox::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsRichTextStyleListBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxRichTextStyleListBox *Preview = new wxRichTextStyleListBox(Parent, GetId(), Pos(Parent), Size(Parent), Style());

//    if (!m_sControl.IsEmpty()){
//        Preview->SetRichTextCtrl(m_sControl);
//    }
//    if (!m_sStyleSheet.IsEmpty()){
//        Preview->SetStyleSheet(m_sStyleSheet);
//        Preview->UpdateStyles();
//    }

    // wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH is the default value.
    switch(m_iStyleType){
        case wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL:
            Preview->SetStyleType(wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL);
            break;
        case wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER:
            Preview->SetStyleType(wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER);
            break;
        case wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST:
            Preview->SetStyleType(wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST);
            break;
        default:
            break;
    }
    if (m_bApplyOnSelection){
        Preview->SetApplyOnSelection(m_bApplyOnSelection);
    }

    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsRichTextStyleListBox::OnEnumWidgetProperties(cb_unused long Flags)
{
    static const long StyleStates[] = {wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL,
                                                                wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER,
                                                                wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST,
                                                                wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH};
    static const wxChar* StyleNames[]  = {wxT("wxRICHTEXT_STYLE_ALL"), wxT("wxRICHTEXT_STYLE_CHARACTER"), wxT("wxRICHTEXT_STYLE_LIST"), wxT("wxRICHTEXT_STYLE_PARAGRAPH"), NULL};

    WXS_SHORT_STRING(wxsRichTextStyleListBox, m_sControl, _("Control"), _T("control"), wxEmptyString, true)
    WXS_SHORT_STRING(wxsRichTextStyleListBox, m_sStyleSheet, _("Style Sheet"), _T("style_sheet"), wxEmptyString, true)
    WXS_ENUM(wxsRichTextStyleListBox, m_iStyleType, _("Style Type"), _T("style_type"), StyleStates, StyleNames, wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH)
    WXS_BOOL(wxsRichTextStyleListBox, m_bApplyOnSelection, _("Apply On Selection"), _T("apply_on_selection"), false);
}
