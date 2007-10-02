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

#include "wxspropertycontainer.h"
#include "wxspropertygridmanager.h"

#include "wxsemptyproperty.h"
#include "wxslongproperty.h"
#include "wxsboolproperty.h"

#include <wx/msgdlg.h>
#include <globals.h>

long wxsPropertyContainer::Flags = 0;
bool wxsPropertyContainer::IsRead = false;
TiXmlElement* wxsPropertyContainer::CurrentElement = 0;
wxsPropertyStream* wxsPropertyContainer::CurrentStream = 0;
wxMutex wxsPropertyContainer::Mutex;

wxsPropertyContainer::wxsPropertyContainer():
    CurrentQP(0),
    BlockChangeCallback(false)
{
}

wxsPropertyContainer::~wxsPropertyContainer()
{
    // Unbinding from grid if there's one associated
    if ( wxsPGRID() )
    {
        wxsPGRID()->UnbindPropertyContainer(this);
    }

    // Unbinding prom quick properties if there's one
    if ( CurrentQP )
    {
        CurrentQP->Container = 0;
        CurrentQP = 0;
    }
}

void wxsPropertyContainer::ShowInPropertyGrid()
{
    wxMutexLocker Lock(Mutex);
    if ( !wxsPGRID() ) return;      // We're not sure that PropertyGridManager has been created
    Flags = (GetPropertiesFlags() & ~(flXml|flPropStream)) | flPropGrid;
    wxsPGRID()->Freeze();
    wxsPGRID()->NewPropertyContainerStart();
    OnEnumProperties(Flags);
    wxsPGRID()->NewPropertyContainerFinish(this);
    Flags = 0;
    OnAddExtraProperties(wxsPGRID());
    wxsPGRID()->Thaw();
}

void wxsPropertyContainer::XmlRead(TiXmlElement* Element)
{
    wxMutexLocker Lock(Mutex);
    // Just to make sure we will read exactly what we've saved
    TiXmlNode::SetCondenseWhiteSpace(false);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flPropStream)) | flXml;
    IsRead = true;
    CurrentElement = Element;
    OnEnumProperties(Flags);
    Flags = 0;
}

void wxsPropertyContainer::XmlWrite(TiXmlElement* Element)
{
    wxMutexLocker Lock(Mutex);
    // Just to make sure we will read exactly what we've saved
    TiXmlNode::SetCondenseWhiteSpace(false);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flPropStream)) | flXml;
    IsRead = false;
    CurrentElement = Element;
    OnEnumProperties(Flags);
    Flags = 0;
}

void wxsPropertyContainer::PropStreamRead(wxsPropertyStream* Stream)
{
    wxMutexLocker Lock(Mutex);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flXml)) | flPropStream;
    IsRead = true;
    CurrentStream = Stream;
    OnEnumProperties(Flags);
    NotifyPropertyChange(true);
    Flags = 0;
}

void wxsPropertyContainer::PropStreamWrite(wxsPropertyStream* Stream)
{
    wxMutexLocker Lock(Mutex);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flXml)) | flPropStream;
    IsRead = false;
    CurrentStream = Stream;
    OnEnumProperties(Flags);

    // Notifying about change since this method could correct some values
    NotifyPropertyChange(true);
    Flags = 0;
}

void wxsPropertyContainer::NotifyPropertyChangeFromPropertyGrid()
{
    if ( BlockChangeCallback ) return;
    BlockChangeCallback = true;

    if ( CurrentQP )
    {
        CurrentQP->Update();
    }

    OnPropertyChanged();

    BlockChangeCallback = false;
}

void wxsPropertyContainer::NotifyPropertyChangeFromQuickProps()
{
    if ( BlockChangeCallback ) return;
    BlockChangeCallback = true;

    if ( wxsPGRID() )
    {
        wxsPGRID()->Update(this);
    }

    OnPropertyChanged();

    BlockChangeCallback = false;
}

void wxsPropertyContainer::NotifyPropertyChange(bool CallPropertyChangeHandler)
{
    if ( BlockChangeCallback ) return;
    BlockChangeCallback = true;

    if ( CurrentQP )
    {
        CurrentQP->Update();
    }

    if ( wxsPGRID() )
    {
        wxsPGRID()->Update(this);
    }

    if ( CallPropertyChangeHandler )
    {
        OnPropertyChanged();
    }
    BlockChangeCallback = false;
}

wxsQuickPropsPanel* wxsPropertyContainer::BuildQuickPropertiesPanel(wxWindow* Parent)
{
    if ( CurrentQP )
    {
        CurrentQP->Container = 0;
    }

    CurrentQP = OnCreateQuickProperties(Parent);

    // CurrentQP MUST be currently associated to this container

    return CurrentQP;
}

void wxsPropertyContainer::Property(wxsProperty& Prop)
{
    // Property function works like kind of state machine, where current state
    // is determined by flPropGrid, flXml, flPropStream bits in Flags variable
    // and IsRead one. These are set up in one of public functions: XmlRead,
    // XmlWrite, PropStreamRead, PropStreamWrite, ShowInPropertyGrid
    switch ( Flags & (flPropGrid|flXml|flPropStream) )
    {
        case flPropGrid:
            // Called from ShowInPropertyGrid
            wxsPGRID()->NewPropertyContainerAddProperty(&Prop,this);
            break;

        case flXml:
            if ( IsRead )
            {
                // Called from XmlRead
                Prop.XmlRead(this,CurrentElement->FirstChildElement(cbU2C(Prop.GetDataName())));
            }
            else
            {
                // Called from XmlWrite
                TiXmlElement* Element = CurrentElement->InsertEndChild(
                    TiXmlElement(cbU2C(Prop.GetDataName())))->ToElement();

                if ( !Prop.XmlWrite(this,Element) )
                {
                    // Removing useless node, TiXml automatically frees memory
                    CurrentElement->RemoveChild(Element);
                }
            }
            break;

        case flPropStream:
            if ( IsRead )
            {
                // Called from PropStreamRead
                Prop.PropStreamRead(this,CurrentStream);
            }
            else
            {
                // Called from PropStreamWrite
                Prop.PropStreamWrite(this,CurrentStream);
            }
            break;

        default:

            // This can not be done
            wxMessageBox(_T("wxsPropertyContainer::Property() function has been\n")
                         _T("called manually. If you are the Developer,\n")
                         _T("please remove this code."));

    }
}

void wxsPropertyContainer::SubContainer(wxsPropertyContainer* Container,long NewFlags)
{
    if ( !Container ) return;
    long FlagsStore = Flags;
    // Flags will be replaced using NewFlags but bits used internally by wxsPropertyContainer will be left untouched
    Flags = ( Flags    &  (flPropGrid|flXml|flPropStream) ) |   // Leaving old part of data processing type (to be same as parent's one)
            ( NewFlags & ~(flPropGrid|flXml|flPropStream) );    // Rest taken from new properties
    Container->OnEnumProperties(NewFlags);
    Flags = FlagsStore;
}
