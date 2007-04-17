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

#include "wxsarraystringproperty.h"
#include "wxsarraystringeditordlg.h"

#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxArrayString)

wxsArrayStringProperty::wxsArrayStringProperty(const wxString& PGName,const wxString& DataName,const wxString& _DataSubName,long _Offset,int Priority):
    wxsCustomEditorProperty(PGName,DataName,Priority),
    Offset(_Offset),
    DataSubName(_DataSubName)
{}

bool wxsArrayStringProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsArrayStringEditorDlg Dlg(NULL,VALUE);
    return Dlg.ShowModal() == wxID_OK;
}

bool wxsArrayStringProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    VALUE.Clear();

    if ( !Element )
    {
        return false;
    }

    for ( TiXmlElement* Item = Element->FirstChildElement(cbU2C(DataSubName));
          Item;
          Item = Item->NextSiblingElement(cbU2C(DataSubName)) )
    {
        const char* Text = Item->GetText();
        if ( Text )
        {
            VALUE.Add(cbC2U(Text));
        }
        else
        {
            VALUE.Add(wxEmptyString);
        }
    }
    return true;
}

bool wxsArrayStringProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    size_t Count = VALUE.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        XmlSetString(Element,VALUE[i],DataSubName);
    }
    return Count != 0;
}

bool wxsArrayStringProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    VALUE.Clear();
    Stream->SubCategory(GetDataName());
    for(;;)
    {
        wxString Item;
        if ( !Stream->GetString(DataSubName,Item,wxEmptyString) ) break;
        VALUE.Add(Item);
    }
    Stream->PopCategory();
    return true;
}

bool wxsArrayStringProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    Stream->SubCategory(GetDataName());
    size_t Count = VALUE.GetCount();
    for ( size_t i=0; i<Count; i++ )
    {
        Stream->PutString(DataSubName,VALUE[i],wxEmptyString);
    }
    Stream->PopCategory();
    return true;
}

wxString wxsArrayStringProperty::GetStr(wxsPropertyContainer* Object)
{
    wxString Result;
    size_t Count = VALUE.Count();

    if ( Count == 0 )
    {
        return _("Click to add items");
    }

    for ( size_t i=0; i<Count; i++ )
    {
        wxString Item = VALUE[i];
        Item.Replace(_T("\""),_T("\\\""));
        if ( i > 0 )
        {
            Result.Append(_T(' '));
        }
        Result.Append(_T('"'));
        Result.Append(Item);
        Result.Append(_T('"'));
    }
    return Result;
}
