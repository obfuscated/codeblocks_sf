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

#include "wxsresourcefactory.h"

// TODO: Support dynamic loading / unloading of factories

wxsResourceFactory* wxsResourceFactory::m_UpdateQueue = 0;
wxsResourceFactory* wxsResourceFactory::m_Initialized = 0;
wxsResourceFactory::HashT wxsResourceFactory::m_Hash;
wxString wxsResourceFactory::m_LastExternalName;
wxsResourceFactory* wxsResourceFactory::m_LastExternalFactory = 0;
bool wxsResourceFactory::m_AllAttached = false;

wxsResourceFactory::wxsResourceFactory()
{
    m_Next = m_UpdateQueue;
    m_UpdateQueue = this;
    m_Attached = false;
}

wxsResourceFactory::~wxsResourceFactory()
{
}

void wxsResourceFactory::InitializeFromQueue()
{
    while ( m_UpdateQueue )
    {
        wxsResourceFactory* NextFactory = m_UpdateQueue->m_Next;
        m_UpdateQueue->Initialize();
        m_UpdateQueue = NextFactory;
    }
}

inline void wxsResourceFactory::Initialize()
{
    for ( int i=OnGetCount(); i-->0; )
    {
        wxString Name;
        wxString GUI;
        OnGetInfo(i,Name,GUI);
        ResourceInfo& Info = m_Hash[Name];
        Info.m_Factory = this;
        Info.m_Number = i;
        Info.m_GUI = GUI;
        if ( Info.m_MenuId < 0 )
        {
            Info.m_MenuId = wxNewId();
        }
    }

    m_Next = m_Initialized;
    m_Initialized = this;

    if ( m_AllAttached )
    {
        OnAttach();
        m_Attached = true;
    }
}

wxsResource* wxsResourceFactory::Build(const wxString& ResourceType,wxsProject* Project)
{
    InitializeFromQueue();
    ResourceInfo& Info = m_Hash[ResourceType];
    if ( !Info.m_Factory )
    {
        return 0;
    }
    return Info.m_Factory->OnCreate(Info.m_Number,Project);
}

bool wxsResourceFactory::CanHandleExternal(const wxString& FileName)
{
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        if ( Factory->OnCanHandleExternal(FileName) )
        {
            m_LastExternalName = FileName;
            m_LastExternalFactory = Factory;
            return true;
        }
    }
    m_LastExternalName = wxEmptyString;
    m_LastExternalFactory = 0;
    return false;
}

wxsResource* wxsResourceFactory::BuildExternal(const wxString& FileName)
{
    InitializeFromQueue();
    if ( m_LastExternalFactory && (m_LastExternalName==FileName) )
    {
        return m_LastExternalFactory->OnBuildExternal(FileName);
    }
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        wxsResource* Res = Factory->OnBuildExternal(FileName);
        if ( Res ) return Res;
    }
    return 0;
}

void wxsResourceFactory::BuildSmithMenu(wxMenu* menu)
{
    InitializeFromQueue();
    for ( HashT::iterator i=m_Hash.begin(); i!=m_Hash.end(); ++i )
    {
        if ( i->second.m_Factory == 0 ) continue;
        wxString MenuEntry = _T("Add ") + i->first;
        menu->Append(i->second.m_MenuId,MenuEntry);
    }
}

bool wxsResourceFactory::NewResourceMenu(int Id,wxsProject* Project)
{
    for ( HashT::iterator i=m_Hash.begin(); i!=m_Hash.end(); ++i )
    {
        if ( i->second.m_Factory == 0 ) continue;
        if ( i->second.m_MenuId == Id )
        {
            return i->second.m_Factory->OnNewWizard(i->second.m_Number,Project);
        }
    }
    return false;
}

int wxsResourceFactory::ResourceTreeIcon(const wxString& ResourceType)
{
    InitializeFromQueue();
    ResourceInfo& Info = m_Hash[ResourceType];
    if ( !Info.m_Factory )
    {
        return -1;
    }
    return Info.m_Factory->OnResourceTreeIcon(Info.m_Number);
}

void wxsResourceFactory::OnAttachAll()
{
    if ( m_AllAttached ) return;
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory = Factory->m_Next )
    {
        if ( !Factory->m_Attached )
        {
            Factory->OnAttach();
            Factory->m_Attached = true;
        }
    }
    m_AllAttached = true;
}

void wxsResourceFactory::OnReleaseAll()
{
    if ( !m_AllAttached ) return;
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory = Factory->m_Next )
    {
        if ( Factory->m_Attached )
        {
            Factory->OnRelease();
            Factory->m_Attached = false;
        }
    }
    m_AllAttached = false;
}
