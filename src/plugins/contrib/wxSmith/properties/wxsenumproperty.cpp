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

#include "wxsenumproperty.h"

#include <globals.h>
#include <prep.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,long)

wxsEnumProperty::wxsEnumProperty(const wxString& PGName, const wxString& DataName,long _Offset,const long* _Values,const wxChar** _Names,bool _UpdateEntries,long _Default,bool _UseNamesInXml,int Priority):
    wxsProperty(PGName,DataName,Priority),
    Offset(_Offset),
    Default(_Default),
    UpdateEntries(_UpdateEntries),
    Values(_Values),
    Names(_Names),
    UseNamesInXml(_UseNamesInXml)
{}


void wxsEnumProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGChoices PGC(Names,Values);
    PGRegister(Object,Grid,Grid->AppendIn(Parent,NEW_IN_WXPG14X wxEnumProperty(GetPGName(),wxPG_LABEL,PGC,VALUE)));
}

bool wxsEnumProperty::PGRead(cb_unused wxsPropertyContainer* Object,
                             wxPropertyGridManager* Grid,wxPGId Id,
                             cb_unused long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsEnumProperty::PGWrite(wxsPropertyContainer* Object, wxPropertyGridManager* Grid,
                              wxPGId Id, cb_unused long Index)
{
    if ( UpdateEntries )
    {
        #if wxCHECK_VERSION(2, 9, 0)
        wxPGChoices(Id->GetChoices()).Set(Names,Values);
        #else
        Grid->GetPropertyChoices(Id).Set(Names,Values);
        #endif
    }
    Grid->SetPropertyValue(Id,VALUE);
    return true;
}

bool wxsEnumProperty::XmlRead(cb_unused wxsPropertyContainer* Object,
                              TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE = Default;
        return false;
    }
    const char* Text = Element->GetText();
    if ( !Text )
    {
        VALUE = Default;
        return false;
    }
    if ( UseNamesInXml )
    {
        // Searching for node text in names
        wxString TextS = cbC2U(Text);
        int i = 0;
        for ( const wxChar** Ptr = Names; *Ptr; Ptr++, i++ )
        {
            if ( TextS == *Ptr )
            {
                VALUE = Values[i];
                return true;
            }
        }
        VALUE = Default;
        return false;;
    }
    else
    {
        VALUE = atoi(Text);
    }
    return true;
}

bool wxsEnumProperty::XmlWrite(cb_unused wxsPropertyContainer* Object,
                               TiXmlElement* Element)
{
    if ( VALUE != Default )
    {
        if ( UseNamesInXml )
        {
            // searching for name of this value
            int i = 0;
            for ( const wxChar** Ptr = Names; *Ptr; Ptr++, i++ )
            {
                if ( VALUE == Values[i] )
                {
                    Element->InsertEndChild(TiXmlText(cbU2C(*Ptr)));
                    return true;
                }
            }

            // Did not found value, storing as integer
        }

        Element->InsertEndChild(TiXmlText(cbU2C(wxString::Format(_T("%ld"),VALUE))));
        return true;
    }
    return false;
}

bool wxsEnumProperty::PropStreamRead(cb_unused wxsPropertyContainer* Object,
                                     wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),VALUE,Default);
}

bool wxsEnumProperty::PropStreamWrite(cb_unused wxsPropertyContainer* Object,
                                      wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),VALUE,Default);
}
