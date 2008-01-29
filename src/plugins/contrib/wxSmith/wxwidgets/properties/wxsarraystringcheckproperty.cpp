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

#include "wxsarraystringcheckproperty.h"
#include "wxsarraystringcheckeditordlg.h"

#include <globals.h>

// Helper macros for fetching variables
#define VALUE   wxsVARIABLE(Object,Offset,wxArrayString)
#define CHECK   wxsVARIABLE(Object,CheckOffset,wxArrayBool)

wxsArrayStringCheckProperty::wxsArrayStringCheckProperty(const wxString& PGName,const wxString& DataName,const wxString& _DataSubName,long _Offset,long _CheckOffset,int Priority):
    wxsCustomEditorProperty(PGName,DataName,Priority),
    Offset(_Offset),
    CheckOffset(_CheckOffset),
    DataSubName(_DataSubName)
{}

bool wxsArrayStringCheckProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsArrayStringCheckEditorDlg Dlg(0,VALUE,CHECK);
    return Dlg.ShowModal() == wxID_OK;
}

bool wxsArrayStringCheckProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    VALUE.Clear();
    CHECK.Clear();

    if ( !Element )
    {
        return false;
    }

    for ( TiXmlElement* Item = Element->FirstChildElement(cbU2C(DataSubName));
          Item;
          Item = Item->NextSiblingElement(cbU2C(DataSubName)) )
    {
        // Fetching value of "checked" flag
        const char* CheckedAttr = Item->Attribute("checked");
        CHECK.Add( CheckedAttr && CheckedAttr[0]=='1' && CheckedAttr[1]=='\0' );

        // Fetching item text
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

bool wxsArrayStringCheckProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    size_t Count = VALUE.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        TiXmlNode* End = Element->InsertEndChild(TiXmlElement(cbU2C(DataSubName)));
        if ( End )
        {
            if ( CHECK[i] )
            {
                End->ToElement()->SetAttribute("checked","1");
            }
            End->InsertEndChild(TiXmlText(cbU2C(VALUE[i])));
        }
    }
    return Count != 0;
}

bool wxsArrayStringCheckProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    VALUE.Clear();
    Stream->SubCategory(GetDataName());
    for(;;)
    {
        wxString Item;
        if ( !Stream->GetString(DataSubName,Item,wxEmptyString) ) break;
        VALUE.Add(Item);

        bool Checked;
        Stream->GetBool(DataSubName + _T("_checked"),Checked,false);
        CHECK.Add(Checked);
    }
    Stream->PopCategory();
    return true;
}

bool wxsArrayStringCheckProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    Stream->SubCategory(GetDataName());
    size_t Count = VALUE.GetCount();
    for ( size_t i=0; i<Count; i++ )
    {
        Stream->PutString(DataSubName,VALUE[i],wxEmptyString);
        Stream->PutBool(DataSubName + _T("_checked"),CHECK[i],false);
    }
    Stream->PopCategory();
    return true;
}

wxString wxsArrayStringCheckProperty::GetStr(wxsPropertyContainer* Object)
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
        if ( CHECK[i] )
        {
            Result.Append(_T('['));
        }
        Result.Append(_T('"'));
        Result.Append(Item);
        Result.Append(_T('"'));
        if ( CHECK[i] )
        {
            Result.Append(_T(']'));
        }
    }
    return Result;
}
