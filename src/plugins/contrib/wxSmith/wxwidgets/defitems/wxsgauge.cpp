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

#include <wx/gauge.h>
#include "wxsgauge.h"

namespace
{
    wxsRegisterItem<wxsGauge> Reg(_T("Gauge"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsGaugeStyles,_T(""))
        WXS_ST(wxGA_HORIZONTAL)
        WXS_ST(wxGA_VERTICAL)
        WXS_ST(wxGA_SMOOTH)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsGaugeEvents)
    WXS_EV_END()
}

wxsGauge::wxsGauge(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGaugeEvents,
        wxsGaugeStyles),
    Range(100),
    Value(0),
    Shadow(0),
    Bezel(0)
{}



void wxsGauge::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W, %I, %d, %P, %S, %T, %V, %N);\n"),Range);
            if ( Value )  Code << Codef(Language,_T("%ASetValue(%d);\n"),Value);
            if ( Shadow ) Code << Codef(Language,_T("%ASetShadowWidth(%d);\n"),Shadow);
            if ( Bezel )  Code << Codef(Language,_T("%ASetBezelFace(%d);\n"),Bezel);
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGauge::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsGauge::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxGauge* Preview = new wxGauge(Parent,GetId(),Range,Pos(Parent),Size(Parent),Style());
    if ( Value )  Preview->SetValue(Value);
    if ( Shadow ) Preview->SetShadowWidth(Shadow);
    if ( Bezel )  Preview->SetBezelFace(Bezel);
    return SetupWindow(Preview,Flags);
}


void wxsGauge::OnEnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsGauge,Value,_("Value"),_T("value"),0)
    WXS_LONG(wxsGauge,Range,_("Range"),_T("range"),100)
    WXS_LONG(wxsGauge,Shadow,_("3D Shadow Width"),_T("shadow"),0)
    WXS_LONG(wxsGauge,Bezel,_("Bezel Face Width"),_T("bezel"),0)
}

void wxsGauge::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/gauge.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsGauge::OnEnumDeclFiles"),Language);
    }
}
