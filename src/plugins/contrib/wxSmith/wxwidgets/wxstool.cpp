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

#include "wxstool.h"

#include "wxsitemresdata.h"
#include <messagemanager.h>

wxsTool::wxsTool(
    wxsItemResData* Data,
    const wxsItemInfo* Info,
    const wxsEventDesc* EventArray,
    const wxsStyleSet* StyleSet,
    long PropertiesFlags):
        wxsParent(Data,Info,PropertiesFlags,EventArray),
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

void wxsTool::OnEnumItemProperties(long Flags)
{
    static const int Priority = 50;

    OnEnumToolProperties(Flags);
    if ( m_StyleSet )
    {
        WXS_STYLE_P(wxsTool,m_StyleBits,_("Style"),_T("style"),m_StyleSet,Priority);
        WXS_EXSTYLE_P(wxsTool,m_ExStyleBits,_("Extra style"),_T("exstyle"),m_StyleSet,Priority);
    }
}

void wxsTool::OnAddItemQPP(wxsAdvQPP* QPP)
{
    OnAddToolQPP(QPP);
}

bool wxsTool::OnCodefExtension(wxsCodingLang Language,wxString& Result,const wxChar* &FmtChar,va_list ap)
{
    if ( *FmtChar == _T('T') )
    {
        Result << StyleCode(Language);
        FmtChar++;
        return true;
    }
    return wxsItem::OnCodefExtension(Language,Result,FmtChar,ap);
}
