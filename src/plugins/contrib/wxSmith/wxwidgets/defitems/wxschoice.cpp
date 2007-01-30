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

#include "wxschoice.h"

namespace
{
    wxsRegisterItem<wxsChoice> Reg(_T("Choice"),wxsTWidget,_T("Standard"),70);

    WXS_ST_BEGIN(wxsChoiceStyles,_T(""))
        WXS_ST_CATEGORY("wxChoice")
        WXS_ST(wxCB_SORT)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsChoiceEvents)
        WXS_EVI(EVT_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,wxCommandEvent,Select)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsChoice::wxsChoice(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsChoiceEvents,
        wxsChoiceStyles),
    DefaultSelection(-1)
{}


void wxsChoice::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%P,%S,0,NULL,%T,%V,%N);\n"));

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( DefaultSelection == (int)i )
                {
                    Code << Codef(Language,_T("%ASetSelection( "));
                }
                Code << Codef(Language,_T("%AAppend(%t)"),ArrayChoices[i].c_str());
                if ( DefaultSelection == (int)i )
                {
                    Code << _T(" )");
                }
                Code << _T(";\n");
            }

            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsChoice::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsChoice::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxChoice* Preview = new wxChoice(Parent,GetId(),Pos(Parent),Size(Parent),Style());

    for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( (int)i == DefaultSelection )
        {
            Preview->SetSelection(Val);
        }
    }
    return SetupWindow(Preview,Flags);
}


void wxsChoice::OnEnumWidgetProperties(long Flags)
{
    WXS_ARRAYSTRING(wxsChoice,ArrayChoices,0,_("Choices"),_T("content"),_T("item"))
    WXS_LONG(wxsChoice,DefaultSelection,0,_("Default"),_T("default"),0)
}

void wxsChoice::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/choice.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsChoice::OnEnumDeclFiles"),Language);
    }
}
