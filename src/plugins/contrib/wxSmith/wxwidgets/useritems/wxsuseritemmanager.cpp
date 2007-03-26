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

#include "wxsuseritemmanager.h"

wxsUserItemManager& wxsUserItemManager::Get()
{
    static wxsUserItemManager Singleton;
    return Singleton;
}


wxsUserItemManager::wxsUserItemManager()
{
}

wxsUserItemManager::~wxsUserItemManager()
{
    UninitializeUserItems();
}

void wxsUserItemManager::InitializeUserItems()
{
    // First deleting all items which may have been here
    UninitializeUserItems();

    // Now enumerating user items inside configuration node
    ConfigManager* Manager = Manager::Get()->GetConfigManager(_T("wxsmith"));

    // Now enumerating all configuration items
    wxArrayString UserItems = Manager->EnumerateSubPaths(_T("/useritems"));
    for ( size_t i=0; i<UserItems.Count(); i++ )
    {
        wxString UserItemName = UserItems[i];

        // Searching for given item inside item factory
        if ( wxsItemFactory::GetInfo(UserItemName) )
        {
            // This item is already registered and won't be added
            continue;
        }

        // Creating new user item description
        wxsUserItemDescription* Description = new wxsUserItemDescription(UserItemName);
        Description->ReadFromConfig(Manager,_T("/useritems/")+UserItemName);
        m_Descriptions.Add(Description);
    }
}

void wxsUserItemManager::UninitializeUserItems()
{
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        m_Descriptions[i]->DecReference(true);
    }
    m_Descriptions.Clear();
}

void wxsUserItemManager::StoreItemsInsideConfiguration()
{
    ConfigManager* Manager = Manager::Get()->GetConfigManager(_T("wxsmith"));

    // Need to remove all previous things inside configuration nodes
    Manager->DeleteSubPath(_T("/useritems"));

    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        m_Descriptions[i]->WriteToConfig(Manager,_T("/useritems/")+m_Descriptions[i]->GetName());
    }
}

int wxsUserItemManager::GetItemsCount()
{
    return (int)m_Descriptions.GetCount();
}

wxsUserItemDescription* wxsUserItemManager::GetDescription(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= GetItemsCount() ) return NULL;
    return m_Descriptions[Index];
}

void wxsUserItemManager::DeleteDescription(int Index)
{
    if ( Index < 0 ) return;
    if ( Index >= GetItemsCount() ) return;
    m_Descriptions[Index]->DecReference(true);
    m_Descriptions.RemoveAt(Index);
}

void wxsUserItemManager::UnregisterAll()
{
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        m_Descriptions[i]->Unregister();
    }
}

void wxsUserItemManager::ReregisterAll()
{
    for ( size_t i=0; i<m_Descriptions.Count(); i++ )
    {
        m_Descriptions[i]->Reregister();
    }
}

void wxsUserItemManager::AddDescription(wxsUserItemDescription* Description)
{
    m_Descriptions.Add(Description);
}
