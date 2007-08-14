/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <wx/radiobox.h>
#include <wx/window.h> // GetParent
#include "wxsradiobox.h"

namespace
{
    wxsRegisterItem<wxsRadioBox> Reg(_T("RadioBox"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsRadioBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxRadioBox")
        WXS_ST(wxRA_SPECIFY_COLS)
        WXS_ST(wxRA_HORIZONTAL)
        WXS_ST(wxRA_SPECIFY_ROWS)
        WXS_ST(wxRA_VERTICAL)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsRadioBoxEvents)
        WXS_EVI(EVT_RADIOBOX,wxEVT_COMMAND_RADIOBOX_SELECTED,wxCommandEvent,Select)
    WXS_EV_END()
}

wxsRadioBox::wxsRadioBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsRadioBoxEvents,
        wxsRadioBoxStyles),
    Label(_("Label")),
    DefaultSelection(-1),
    Dimension(1)
{}


void wxsRadioBox::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            // wxRadioBox does not have Append Function , therefore , have to build a wxString[]
            // to pass in to the ctor
            if ( ArrayChoices.GetCount() > 0 )
            {
                Code<< _T("wxString wxRadioBoxChoices_") << GetVarName()
                    << _T("[") << wxString::Format(_T("%d"),ArrayChoices.GetCount()) << _T("] = \n{\n");
                for ( size_t i = 0; i < ArrayChoices.GetCount(); ++i )
                {
                    Code << _T("\t") << wxsCodeMarks::WxString(wxsCPP,ArrayChoices[i]);
                    if ( i != ArrayChoices.GetCount()-1 ) Code << _T(",");
                    Code << _T("\n");
                }
                Code << _T("};\n");
            }

            if ( Dimension < 1 ) Dimension = 1;

            Code << Codef(Language,_T("%C(%W, %I, %t, %P, %S, %d, %s, %d, %T, %V, %N);\n"),
                        Label.c_str(),ArrayChoices.GetCount(),
                        (ArrayChoices.IsEmpty()?_T("0"):(_T("wxRadioBoxChoices_")+GetVarName()).c_str()),
                        Dimension);

            if ( DefaultSelection >= 0 && DefaultSelection < (int)ArrayChoices.GetCount() )
            {
                Code << Codef(Language, _T("%ASetSelection(%d);\n"), DefaultSelection);
            }
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsRadioBox::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsRadioBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxRadioBox* Preview = new wxRadioBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),ArrayChoices, Dimension, Style());
    if ( DefaultSelection >= 0 && DefaultSelection < (int)ArrayChoices.GetCount() )
    {
        Preview->SetSelection(DefaultSelection);
    }
    return SetupWindow(Preview,Flags);
}

void wxsRadioBox::OnEnumWidgetProperties(long Flags)
{
    WXS_SHORT_STRING(wxsRadioBox,Label,_("Label"),_T("label"),_T(""),true)
    WXS_ARRAYSTRING(wxsRadioBox,ArrayChoices,_("Choices"),_T("content"),_T("item"))
    WXS_LONG(wxsRadioBox,DefaultSelection,_("Default"),_T("default"),0)
    WXS_LONG(wxsRadioBox,Dimension,_("Dimension"),_T("dimension"),1)

}

void wxsRadioBox::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/radiobox.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsRadioBox::OnEnumDeclFiles"),Language);
    }
}
