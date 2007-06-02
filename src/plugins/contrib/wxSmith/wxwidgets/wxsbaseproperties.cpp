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
    static const int Priority = 50;

    if ( Flags & wxsItem::flPosition ) WXS_POSITION_P(wxsBaseProperties,m_Position,_("Default pos"),_("X"),_("Y"),_("Pos in dialog units"),_T("pos"),Priority);
    if ( Flags & wxsItem::flSize     ) WXS_SIZE_P    (wxsBaseProperties,m_Size,    _("Default size"),_("Width"),_("Height"),_("Size in dialog units"),_T("size"),Priority);
    if ( Flags & wxsItem::flEnabled  ) WXS_BOOL_P    (wxsBaseProperties,m_Enabled, _("Enabled"),_T("enabled"),true,Priority);
    if ( Flags & wxsItem::flFocused  ) WXS_BOOL_P    (wxsBaseProperties,m_Focused, _("Focused"),_T("focused"),false,Priority);
    if ( Flags & wxsItem::flHidden   ) WXS_BOOL_P    (wxsBaseProperties,m_Hidden,  _("Hidden"),_T("hidden"),false,Priority);
    if ( Flags & wxsItem::flColours  ) WXS_COLOUR_P  (wxsBaseProperties,m_Fg,      _("Foreground"),_T("fg"),Priority);
    if ( Flags & wxsItem::flColours  ) WXS_COLOUR_P  (wxsBaseProperties,m_Bg,      _("Background"),_T("bg"),Priority);
    if ( Flags & wxsItem::flFont     ) WXS_FONT_P    (wxsBaseProperties,m_Font,    _("Font"),_("font"),Priority);
    if ( Flags & wxsItem::flToolTip  ) WXS_STRING_P  (wxsBaseProperties,m_ToolTip, _("Tooltip"),_T("tooltip"),wxEmptyString,false,Priority);
    if ( Flags & wxsItem::flHelpText ) WXS_STRING_P  (wxsBaseProperties,m_HelpText,_("Help text"),_T("helptext"),wxEmptyString,false,Priority);

    if ( Flags & wxsItem::flSource )
    {
        if ( Flags & wxsItem::flMinMaxSize )
        {
            // Min / max size not available in XRC
            WXS_SIZE_P(wxsBaseProperties,m_MinSize,_("Default Min size"),_("Min Width"),_("Min Height"),_("Min size in dialog units"),_T("minsize"),Priority);
            WXS_SIZE_P(wxsBaseProperties,m_MaxSize,_("Default Max size"),_("Max Width"),_("Max Height"),_("Max size in dialog units"),_T("maxsize"),Priority);
        }

        if ( Flags & wxsItem::flExtraCode )
        {
            WXS_STRING_P(wxsBaseProperties,m_ExtraCode,_("Extra code"),_T("extra_code"), wxEmptyString,false,Priority);
        }
    }
}

void wxsBaseProperties::AddQPPChild(wxsAdvQPP* QPP,long Flags)
{
    // TODO: Add it
}
