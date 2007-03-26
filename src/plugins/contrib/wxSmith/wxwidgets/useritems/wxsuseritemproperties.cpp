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

#include "wxsuseritemproperties.h"


wxsUserItemProperties::wxsUserItemProperties()
{
}

wxsUserItemProperties::~wxsUserItemProperties()
{
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        delete m_Descriptions[i];
    }
    m_Descriptions.Clear();
}

bool wxsUserItemProperties::SetDescription(int Number,const PropertyDescription* Desc)
{
    if ( Number<0 || Number>=GetDescriptionsCount() ) return false;
    *m_Descriptions[Number] = *Desc;
    return true;
}

bool wxsUserItemProperties::InsertDescription(int Position,const PropertyDescription* Desc)
{
    PropertyDescription* NewDesc = new PropertyDescription;
    *NewDesc = *Desc;

    if ( Position<0 || Position>=GetDescriptionsCount() )
    {
        m_Descriptions.Add(NewDesc);
        return true;
    }

    m_Descriptions.Insert(NewDesc,Position);
    return true;
}

bool wxsUserItemProperties::DeleteDescription(int Number)
{
    if ( Number<0 || Number>=GetDescriptionsCount() ) return false;

    delete m_Descriptions[Number];
    m_Descriptions.RemoveAt(Number);
    return true;
}

wxsGenericPropertyValueList* wxsUserItemProperties::BuildList()
{
    wxsGenericPropertyValueList* List = new wxsGenericPropertyValueList;
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        PropertyDescription* Desc = m_Descriptions[i];
        List->AddProperty(
            Desc->m_Type,
            Desc->m_Name,
            Desc->m_PropGridName,
            Desc->m_PropGridName2,
            Desc->m_PropGridName3,
            Desc->m_PropGridName4,
            Desc->m_DefaultValue);
    }
    return List;
}

void wxsUserItemProperties::ReadFromConfig(ConfigManager* Manager,const wxString& BasePath)
{
    // Clearing current content
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        delete m_Descriptions[i];
    }
    m_Descriptions.Clear();

    // Enumerating all nodes
    wxArrayString SubPaths = Manager->EnumerateSubPaths(BasePath);
    for ( size_t i=0; i<SubPaths.Count(); i++ )
    {
        // Skip empty enteries (should never happen)
        if ( SubPaths[i].IsEmpty() ) continue;

        // Skipping duplicated names
        if ( SubPaths.Index(SubPaths[i],false) < (int)i ) continue;

        wxString ItemPath = BasePath + _T("/") + SubPaths[i] + _T("/");
        PropertyDescription* Desc = new PropertyDescription;
        Desc->m_Type = (PropertyType)Manager->ReadInt(ItemPath+_T("Type"),wxsGenericPropertyValue::ptString);
        Desc->m_Name = SubPaths[i];
        Desc->m_PropGridName  = Manager->Read(ItemPath+_T("PGName1"),wxEmptyString);
        Desc->m_PropGridName2 = Manager->Read(ItemPath+_T("PGName2"),wxEmptyString);
        Desc->m_PropGridName3 = Manager->Read(ItemPath+_T("PGName3"),wxEmptyString);
        Desc->m_PropGridName4 = Manager->Read(ItemPath+_T("PGName4"),wxEmptyString);
        Desc->m_DefaultValue  = Manager->Read(ItemPath+_T("Default"),wxEmptyString);

        m_Descriptions.Add(Desc);
    }
}

void wxsUserItemProperties::WriteToConfig(ConfigManager* Manager,const wxString& BasePath)
{
    Manager->DeleteSubPath(BasePath);
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        PropertyDescription* Desc = m_Descriptions[i];
        wxString ItemPath = BasePath + _T("/") + Desc->m_Name;
        Manager->Write(ItemPath+_T("Type"),(int)Desc->m_Type);
        Manager->Write(ItemPath+_T("PGName1"),Desc->m_PropGridName);
        Manager->Write(ItemPath+_T("PGName2"),Desc->m_PropGridName2);
        Manager->Write(ItemPath+_T("PGName3"),Desc->m_PropGridName3);
        Manager->Write(ItemPath+_T("PGName4"),Desc->m_PropGridName4);
        Manager->Write(ItemPath+_T("Default"),Desc->m_DefaultValue);
    }
}
