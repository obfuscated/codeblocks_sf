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

#include "wxsgenericpropertyvaluelist.h"


wxsGenericPropertyValueList::wxsGenericPropertyValueList()
{
}

wxsGenericPropertyValueList::~wxsGenericPropertyValueList()
{
    for ( size_t i=0; i<m_Properties.Count(); i++ )
    {
        delete m_Properties[i];
    }
    m_Properties.Clear();
}

void wxsGenericPropertyValueList::AddProperty(
            wxsGenericPropertyValue::PropertyType Type,
            const wxString& Name,
            const wxString& PropertyName1,
            const wxString& PropertyName2,
            const wxString& PropertyName3,
            const wxString& PropertyName4,
            const wxString& InitialValue)
{
    wxsGenericPropertyValue* NewValue = new wxsGenericPropertyValue(Type,Name,
        PropertyName1,PropertyName2,PropertyName3,PropertyName4);
    NewValue->SetValueFromString(InitialValue);
    m_Properties.Add(NewValue);
}

void wxsGenericPropertyValueList::OnEnumProperties(long Flags)
{
    for ( size_t i=0; i<m_Properties.Count(); i++ )
    {
        SubContainer(m_Properties[i]);
    }
}

bool wxsGenericPropertyValueList::GetPropertyValue(const wxString& Name,wxString& Value,wxsCodingLang Language,const wxString& WindowParent)
{
    for ( size_t i=0; i<m_Properties.Count(); i++ )
    {
        if ( Name.CmpNoCase(m_Properties[i]->GetName()) )
        {
            Value = m_Properties[i]->GetValueAsString(Language,WindowParent);
            return true;
        }
    }
    return false;
}
