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

#include "wxsbaseproperties.h"
#include "wxsitem.h"

void wxsBaseProperties::OnEnumProperties(long Flags)
{
    WXS_POSITION(wxsBaseProperties,m_Position,wxsItem::flPosition,_("Default pos"),_("X"),_("Y"),_("Pos in dialog units"),_T("pos"));
    WXS_SIZE    (wxsBaseProperties,m_Size,wxsItem::flSize,_("Default size"),_("Width"),_("Height"),_("Size in dialog units"),_T("size"));
    WXS_BOOL    (wxsBaseProperties,m_Enabled,wxsItem::flEnabled,_("Enabled"),_T("enabled"),true);
    WXS_BOOL    (wxsBaseProperties,m_Focused,wxsItem::flFocused,_("Focused"),_T("focused"),false);
    WXS_BOOL    (wxsBaseProperties,m_Hidden,wxsItem::flHidden,_("Hidden"),_T("hidden"),false);
    WXS_COLOUR  (wxsBaseProperties,m_Fg,wxsItem::flColours,_("Foreground"),_T("fg"));
    WXS_COLOUR  (wxsBaseProperties,m_Bg,wxsItem::flColours,_("Background"),_T("bg"));
    WXS_FONT    (wxsBaseProperties,m_Font,wxsItem::flFont,_("Font"),_("font"));
    WXS_STRING  (wxsBaseProperties,m_ToolTip,wxsItem::flToolTip,_("Tooltip"),_T("tooltip"),wxEmptyString,true,false);
    WXS_STRING  (wxsBaseProperties,m_HelpText,wxsItem::flHelpText,_("Help text"),_T("helptext"),wxEmptyString,true,false);

    if ( Flags & wxsItem::flSource )
    {
        // Min / max size not available in XRC
        WXS_SIZE    (wxsBaseProperties,m_MinSize,wxsItem::flMinMaxSize,_("Default Min size"),_("Min Width"),_("Min Height"),_("Min size in dialog units"),_T("minsize"));
        WXS_SIZE    (wxsBaseProperties,m_MaxSize,wxsItem::flMinMaxSize,_("Default Max size"),_("Max Width"),_("Max Height"),_("Max size in dialog units"),_T("maxsize"));
    }
}

void wxsBaseProperties::AddQPPChild(wxsAdvQPP* QPP,long Flags)
{
    // TODO: Add it
}
