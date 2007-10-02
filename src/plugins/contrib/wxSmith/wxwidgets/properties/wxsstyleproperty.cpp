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

#include "wxsstyleproperty.h"
#include "../wxsitem.h"
#include "../wxsflags.h"

#include <wx/tokenzr.h>

// Helper macro for fetching variables
#define STYLEBITS   wxsVARIABLE(Object,Offset,long)
#define STYLESETPTR wxsVARIABLE(Object,StyleSetPtrOffset,const wxsStyleSet*)

using namespace wxsFlags;

wxsStyleProperty::wxsStyleProperty(
    const wxString&  StyleName,
    const wxString&  DataName,
    long _Offset,
    long _StyleSetPtrOffset,
    bool _IsExtra,
    int Priority):
        wxsProperty(StyleName,DataName,Priority),
        Offset(_Offset),
        StyleSetPtrOffset(_StyleSetPtrOffset),
        IsExtra(_IsExtra)
{
}

void wxsStyleProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    if ( STYLESETPTR && !STYLESETPTR->GetNames(IsExtra).IsEmpty() )
    {
        const wxArrayString& StyleNames = STYLESETPTR->GetNames(IsExtra);
        const wxArrayLong&   StyleFlags = STYLESETPTR->GetFlags(IsExtra);
        const wxArrayLong&   StyleBits  = STYLESETPTR->GetBits(IsExtra);

        bool IsXrc = !( GetPropertiesFlags(Object) & flSource );
        wxPGConstants StyleConsts;

        size_t Count = StyleNames.Count();
        for ( size_t i = 0; i < Count; i++ )
        {
            if ( !IsXrc || (StyleFlags[i] & wxsSFXRC) )
            {
                StyleConsts.Add(StyleNames[i],StyleBits[i]);
            }
        }

        if ( StyleConsts.GetCount() )
        {
            wxPGId ID = Grid->AppendIn(Parent,wxFlagsProperty(GetPGName(),wxPG_LABEL,StyleConsts,STYLEBITS));
            Grid->SetPropertyAttribute(ID,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
            PGRegister(Object,Grid,ID);
        }
    }
}

bool wxsStyleProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    STYLEBITS = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsStyleProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    Grid->SetPropertyValue(Id,STYLEBITS);
    return true;
}

bool wxsStyleProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        STYLEBITS = STYLESETPTR?STYLESETPTR->GetDefaultBits(IsExtra):0;
        return false;
    }

    const char* Text = Element->GetText();
    wxString Str;
    if ( Text )
    {
        Str = cbC2U(Text);
    }
    if ( Str.empty() )
    {
        STYLEBITS = STYLESETPTR?STYLESETPTR->GetDefaultBits(IsExtra):0;
        return false;
    }
    STYLEBITS = STYLESETPTR->GetBits(Str,IsExtra);
    return true;
}

bool wxsStyleProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( STYLESETPTR )
    {
        if ( STYLEBITS != STYLESETPTR->GetDefaultBits(IsExtra) )
        {
            Element->InsertEndChild(TiXmlText(cbU2C(STYLESETPTR->GetString(STYLEBITS,IsExtra,wxsCPP))));
            return true;
        }
    }
    return false;
}

bool wxsStyleProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),STYLEBITS,STYLESETPTR?STYLESETPTR->GetDefaultBits(IsExtra):0);
}

bool wxsStyleProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),STYLEBITS,STYLESETPTR?STYLESETPTR->GetDefaultBits(IsExtra):0);
}
