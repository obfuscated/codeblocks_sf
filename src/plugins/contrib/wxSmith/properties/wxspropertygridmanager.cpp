/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxspropertygridmanager.h"
#include "wxspropertycontainer.h"

#include <manager.h>
#include <logmanager.h>

IMPLEMENT_CLASS(wxsPropertyGridManager,wxPropertyGridManager)

wxsPropertyGridManager::wxsPropertyGridManager(
    wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    #if wxCHECK_VERSION(2, 9, 0)
    const char* name):
    #else
    const wxChar* name):
    #endif
        wxPropertyGridManager(parent,id,pos,size,style,name),
        MainContainer(0)
{
    Singleton = this;
    PropertiesList = 0;
}

wxsPropertyGridManager::~wxsPropertyGridManager()
{
    PGIDs.Clear();
    PGEnteries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();
    for ( size_t i=0; i<GetPageCount(); i++ )
    {
        ClearPage(i);
    }
    PreviousIndex = -1;
    PreviousProperty = 0;
    if ( Singleton == this )
    {
        Singleton = 0;
    }
    DeleteTemporaryPropertiesList();
}

void wxsPropertyGridManager::OnChange(wxPropertyGridEvent& event)
{
    wxPGId ID = event.GetProperty();
    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        if ( PGIDs[i] == ID )
        {
            wxsPropertyContainer* Container = PGContainers[i];
            if ( !PGEnteries[i]->PGRead(Container,this,ID,PGIndexes[i]) )
            {
                wxString ErrorMsg;
                ErrorMsg << _T("wxSmith: Couldn't read value from wxsPropertyGridManager")
                         << _T(", propgrid name=") << PGEnteries[i]->GetPGName()
                         << _T(", date name=")     << PGEnteries[i]->GetDataName()
                         << _T(", type name=")     << PGEnteries[i]->GetTypeName();
                Manager::Get()->GetLogManager()->DebugLogError(ErrorMsg);
            }

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
    for ( size_t i=0; i<GetPageCount(); i++ )
    {
        ClearPage(i);
    }
    PreviousIndex = -1;
    PreviousProperty = 0;
    SetNewMainContainer(0);
}

void wxsPropertyGridManager::UnbindPropertyContainer(wxsPropertyContainer* PC, bool doFreeze)
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

    if(doFreeze)
        Freeze();
    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        if ( PGContainers[i] == PC )
        {
            #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
            DeleteProperty(PGIDs[i]);
            #else
            Delete(PGIDs[i]);
            #endif
            PGIDs.RemoveAt(i);
            PGEnteries.RemoveAt(i);
            PGIndexes.RemoveAt(i);
            PGContainers.RemoveAt(i);
        }
    }
    // in some cases the Thaw leads to a crash, so we have to disable Freeze-Thaw until we find
    // another workaround or are sure, that we do not need it.
    if(doFreeze)
        Thaw();

    // If there are no properties, we have unbinded main property container
    if ( !PGIDs.Count() )
    {
        for ( size_t i=0; i<GetPageCount(); i++ )
        {
            ClearPage(i);
        }
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

void wxsPropertyGridManager::NewPropertyContainerStart()
{
    UnbindAll();
    DeleteTemporaryPropertiesList();
}

void wxsPropertyGridManager::NewPropertyContainerAddProperty(wxsProperty* Property,wxsPropertyContainer* Container)
{
    TemporaryPropertiesList* NewItem = new TemporaryPropertiesList;
    NewItem->Property = Property;
    NewItem->Container = Container;
    NewItem->Priority = Property->GetPriority();
    int Priority = NewItem->Priority;

    TemporaryPropertiesList *Prev = 0, *Search;
    for ( Search = PropertiesList; Search && Search->Property->GetPriority() >= Priority; Prev = Search, Search = Search->Next );

    NewItem->Next = Search;
    ( Prev ? Prev->Next : PropertiesList ) = NewItem;
}

void wxsPropertyGridManager::NewPropertyContainerFinish(wxsPropertyContainer* Container)
{
    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    SelectPage(0);
    #else
    SetTargetPage(0);
    #endif

    while ( PropertiesList )
    {
        TemporaryPropertiesList* Next = PropertiesList->Next;
        #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
        PropertiesList->Property->PGCreate(PropertiesList->Container,this,GetGrid()->GetRoot());
        #else
        PropertiesList->Property->PGCreate(PropertiesList->Container,this,GetRoot());
        #endif
        delete PropertiesList;
        PropertiesList = Next;
    }

    SetNewMainContainer(Container);
}

void wxsPropertyGridManager::SetNewMainContainer(wxsPropertyContainer* Container)
{
    MainContainer = Container;
    OnContainerChanged(MainContainer);
}

void wxsPropertyGridManager::DeleteTemporaryPropertiesList()
{
    while ( PropertiesList )
    {
        TemporaryPropertiesList* Next = PropertiesList->Next;
        delete PropertiesList;
        PropertiesList = Next;
    }
}

void wxsPropertyGridManager::StoreSelected(SelectionData* Data)
{
    if ( !Data )
    {
        Data = &LastSelection;
    }

    Data->m_PageIndex = GetSelectedPage();

    #if wxCHECK_VERSION(2, 9, 0)
    wxPGId Selected = GetSelection();
    #else
    wxPGId Selected = GetSelectedProperty();
    #endif
    if ( Selected != NULL )
    {
        Data->m_PropertyName = GetPropertyName(Selected);
    }
    else
    {
        Data->m_PropertyName.Clear();
    }
}

void wxsPropertyGridManager::RestoreSelected(const SelectionData* Data)
{
    if ( !Data )
    {
        Data = &LastSelection;
    }

    if ( Data->m_PageIndex < 0 ) return;
    if ( Data->m_PageIndex >= (int)GetPageCount() ) return;
    if ( Data->m_PropertyName.IsEmpty() ) return;

    SelectPage(Data->m_PageIndex);
    SelectProperty(Data->m_PropertyName);
}


wxsPropertyGridManager* wxsPropertyGridManager::Singleton = 0;

BEGIN_EVENT_TABLE(wxsPropertyGridManager,wxPropertyGridManager)
    EVT_PG_CHANGED(-1,wxsPropertyGridManager::OnChange)
    EVT_PG_DOUBLE_CLICK(-1,wxsPropertyGridManager::OnChange)
END_EVENT_TABLE()
