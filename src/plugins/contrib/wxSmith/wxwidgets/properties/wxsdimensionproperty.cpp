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

#include "wxsdimensionproperty.h"
#include <globals.h>

wxString wxsDimensionData::GetPixelsCode(wxsCoderContext* Context)
{
    switch ( Context->m_Language )
    {
        case wxsCPP:
        {
            if ( !DialogUnits ) return wxString::Format(_T("%ld"),Value);
            return wxString::Format(_T("wxDLG_UNIT(%s,wxSize(%ld,0)).GetWidth()"),Context->m_WindowParent.c_str(),Value);
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDimensionData::GetPixelsCode"),Context->m_Language);
        }
    }
    return wxEmptyString;
}

namespace
{
    // Some helper values to create compound properties
    enum
    {
        DIM_VALUE = 1,
        DIM_UNITS = 2
    };
}



// Helper macros for fetching variables
#define VALUE   (wxsVARIABLE(Object,Offset,wxsDimensionData).Value)
#define UNITS   (wxsVARIABLE(Object,Offset,wxsDimensionData).DialogUnits)


wxsDimensionProperty::wxsDimensionProperty(
    const wxString&  PGName,
    const wxString& _PGDUName,
    const wxString&  DataName,
    long _Offset,
    long _DefaultValue,
    bool _DefaultDialogUnits,
    int Priority):
        wxsProperty(PGName,DataName,Priority),
        Offset(_Offset),
        DefaultValue(_DefaultValue),
        DefaultDialogUnits(_DefaultDialogUnits),
        PGDUName(_PGDUName)
{}


void wxsDimensionProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId DUId;
    PGRegister(Object,Grid,Grid->AppendIn(Parent,NEW_IN_WXPG14X wxIntProperty(GetPGName(),wxPG_LABEL,VALUE)),DIM_VALUE);
    PGRegister(Object,Grid,DUId = Grid->AppendIn(Parent,NEW_IN_WXPG14X wxBoolProperty(PGDUName,wxPG_LABEL,UNITS)),DIM_UNITS);
    Grid->SetPropertyAttribute(DUId,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
}

bool wxsDimensionProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case DIM_VALUE:
            VALUE = Grid->GetPropertyValue(Id).GetLong();
            return true;

        case DIM_UNITS:
            UNITS = Grid->GetPropertyValue(Id).GetBool();
            return true;

        default:
            break;
    }
    return false;
}

bool wxsDimensionProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case DIM_VALUE:
            Grid->SetPropertyValue(Id,VALUE);
            return true;

        case DIM_UNITS:
            Grid->SetPropertyValue(Id,UNITS);
            return true;

        default:
            break;
    }
    return false;
}

bool wxsDimensionProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE = DefaultValue;
        UNITS = DefaultDialogUnits;
        return false;
    }
    const char* Text = Element->GetText();
    if ( !Text )
    {
        VALUE = DefaultValue;
        UNITS = DefaultDialogUnits;
        return false;
    }

    // 'd' character at the end of string means this value is in dialog units
    if ( Text[0] && Text[strlen(Text)-1]=='d' )
    {
        UNITS = true;
    }
    else
    {
        UNITS = false;
    }

    // atol should cut off 'd' at the end
    VALUE = atol(Text);
    return true;
}

bool wxsDimensionProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != DefaultValue || UNITS != DefaultDialogUnits )
    {
        wxString Buffer = wxString::Format(_T("%ld"),VALUE);
        if ( UNITS )
        {
            Buffer.Append(_T("d"));
        }

        Element->InsertEndChild(TiXmlText(cbU2C(Buffer)));
        return true;
    }
    return false;
}

bool wxsDimensionProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( !Stream->GetLong(_T("value"),VALUE,DefaultValue) ) Ret = false;
    if ( !Stream->GetBool(_T("dialogunits"),UNITS,DefaultDialogUnits) ) Ret = false;
    Stream->PopCategory();
    return Ret;
}

bool wxsDimensionProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( !Stream->PutLong(_T("value"),VALUE,DefaultValue) ) Ret = false;
    if ( !Stream->PutBool(_T("dialogunits"),UNITS,DefaultDialogUnits) ) Ret = false;
    Stream->PopCategory();
    return Ret;
}
