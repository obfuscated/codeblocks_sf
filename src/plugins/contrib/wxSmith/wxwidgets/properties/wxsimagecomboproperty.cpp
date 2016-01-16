/** \file wxsimagecomboproperty.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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
* This code was taken from the wxSmithImage plug-in, copyright Ron Collins
* and released under the GPL.
*
*/

#include "wxsimagecomboproperty.h"
#include "wxsimagecomboeditordlg.h"

#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxArrayString)

wxsImageComboProperty::wxsImageComboProperty(const wxString& PGName,const wxString& _DataName,const wxString& _DataSubName,long _Offset,int Priority):
    wxsCustomEditorProperty(PGName,_DataName,Priority),
    Offset(_Offset),
    DataSubName(_DataSubName),
    DataName(_DataName)
{}

bool wxsImageComboProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsImageComboEditorDlg Dlg(0);
    return Dlg.Execute(VALUE);
}

/*! \brief Read XML control data.
 *
 * \param Object wxsPropertyContainer*    A pointer to a wxsPropertyContainer object.
 * \param Element TiXmlElement*                A pointer to the parent node of the XML block.
 * \return bool
 *
 */
bool wxsImageComboProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
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

/*! \brief Write XML data.
 *
 * \param Object wxsPropertyContainer*    A pointer to a wxsPropertyContainer object.
 * \param Element TiXmlElement*                A pointer to the parent node of the XML block.
 * \return bool
 *
 */
bool wxsImageComboProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    size_t Count = VALUE.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        XmlSetString(Element,VALUE[i],DataSubName);
    }
    return Count != 0;
}

/*! \brief Read a property stream.
 *
 * \param Object wxsPropertyContainer*
 * \param Stream wxsPropertyStream*
 * \return bool
 *
 */
bool wxsImageComboProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
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

/*! \brief Write a property stream.
 *
 * \param Object wxsPropertyContainer*
 * \param Stream wxsPropertyStream*
 * \return bool
 *
 */
bool wxsImageComboProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
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

/*! \brief Get a string to display in the text field.
 *
 * \param Object wxsPropertyContainer*
 * \return wxString
 *
 */
wxString wxsImageComboProperty::GetStr(wxsPropertyContainer* Object)
{
    wxString Result;
    size_t Count = VALUE.Count();

    if ( Count == 0 )
    {
        return _("Click to add items");
    }

    for ( size_t i = 0; i < Count; i++ )
    {
        wxString Item = VALUE[i];
        Item.Replace(_T("\""),_T("\\\""));
        if ( i > 0 )
        {
            Result.Append(_T(' '));
        }
        Result.Append(wxEmptyString);
        Result.Append(Item);
        Result.Append(wxEmptyString);
    }
    return Result;
}
