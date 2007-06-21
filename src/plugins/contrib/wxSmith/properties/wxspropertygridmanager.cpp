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

#include "wxspropertygridmanager.h"
#include "wxspropertycontainer.h"

IMPLEMENT_CLASS(wxsPropertyGridManager,wxPropertyGridManager)

wxsPropertyGridManager::wxsPropertyGridManager(
    wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxChar* name):
        wxPropertyGridManager(parent,id,pos,size,style,name),
        MainContainer(0)
{
    Singleton = this;
}

wxsPropertyGridManager::~wxsPropertyGridManager()
{
    PGIDs.Clear();
    PGEnteries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();
    ClearPage(0);
    PreviousIndex = -1;
    PreviousProperty = 0;
    if ( Singleton == this )
    {
        Singleton = 0;
    }
}

void wxsPropertyGridManager::OnChange(wxPropertyGridEvent& event)
{
    wxPGId ID = event.GetProperty();
    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        if ( PGIDs[i] == ID )
        {
            wxsPropertyContainer* Container = PGContainers[i];
            PGEnteries[i]->PGRead(Container,this,ID,PGIndexes[i]);

            // Notifying about property change
            Container->NotifyPropertyChangeFromPropertyGrid();

            // Notifying about sub property change
            if ( Container!=MainContainer && MainContainer!=0 )
            {
                MainContainer->OnSubPropertyChanged(Container);
            }
            Update(0);
            return;
        }
    }

    // Did not found changed id, it's time to say to container
    MainContainer->OnExtraPropertyChanged(this,ID);
}

void wxsPropertyGridManager::Update(wxsPropertyContainer* PC)
{
    if ( PC && PGContainersSet.find(PC) == PGContainersSet.end() )
    {
        // This container is not used here
        return;
    }

    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        PGEnteries[i]->PGWrite(PGContainers[i],this,PGIDs[i],PGIndexes[i]);
    }
}

void wxsPropertyGridManager::UnbindAll()
{
    // TODO: Remove all extra pages, leave only first one
    PGIDs.Clear();
    PGEnteries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();
    ClearPage(0);
    PreviousIndex = -1;
    PreviousProperty = 0;
    SetNewMainContainer(0);
}

void wxsPropertyGridManager::UnbindPropertyContainer(wxsPropertyContainer* PC)
{
    if ( PGContainersSet.find(PC) == PGContainersSet.end() )
    {
        // This container is not used here
        return;
    }

    if ( PC == MainContainer )
    {
        // Main container unbinds all
        UnbindAll();
        return;
    }

    Freeze();
    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        if ( PGContainers[i] == PC )
        {
            Delete(PGIDs[i]);
            PGIDs.RemoveAt(i);
            PGEnteries.RemoveAt(i);
            PGIndexes.RemoveAt(i);
            PGContainers.RemoveAt(i);
        }
    }
    Thaw();

    // If there are no properties, we have unbinded main property container
    if ( !PGIDs.Count() )
    {
        SetNewMainContainer(0);
    }
}

long wxsPropertyGridManager::Register(wxsPropertyContainer* Container,wxsProperty* Property,wxPGId Id,long Index)
{
    if ( !Property ) return -1;

    if ( Property != PreviousProperty )
    {
        PreviousIndex = -1;
    }

    if ( Index < 0 )
    {
        Index = ++PreviousIndex;
    }

    PGEnteries.Add(Property);
    PGIDs.Add(Id);
    PGIndexes.Add(Index);
    PGContainers.Add(Container);
    PGContainersSet.insert(Container);
    return Index;
}

void wxsPropertyGridManager::SetNewMainContainer(wxsPropertyContainer* Container)
{
    MainContainer = Container;
    OnContainerChanged(MainContainer);
}

wxsPropertyGridManager* wxsPropertyGridManager::Singleton = 0;

BEGIN_EVENT_TABLE(wxsPropertyGridManager,wxPropertyGridManager)
    EVT_PG_CHANGED(-1,wxsPropertyGridManager::OnChange)
END_EVENT_TABLE()
