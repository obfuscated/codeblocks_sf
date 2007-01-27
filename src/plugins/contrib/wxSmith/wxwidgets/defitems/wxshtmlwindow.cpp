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

#include "wxshtmlwindow.h"
#include <wx/html/htmlwin.h>

namespace
{
    wxsRegisterItem<wxsHtmlWindow> Reg(_T("HtmlWindow"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsHtmlWindowStyles,_T("wxHW_SCROLLBAR_AUTO"))
        WXS_ST_CATEGORY("wxHtmlWindow")
        WXS_ST(wxHW_SCROLLBAR_NEVER)
        WXS_ST(wxHW_SCROLLBAR_AUTO)
        WXS_ST(wxHW_NO_SELECTION)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsHtmlWindowEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsHtmlWindow::wxsHtmlWindow(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsHtmlWindowEvents,
        wxsHtmlWindowStyles)
{
}

void wxsHtmlWindow::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( !IsRootItem() )
            {
                Code << GetVarName() << _T(" = new wxHtmlWindow(");
            }
            else
            {
                Code << _T("Create(");
            }

            Code << WindowParent << _T(",")
                 << GetIdName() << _T(",")
                 << PosCode(WindowParent,wxsCPP) << _T(",")
                 << SizeCode(WindowParent,wxsCPP) << _T(",")
                 << StyleCode(wxsCPP) << _T(",")
                 << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false) << _T(");\n");

            if ( Borders.Value )
            {
                Code << GetVarName() << _T("->SetBorders(") << Borders.GetPixelsCode(WindowParent,wxsCPP) << _T(");\n");
            }

            if ( !Url.empty() )
            {
                Code << GetVarName() << _T("->LoadPage(") << wxsCodeMarks::WxString(wxsCPP,Url,false) << _T(");\n");
            }
            else if ( !HtmlCode.empty() )
            {
                Code << GetVarName() << _T("->SetPage(") << wxsCodeMarks::WxString(wxsCPP,HtmlCode,false) << _T(");\n");
            }

            SetupWindowCode(Code,Language);
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsHtmlWindow::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsHtmlWindow::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxHtmlWindow* Preview = new wxHtmlWindow(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    if ( Borders.Value )
    {
        Preview->SetBorders(Borders.GetPixels(Parent));
    }
    if ( !Url.empty() )
    {
//        if ( Flags & pfExact )
//        {
//            Preview->LoadPage(Url);
//        }
//        else
        {
            Preview->SetPage(
                wxString(_T("<body><center>")) +
                _("Following url will be used:") +
                _T("<br>") + Url + _T("</center></body>"));
        }
    }
    else if ( !HtmlCode.empty() )
    {
        Preview->SetPage(HtmlCode);
    }

    return SetupWindow(Preview,Flags);
}

void wxsHtmlWindow::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsHtmlWindow,Url,0,_("Url"),_T("url"),_T(""),false,false)
    WXS_STRING(wxsHtmlWindow,HtmlCode,0,_("Html Code"),_T("htmlcode"),_T(""),true,false)
    WXS_DIMENSION(wxsHtmlWindow,Borders,0,_("Borders"),_("Borders in Dialog Units"),_("borders"),0,false)
}

void wxsHtmlWindow::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Decl.Add(_T("<wx/html/htmlwin.h>"));
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsHtmlWindow::OnEnumDeclFiles"),Language);
        }
    }
}
