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

#include "wxswidget.h"

#include <messagemanager.h>

wxsWidget::wxsWidget(
    wxsItemResData* Data,
    const wxsItemInfo* Info,
    const wxsEventDesc* EventArray,
    const wxsStyleSet* StyleSet,
    long PropertiesFlags):
        wxsItem(Data,Info,PropertiesFlags,EventArray),
        m_StyleSet(StyleSet),
        m_StyleBits(0),
        m_ExStyleBits(0)
{
    if ( StyleSet )
    {
        m_StyleBits = StyleSet->GetDefaultBits(false);
        m_ExStyleBits = StyleSet->GetDefaultBits(true);
    }

}

void wxsWidget::OnEnumItemProperties(long Flags)
{
    OnEnumWidgetProperties(Flags);
    if ( m_StyleSet )
    {
        WXS_STYLE(wxsWidget,m_StyleBits,0,_("Style"),_T("style"),m_StyleSet);
        WXS_EXSTYLE(wxsWidget,m_ExStyleBits,0,_("Extra style"),_T("exstyle"),m_StyleSet);
    }
}

void wxsWidget::OnAddItemQPP(wxsAdvQPP* QPP)
{
    OnAddWidgetQPP(QPP);
}

wxWindow* wxsWidget::SetupWindow(wxWindow* Preview,long Flags)
{
    wxsItem::SetupWindow(Preview,Flags);
    long ExStyle = m_StyleSet ? m_StyleSet->GetWxStyle(m_ExStyleBits,true) : 0;
    if ( ExStyle != 0 )
    {
        Preview->SetExtraStyle(Preview->GetExtraStyle() | ExStyle);
    }
    return Preview;
}

void wxsWidget::SetupWindowCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            BuildSetupWindowCode(Code,WindowParent,Language);
            if ( m_ExStyleBits )
            {
                wxString ExStyleStr = m_StyleSet ? m_StyleSet->GetString(m_ExStyleBits,true,wxsCPP) : _T("0");
                if ( ExStyleStr != _T("0") )
                {
                    Code << GetAccessPrefix(wxsCPP) << _T("SetExtraStyle(") <<
                            GetAccessPrefix(wxsCPP) << _T("GetExtraStyle() | ") <<
                            ExStyleStr << _T(");\n");
                }
            }
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsWidget::SetupWindowCode"),Language);
        }
    }

}

bool wxsWidget::OnCodefExtension(wxsCodingLang Language,wxString& Result,const wxChar* &FmtChar,va_list ap)
{
    if ( *FmtChar == _T('T') )
    {
        Result << StyleCode(Language);
        FmtChar++;
        return true;
    }
    return wxsItem::OnCodefExtension(Language,Result,FmtChar,ap);
}
