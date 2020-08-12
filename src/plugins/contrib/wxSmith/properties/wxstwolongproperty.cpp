
/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2020
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
* $Revision: $
* $Id: $
* $HeadURL: $
*/

#include "wxstwolongproperty.h"
#include <globals.h>

wxString wxsTwoLongData::GetLongCode(wxsCoderContext* Context)
{
    switch ( Context->m_Language )
    {
        case wxsCPP:
        {
            return wxString::Format(_T("%ld, %ld"), Value1, Value2);
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsTwoLongData::GetLongCode"),Context->m_Language);
        }
    }
    return wxEmptyString;
}

namespace
{
    // Some helper values to create compound properties
    enum
    {
        DIM_DEF    = 1,
        DIM_VALUE1 = 2,
        DIM_VALUE2 = 3,
    };
}



// Helper macros for fetching variables
#define VALUE1   (wxsVARIABLE(Object,Offset,wxsTwoLongData).Value1)
#define VALUE2   (wxsVARIABLE(Object,Offset,wxsTwoLongData).Value2)
#define DEFVALUE   (wxsVARIABLE(Object,Offset,wxsTwoLongData).DefValue)


wxsTwoLongProperty::wxsTwoLongProperty(
            const wxString& PGName,
            const wxString& PGValue1Name,
            const wxString& PGValue2Name,
            const wxString& DataName,
            long _Offset,
            long _DefaultValue1,
            long _DefaultValue2,
            int Priority) :
        wxsProperty(PGName,DataName,Priority),
        Offset(_Offset),
        DefaultValue1(_DefaultValue1),
        DefaultValue2(_DefaultValue2),
        Value1Name(PGValue1Name),
        Value2Name(PGValue2Name)
{}


void wxsTwoLongProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId DefId = Grid->AppendIn(Parent, new wxBoolProperty(GetPGName(),wxPG_LABEL,DEFVALUE));
    wxPGId V1Id = Grid->AppendIn(Parent, new wxIntProperty(Value1Name,wxPG_LABEL,VALUE1));
    wxPGId V2Id = Grid->AppendIn(Parent, new wxIntProperty(Value2Name,wxPG_LABEL,VALUE2));

    Grid->SetPropertyAttribute(DefId,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);

    PGRegister(Object,Grid,DefId,DIM_DEF);
    PGRegister(Object,Grid,V1Id,DIM_VALUE1);
    PGRegister(Object,Grid,V2Id,DIM_VALUE2);

    if ( DEFVALUE )
    {
        Grid->DisableProperty(V1Id);
        Grid->DisableProperty(V2Id);
    }

}

bool wxsTwoLongProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case DIM_DEF:
            DEFVALUE = Grid->GetPropertyValue(Id).GetBool();
            break;

        case DIM_VALUE1:
            VALUE1 = Grid->GetPropertyValue(Id).GetLong();
            break;

        case DIM_VALUE2:
            VALUE2 = Grid->GetPropertyValue(Id).GetLong();
            break;

        default:
            break;
    }

    return true;
}

bool wxsTwoLongProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case DIM_DEF:
            Grid->SetPropertyValue(Id,DEFVALUE);
            break;

        case DIM_VALUE1:
            if ( DEFVALUE )
            {
                Grid->DisableProperty(Id);
            }
            else
            {
                Grid->EnableProperty(Id);
            }
            Grid->SetPropertyValue(Id,VALUE1);
            break;

        case DIM_VALUE2:
            if ( DEFVALUE )
            {
                Grid->DisableProperty(Id);
            }
            else
            {
                Grid->EnableProperty(Id);
            }
            Grid->SetPropertyValue(Id,VALUE2);
            break;

        default:
            break;
    }
    return true;
}

bool wxsTwoLongProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        DEFVALUE = true;
        VALUE1 = -1;
        VALUE2 = -1;
        return false;
    }

    const char* Text = Element->GetText();

    // If no node or empty text, using default values
    if ( !Text || !Text[0] )
    {
        DEFVALUE = true;
        VALUE1 = -1;
        VALUE2 = -1;
        return false;
    }
    wxString Str = cbC2U(Text);

    if ( !Str.BeforeFirst(_T(',')).ToLong(&VALUE1) ||
         !Str.AfterLast(_T(',')).ToLong(&VALUE2) )
    {
        DEFVALUE = true;
        VALUE1 = -1;
        VALUE2 = -1;
        return false;
    }
    DEFVALUE = false;

    return true;
}

bool wxsTwoLongProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !DEFVALUE )
    {
        wxString Str;
        Str.Printf(_T("%ld,%ld"),VALUE1,VALUE2);
        Element->InsertEndChild(TiXmlText(cbU2C(Str)));
        return true;
    }
    return false;
}

bool wxsTwoLongProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( !Stream->GetBool(_T("default"),DEFVALUE,true) ) Ret = false;
    if ( !DEFVALUE )
    {
        if ( !Stream->GetLong(_T("value1"),VALUE1,-1) ) Ret = false;
        if ( !Stream->GetLong(_T("value2"),VALUE2,-1) ) Ret = false;
    }
    Stream->PopCategory();
    return Ret;
}

bool wxsTwoLongProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( !Stream->PutBool(_T("default"),DEFVALUE,true) ) Ret = false;
    if ( !DEFVALUE )
    {
        if ( !Stream->PutLong(_T("value1"),VALUE1,-1) ) Ret = false;
        if ( !Stream->PutLong(_T("value2"),VALUE2,-1) ) Ret = false;
    }
    Stream->PopCategory();
    return Ret;
}
