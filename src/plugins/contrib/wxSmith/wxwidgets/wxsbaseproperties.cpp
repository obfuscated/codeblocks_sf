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
}

void wxsBaseProperties::SetupWindow(wxWindow* Window,long Flags)
{
    bool IsExact = (Flags&wxsItem::pfExact) != 0;
    if ( !m_Enabled ) Window->Disable();
    if ( m_Focused  ) Window->SetFocus();
    if ( m_Hidden && IsExact ) Window->Hide();
    wxColour FGCol = m_Fg.GetColour();
    if ( FGCol.Ok() ) Window->SetForegroundColour(FGCol);
    wxColour BGCol = m_Bg.GetColour();
    if ( BGCol.Ok() ) Window->SetBackgroundColour(BGCol);
    wxFont FontVal = m_Font.BuildFont();
    if ( FontVal.Ok() ) Window->SetFont(FontVal);
    if ( !m_ToolTip.empty() ) Window->SetToolTip(m_ToolTip);
    if ( !m_HelpText.empty() ) Window->SetHelpText(m_HelpText);
}

void wxsBaseProperties::BuildSetupWindowCode(wxString& Code,const wxString& WindowName,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString VarAccess = WindowName.empty() ? _T("") : WindowName + _T("->");
            if ( !m_Enabled ) Code << VarAccess << _T("Disable();\n");
            if ( m_Focused  ) Code << VarAccess << _T("SetFocus();\n");
            if ( m_Hidden )   Code << VarAccess << _T("Hide();\n");

            wxString FGCol = m_Fg.BuildCode(wxsCPP);
            if ( !FGCol.empty() ) Code << VarAccess << _T("SetForegroundColour(") << FGCol << _T(");\n");

            wxString BGCol = m_Bg.BuildCode(wxsCPP);
            if ( !BGCol.empty() ) Code << VarAccess << _T("SetBackgroundColour(") << BGCol << _T(");\n");

            wxString FontVal = m_Font.BuildFontCode(WindowName + _T("Font"), wxsCPP);
            if ( !FontVal.empty() )
            {
                Code << FontVal;
                Code << VarAccess << _T("SetFont(") << WindowName << _T("Font);\n");
            }
            if ( !m_ToolTip.empty()  ) Code << VarAccess << _T("SetToolTip(") << wxsCodeMarks::WxString(wxsCPP,m_ToolTip) << _T(");\n");
            if ( !m_HelpText.empty() ) Code << VarAccess << _T("SetHelpText(") << wxsCodeMarks::WxString(wxsCPP,m_HelpText) << _T(");\n");
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsBaseProperties::BuildSetupWindowCode"),Language);
        }
    }

}

void wxsBaseProperties::AddQPPChild(wxsAdvQPP* QPP,long Flags)
{
    // TODO: Add it
}
