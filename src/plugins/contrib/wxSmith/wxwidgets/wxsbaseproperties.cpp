/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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
#include "wxsflags.h"

#include <globals.h>

using namespace wxsFlags;

void wxsBaseProperties::OnEnumProperties(long Flags)
{
    static const int Priority1 = 60;
    static const int Priority2 = 50;

    bool NotXml    = ! ( Flags & flXml );
    bool NotRoot   = ! ( Flags & flRoot );
    bool Root      = ! NotRoot;
    bool NotSource = ! ( Flags & flSource );
    bool Source    = ! NotSource;
    bool Pointer   = !!( Flags & flPointer );
    bool VISFilter = NotXml && NotRoot;
    bool PSFilter  = Root && Source;

    if ( Flags & flVariable && VISFilter            ) WXS_SHORT_STRING_P (wxsBaseProperties,m_VarName,        _("Var name"),                                                                      _T("var_name"),      _T(""),true,  Priority1);
    if ( Flags & flVariable && VISFilter && Pointer ) WXS_BOOL_P         (wxsBaseProperties,m_IsMember,       _(" Is member"),                                                                    _T("var_is_member"), true,         Priority1);
    if ( Flags & flId       && VISFilter            ) WXS_SHORT_STRING_P (wxsBaseProperties,m_IdName,         _("Identifier"),                                                                    _T("identifier"),    _T(""),true,  Priority1);
    if ( Flags & flSubclass && VISFilter            ) WXS_SHORT_STRING_P (wxsBaseProperties,m_Subclass,       _("Class name"),                                                                    _T("subclass"),      _T(""),false, Priority1);
    if ( Flags & flPosition                         ) WXS_POSITION_P     (wxsBaseProperties,m_Position,       _("Default pos"),_("X"),_("Y"),_("Pos in dialog units"),                            _T("pos"),                         Priority2);
    if ( Flags & flSize                             ) WXS_SIZE_P         (wxsBaseProperties,m_Size,           _("Default size"),_("Width"),_("Height"),_("Size in dialog units"),                 _T("size"),                        Priority2);
    if ( Flags & flEnabled                          ) WXS_BOOL_P         (wxsBaseProperties,m_Enabled,        _("Enabled"),                                                                       _T("enabled"),       true,         Priority2);
    if ( Flags & flFocused                          ) WXS_BOOL_P         (wxsBaseProperties,m_Focused,        _("Focused"),                                                                       _T("focused"),       false,        Priority2);
    if ( Flags & flHidden                           ) WXS_BOOL_P         (wxsBaseProperties,m_Hidden,         _("Hidden"),                                                                        _T("hidden"),        false,        Priority2);
    if ( Flags & flColours                          ) WXS_COLOUR_P       (wxsBaseProperties,m_Fg,             _("Foreground"),                                                                    _T("fg"),                          Priority2);
    if ( Flags & flColours                          ) WXS_COLOUR_P       (wxsBaseProperties,m_Bg,             _("Background"),                                                                    _T("bg"),                          Priority2);
    if ( Flags & flFont                             ) WXS_FONT_P         (wxsBaseProperties,m_Font,           _("Font"),                                                                          _T("font"),                        Priority2);
    if ( Flags & flToolTip                          ) WXS_STRING_P       (wxsBaseProperties,m_ToolTip,        _("Tooltip"),                                                                       _T("tooltip"),       _T(""),false, Priority2);
    if ( Flags & flHelpText                         ) WXS_STRING_P       (wxsBaseProperties,m_HelpText,       _("Help text"),                                                                     _T("helptext"),      _T(""),false, Priority2);
    if ( Flags & flMinMaxSize && Source             ) WXS_SIZE_P         (wxsBaseProperties,m_MinSize,        _("Default Min size"),_("Min Width"),_("Min Height"),_("Min size in dialog units"), _T("minsize"),                     Priority2);
    if ( Flags & flMinMaxSize && Source             ) WXS_SIZE_P         (wxsBaseProperties,m_MaxSize,        _("Default Max size"),_("Max Width"),_("Max Height"),_("Max size in dialog units"), _T("maxsize"),                     Priority2);
    if ( Flags & flExtraCode  && Source             ) WXS_STRING_P       (wxsBaseProperties,m_ExtraCode,      _("Extra code"),                                                                    _T("extra_code"),    _T(""),false, Priority2);
    if (                         PSFilter           ) WXS_BOOL_P         (wxsBaseProperties,m_ParentFromArg,  _("Use parent from argument"),                                                      _T("parent_arg"),    true,         Priority2);
    if ( Flags & flId         && PSFilter           ) WXS_BOOL_P         (wxsBaseProperties,m_IdFromArg,      _("Use ID from argument"),                                                          _T("id_arg"),        true,         Priority2);
    if ( Flags & flPosition   && PSFilter           ) WXS_BOOL_P         (wxsBaseProperties,m_PositionFromArg,_("Use pos from argument"),                                                         _T("pos_arg"),       false,        Priority2);
    if ( Flags & flSize       && PSFilter           ) WXS_BOOL_P         (wxsBaseProperties,m_SizeFromArg,    _("Use size from argument"),                                                        _T("size_arg"),      false,        Priority2);
    if ( m_StyleSet                                 ) WXS_STYLE_P        (wxsBaseProperties,m_StyleBits,      _("Style"),                                                                         _T("style"),         m_StyleSet,   Priority2);
    if ( m_StyleSet                                 ) WXS_EXSTYLE_P      (wxsBaseProperties,m_ExStyleBits,    _("Extra style"),                                                                   _T("exstyle"),       m_StyleSet,   Priority2);
}

void wxsBaseProperties::AddQPPChild(wxsAdvQPP* QPP,long Flags)
{
    // TODO: Add it
}

void wxsBaseProperties::SpecialXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        m_IdName = cbC2U(Element->Attribute("name"));
        m_Subclass = cbC2U(Element->Attribute("subclass"));
    }

    if ( IsExtra )
    {
        m_VarName = cbC2U(Element->Attribute("variable"));
        const char* MbrText = Element->Attribute("member");
        m_IsMember = !MbrText || !strcmp(MbrText,"yes");
    }
}

void wxsBaseProperties::SpecialXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra,long Flags,const wxString& ClassName)
{
    if ( !(Flags & flRoot) )
    {
        if ( IsXRC )
        {
            if ( Flags & flId )
            {
                Element->SetAttribute("name",cbU2C(m_IdName));
            }
            if ( Flags & flSubclass )
            {
                if ( !m_Subclass.IsEmpty() && (m_Subclass!=ClassName) )
                {
                    Element->SetAttribute("subclass",cbU2C(m_Subclass));
                }
            }
        }

        if ( IsExtra )
        {
            if ( Flags & flVariable )
            {
                Element->SetAttribute("variable",cbU2C(m_VarName));
                Element->SetAttribute("member",m_IsMember ? "yes" : "no" );
            }
        }
    }
}

