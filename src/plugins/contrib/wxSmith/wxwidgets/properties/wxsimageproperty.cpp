/** \file wxsimageproperty.cpp
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

#include "wxsimageproperty.h"
#include "wxsimagelisteditordlg.h"
#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxArrayString)

wxsImageProperty::wxsImageProperty(const wxString &PGName, const wxString &_DataName, const wxString &_DataSubName, long _Offset, int Priority):
    wxsCustomEditorProperty(PGName, _DataName, Priority),
    Offset(_Offset),
    DataSubName(_DataSubName),
    DataName(_DataName)
{}

/*! \brief Show the image editor.
 *
 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
 * \return bool	True on success, otherwise false.
 *
 */
bool wxsImageProperty::ShowEditor(wxsPropertyContainer *Object)
{
    int                     						n;
    wxsBitmapIconData       	data;
    wxsBitmapIconEditorDlg	dlg(0, data, _T("wxART_OTHER"));
    wxBitmap                				bmp;
    wxString                				ss;

	// show the dialog, exit immediately if cancelled
    n = dlg.ShowModal();
    if(n != wxID_OK){
    	return false;
    }

	// no data?
    if(data.IsEmpty()){
        VALUE.Clear();
    }

	// get a preview image and store it as an array of XPM data
    else{
        bmp = data.GetPreview(wxDefaultSize);
        wxsImageListEditorDlg::BitmapToArray(bmp, VALUE);
    }

	// done
    return true;
}

/*! \brief Read XML data.
 *
 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
 * \param Element TiXmlElement*				A pointer to a TiXmlElement object.
 * \return bool	True on success, otherwise false.
 *
 */
bool wxsImageProperty::XmlRead(wxsPropertyContainer *Object, TiXmlElement *Element)
{
    VALUE.Clear();

    if(!Element)
    {
        return false;
    }

    for(TiXmlElement *Item = Element->FirstChildElement(cbU2C(DataSubName));
            Item;
            Item = Item->NextSiblingElement(cbU2C(DataSubName)))
    {
        const char *Text = Item->GetText();
        if(Text)
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
 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
 * \param Element TiXmlElement*				A pointer to a TiXmlElement object.
 * \return bool	True if count != 0, false otherwise.
 *
 */
bool wxsImageProperty::XmlWrite(wxsPropertyContainer *Object, TiXmlElement *Element)
{
    size_t Count = VALUE.Count();
    for(size_t i = 0; i < Count; i++)
    {
        XmlSetString(Element, VALUE[i], DataSubName);
    }
    return Count != 0;
}

/*! \brief Read from a property stream.
 *
 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
 * \param Stream wxsPropertyStream*		A pointer to a wxsPropertyStream object.
 * \return bool	Always returns true.
 *
 */
bool wxsImageProperty::PropStreamRead(wxsPropertyContainer *Object, wxsPropertyStream *Stream)
{
    VALUE.Clear();
    Stream->SubCategory(GetDataName());
    for(;;)
    {
        wxString Item;
        if(!Stream->GetString(DataSubName, Item, wxEmptyString)){
        	break;
        }
        VALUE.Add(Item);
    }
    Stream->PopCategory();
    return true;
}

/*! \brief Write to the property stream.
 *
 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
 * \param Stream wxsPropertyStream*		A pointer to a wxsPropertyStream object.
 * \return bool	Always returns true.
 *
 */
bool wxsImageProperty::PropStreamWrite(wxsPropertyContainer *Object, wxsPropertyStream *Stream)
{
    Stream->SubCategory(GetDataName());
    size_t Count = VALUE.GetCount();
    for(size_t i = 0; i < Count; i++)
    {
        Stream->PutString(DataSubName, VALUE[i], wxEmptyString);
    }
    Stream->PopCategory();
    return true;
}

/*! \brief Get a string to display in the text field..
 *
 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
 * \return wxString	The image string.
 *
 */
wxString wxsImageProperty::GetStr(wxsPropertyContainer *Object)
{
    wxString Result;
    size_t Count = VALUE.Count();

    if(Count == 0)
    {
        return _("Click to add items");
    }

    for(size_t i = 0; i < Count; i++)
    {
        wxString Item = VALUE[i];
        Item.Replace(_T("\""), _T("\\\""));
        if(i > 0)
        {
            Result.Append(_T(' '));
        }
        Result.Append(_T('"'));
        Result.Append(Item);
        Result.Append(_T('"'));
    }
    return Result;
}
