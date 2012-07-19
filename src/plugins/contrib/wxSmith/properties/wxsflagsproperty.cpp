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

#include "wxsflagsproperty.h"
#include <globals.h>
#include <wx/tokenzr.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,long)

wxsFlagsProperty::wxsFlagsProperty(const wxString& PGName, const wxString& DataName,long _Offset,const long* _Values,const wxChar** _Names,bool _UpdateEnteries,long _Default,bool _UseNamesInXml,int Priority):
    wxsProperty(PGName,DataName,Priority),
    Offset(_Offset),
    Default(_Default),
    UpdateEnteries(_UpdateEnteries),
    Values(_Values),
    Names(_Names),
    UseNamesInXml(_UseNamesInXml)
{}


void wxsFlagsProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGChoices PGC(Names,Values);
    wxPGId Id = Grid->AppendIn(Parent,NEW_IN_WXPG14X wxFlagsProperty(GetPGName(),wxPG_LABEL,PGC,VALUE));
    Grid->SetPropertyAttribute(Id,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
    PGRegister(Object,Grid,Id);
}

bool wxsFlagsProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsFlagsProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    if ( UpdateEnteries )
    {
        #if wxCHECK_VERSION(2, 9, 0)
        wxPGChoices choices = Grid->GetGrid()->GetSelection()->GetChoices();
        choices.Set(Names,Values);
        #else
        Grid->GetPropertyChoices(Id).Set(Names,Values);
        #endif
    }
    Grid->SetPropertyValue(Id,VALUE);
    return true;
}

bool wxsFlagsProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
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
        wxString TextS = cbC2U(Text);
        wxStringTokenizer Tokenizer(cbC2U(Text),_T("| \t\n"), wxTOKEN_STRTOK);
        VALUE = 0;
        while ( Tokenizer.HasMoreTokens() )
        {
            wxString Token = Tokenizer.GetNextToken();
            int i = 0;
            for ( const wxChar** Ptr = Names; *Ptr; Ptr++, i++ )
            {
                if ( Token == *Ptr )
                {
                    VALUE |= Values[i];
                    break;
                }
            }
        }
        return false;;
    }
    else
    {
        VALUE = atol(Text);
    }
    return true;
}

bool wxsFlagsProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != Default )
    {
        if ( UseNamesInXml )
        {
            wxString FlagsText;
            int i = 0;
            for ( const wxChar** Ptr = Names; *Ptr; Ptr++, i++ )
            {
                if ( (VALUE & Values[i]) == Values[i] )
                {
                    FlagsText << *Ptr << _T("|");
                }
            }

            if ( !FlagsText.IsEmpty() )
            {
                // Removing last '|' character
                FlagsText.RemoveLast();
            }
            Element->InsertEndChild(TiXmlText(cbU2C(FlagsText)));
        }
        else
        {
            Element->InsertEndChild(TiXmlText(cbU2C(wxString::Format(_T("%ld"),VALUE))));
        }
        return true;
    }
    return false;
}

bool wxsFlagsProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),VALUE,Default);
}

bool wxsFlagsProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),VALUE,Default);
}
