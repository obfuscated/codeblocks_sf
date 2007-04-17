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

#include "wxsstringproperty.h"

#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxString)

// TODO: Fix \n handling in property editor


wxsStringProperty::wxsStringProperty(const wxString& PGName, const wxString& DataName,long _Offset,bool _IsLongString,bool _XmlStoreEmpty,const wxString& _Default,int Priority):
    wxsProperty(PGName,DataName,Priority),
    Offset(_Offset),
    IsLongString(_IsLongString),
    XmlStoreEmpty(_XmlStoreEmpty),
    Default(_Default)
{}


void wxsStringProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxString Fixed = VALUE;
    Fixed.Replace(_T("\n"),_T("\\n"));
    wxPGId Id;
    if ( IsLongString )
    {
        Id = Grid->AppendIn(Parent,wxLongStringProperty(GetPGName(),wxPG_LABEL,Fixed));
    }
    else
    {
        Id = Grid->AppendIn(Parent,wxStringProperty(GetPGName(),wxPG_LABEL,Fixed));
    }
    PGRegister(Object,Grid,Id);
}

bool wxsStringProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetString();
    VALUE.Replace(_T("\\n"),_T("\n"));
    return true;
}

bool wxsStringProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    wxString Fixed = VALUE;
    Fixed.Replace(_T("\n"),_T("\\n"));
    Grid->SetPropertyValue(Id,Fixed);
    return true;
}

bool wxsStringProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE.Clear();
        return false;
    }
    // TODO: Use proper endocing
    wxString Base = cbC2U(Element->GetText());
    wxString Result;
    for ( const wxChar* Ch = Base.c_str(); *Ch; Ch++ )
    {
        if ( *Ch == _T('_') )
        {
            if ( *++Ch != _T('_') )
            {
                Result << _T('&');
            }
            Result << *Ch;
        }
        else if ( *Ch == _T('\\') )
        {
            switch ( *++Ch )
            {
                case _T('n'):  Result << _T('\n'); break;
                case _T('r'):  Result << _T('\r'); break;
                case _T('t'):  Result << _T('\t'); break;
                case _T('\\'): Result << _T('\\'); break;
                default: Result << _T('\\') << *Ch; break;
            }
        }
        else
        {
            Result << *Ch;
        }
    }
    VALUE = Result;
    return true;
}

bool wxsStringProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( XmlStoreEmpty || (VALUE != Default) )
    {
        wxString Base = VALUE;
        wxString Result;
        for ( const wxChar* Ch = Base.c_str(); *Ch; Ch++ )
        {
            switch ( *Ch )
            {
                case _T('_'):  Result << _T("__"); break;
                case _T('&'):  Result << _T('_');  break;     // We could leave this to be translated into &amp; but this looks nicer ;)
                case _T('\\'): Result << _T("\\\\"); break;
                // We could handle \n and \r here too but this is not necessary since XRC loading
                // routines also handle \n and \r chars
                default:       Result << *Ch;
            }
        }
        // TODO: Use proper encoding
        Element->InsertEndChild(TiXmlText(cbU2C(Result)));
        return true;
    }
    return false;
}

bool wxsStringProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetString(GetDataName(),VALUE,Default);
}

bool wxsStringProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutString(GetDataName(),VALUE,Default);
}
