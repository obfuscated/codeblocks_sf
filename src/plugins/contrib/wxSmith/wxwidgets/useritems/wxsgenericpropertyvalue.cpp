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

#include "wxsgenericpropertyvalue.h"


wxsGenericPropertyValue::wxsGenericPropertyValue(
    PropertyType Type,
    const wxString& Name,
    const wxString& PropertyName,
    const wxString& PropertyName2,
    const wxString& PropertyName3,
    const wxString& PropertyName4):
        m_Type(Type),
        m_Name(Name),
        m_PropertyName(PropertyName),
        m_PropertyName2(PropertyName2),
        m_PropertyName3(PropertyName3),
        m_PropertyName4(PropertyName4)
{
}

wxString wxsGenericPropertyValue::GetValueAsString(wxsCodingLang Language,const wxString& WindowParent)
{
    switch ( Language )
    {
        case wxsCPP:
            switch ( m_Type )
            {
                case ptString:
                case ptLongString:  return wxsCodeMarks::WxString(Language,m_String,true);
                case ptLong:        return wxString::Format(_T("%d"),m_Long);
                case ptBool:        return m_Bool ? _T("true") : _T("false");
                case ptBitmap:      return m_Bitmap.BuildCode(true,_T("wxDefaultSize"),Language);
                case ptColour:      return m_Colour.BuildCode(Language);
                case ptDimension:   return m_Dimension.GetPixelsCode(WindowParent,Language);
                case ptPosition:    return m_PositionSize.GetPositionCode(WindowParent,Language);
                case ptSize:        return m_PositionSize.GetSizeCode(WindowParent,Language);
            }
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsGenericPropertyValue::GetValueAsString"),Language);
    }

    return wxEmptyString;
}

void wxsGenericPropertyValue::SetValueFromString(const wxString& String)
{
    switch ( m_Type )
    {
        case ptString:
        case ptLongString:
            m_String = String;
            break;

        case ptLong:
            String.ToLong(&m_Long);
            break;

        case ptBool:
            m_Bool = String == _T("true");
            break;

        case ptBitmap:
            m_Bitmap.Id.Clear();
            m_Bitmap.Client.Clear();
            m_Bitmap.FileName.Clear();
            break;

        case ptColour:
            m_Colour.m_type = wxsCOLOUR_DEFAULT;
            break;

        case ptDimension:
            m_Dimension.DialogUnits = false;
            String.ToLong(&m_Dimension.Value);
            break;

        case ptPosition:
        case ptSize:
            if ( String.IsEmpty() )
            {
                m_PositionSize.IsDefault = true;
                m_PositionSize.DialogUnits = false;
                m_PositionSize.X = 0;
                m_PositionSize.Y = 0;
            }
            else
            {
                m_PositionSize.IsDefault = false;
                m_PositionSize.DialogUnits = false;
                String.BeforeFirst(_T(',')).ToLong(&m_PositionSize.X);
                String.AfterLast(_T(',')).ToLong(&m_PositionSize.Y);
            }
            break;
    }
}

void wxsGenericPropertyValue::OnEnumProperties(long Flags)
{
    switch ( m_Type )
    {
        case ptString:
        {
            wxsStringProperty Prop(m_PropertyName,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_String),false,true,wxEmptyString);
            Property(Prop,0);
            break;
        }

        case ptLongString:
        {
            wxsStringProperty Prop(m_PropertyName,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_String),true,true,wxEmptyString);
            Property(Prop,0);
            break;
        }

        case ptLong:
        {
            wxsLongProperty Prop(m_PropertyName,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_Long));
            Property(Prop,0);
            break;
        }

        case ptBool:
        {
            wxsBoolProperty Prop(m_PropertyName,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_Bool));
            Property(Prop,0);
            break;
        }

        case ptBitmap:
        {
            wxsBitmapProperty Prop(m_PropertyName,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_Bitmap));
            Property(Prop,0);
            break;
        }

        case ptColour:
        {
            wxsColourProperty Prop(m_PropertyName,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_Colour));
            Property(Prop,0);
            break;
        }

        case ptDimension:
        {
            wxsDimensionProperty Prop(m_PropertyName,m_PropertyName2,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_Dimension));
            Property(Prop,0);
            break;
        }

        case ptPosition:
        {
            wxsPositionProperty Prop(m_PropertyName,m_PropertyName2,m_PropertyName3,m_PropertyName4,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_PositionSize));
            Property(Prop,0);
            break;
        }

        case ptSize:
        {
            wxsSizeProperty Prop(m_PropertyName,m_PropertyName2,m_PropertyName3,m_PropertyName4,m_Name,wxsOFFSET(wxsGenericPropertyValue,m_PositionSize));
            Property(Prop,0);
            break;
        }
    }
}
