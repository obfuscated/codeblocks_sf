/*
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

#include <wx/defs.h> // to have wxUSE_CHECKLISTBOX defined in the checklst.h header
#include <wx/checklst.h>
#include "wxschecklistbox.h"

namespace
{
    wxsRegisterItem<wxsCheckListBox> Reg(_T("CheckListBox"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsCheckListBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxCheckListBox")
        WXS_ST(wxLB_SINGLE)
        WXS_ST(wxLB_MULTIPLE)
        WXS_ST(wxLB_EXTENDED)
        WXS_ST(wxLB_HSCROLL)
        WXS_ST(wxLB_ALWAYS_SB)
        WXS_ST(wxLB_NEEDED_SB)
        WXS_ST(wxLB_SORT)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsCheckListBoxEvents)
        WXS_EVI(EVT_CHECKLISTBOX,wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,wxCommandEvent,Toggled)
    WXS_EV_END()
}

wxsCheckListBox::wxsCheckListBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsCheckListBoxEvents,
        wxsCheckListBoxStyles)
{}


void wxsCheckListBox::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/checklst.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %P, %S, 0, 0, %T, %V, %N);\n"));

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( ArrayChecks[i] )
                {
                    Codef( _T("%ACheck("));
                }
                Codef( _T("%AAppend(%t)"), ArrayChoices[i].c_str());
                if ( ArrayChecks[i] )
                {
                    Codef(_T(")"));
                }
                Codef(_T(";\n"));
            }
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsCheckListBox::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsCheckListBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxCheckListBox* Preview = new wxCheckListBox(Parent,GetId(),Pos(Parent),Size(Parent),0,0,Style());
    for ( size_t i = 0; i < ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( ArrayChecks[i] )
        {
            Preview->Check(Val);
        }
    }
    return SetupWindow(Preview,Flags);
}

void wxsCheckListBox::OnEnumWidgetProperties(long Flags)
{
    WXS_ARRAYSTRINGCHECK(wxsCheckListBox,ArrayChoices,ArrayChecks,_("Choices"),_T("content"),_T("item"));
}
