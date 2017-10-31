/** \file wxscolourdialog.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#include "wxscolourdialog.h"
#include "../wxsitemresdata.h"
#include <wx/colordlg.h>

namespace
{
    wxsRegisterItem<wxsColourDialog> Reg(_T("ColourDialog"),wxsTTool,_T("Dialogs"),200,false);
}

wxsColourDialog::wxsColourDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info)
{
    m_ChooseFull = true;
}

void wxsColourDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/colordlg.h>"),GetInfo().ClassName,hfInPCH);

            bool Defaults = true;
            if ( !m_ChooseFull ) Defaults = false;
            if ( m_Colour.m_type!=wxsCOLOUR_DEFAULT ) Defaults = false;

            if ( !Defaults )
            {
                wxString ColourDataName = GetCoderContext()->GetUniqueName(_T("__ColourData"));

                Codef(_T("wxColourData %v;\n"),ColourDataName.wx_str());

                if ( !m_ChooseFull )
                {
                    Codef(_T("%v.SetChooseFull(%b);\n"),ColourDataName.wx_str(),m_ChooseFull);
                }

                if ( m_Colour.m_type != wxsCOLOUR_DEFAULT )
                {
                    Codef(_T("%v.SetColour(%l);\n"),ColourDataName.wx_str(),&m_Colour);
                }

                Codef(_T("%C(%W, &%v);\n"),ColourDataName.wx_str());
            }
            else
            {
                Codef(_T("%C(%W);\n"));
            }
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsColourDialog::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

void wxsColourDialog::OnEnumToolProperties(cb_unused long Flags)
{
    WXS_BOOL(wxsColourDialog,m_ChooseFull,_("Full dialog"),_T("choosefull"),true);
    WXS_COLOUR(wxsColourDialog,m_Colour,_("Colour"),_T("colour"));
}
