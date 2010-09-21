/** \file wxseditenumproperty.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010  Gary Harris.
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
*/

#include "wxseditenumproperty.h"

#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxString)

wxsEditEnumProperty::wxsEditEnumProperty(const wxString &PGName,
        const wxString &DataName,
        long _Offset,
        const wxChar **_Names,
        bool _UpdateEnteries,
        const wxString &_Default,
        bool _XmlStoreEmpty,
        bool _UseNamesInXml,
        int Priority):
    wxsProperty(PGName, DataName, Priority),
    Offset(_Offset),
    Default(_Default),
    XmlStoreEmpty(_XmlStoreEmpty),
    UpdateEnteries(_UpdateEnteries),
    Names(_Names),
    UseNamesInXml(_UseNamesInXml)
{
    int     i;

	// the calling routine only needs the returned value as a string
	// the list of long Values[] is useless to the calling routine
	// so, let us hope that we have enough Values[] here

    for(i = 0; i < 512; i++) Values[i] = i;
}

/*! \brief Register the control with a property grid.
 *
 * \param Object wxsPropertyContainer*
 * \param Grid wxPropertyGridManager*
 * \param Parent wxPGId
 * \return void
 *
 */
void wxsEditEnumProperty::PGCreate(wxsPropertyContainer *Object, wxPropertyGridManager *Grid, wxPGId Parent)
{
    wxPGChoices PGC(Names, Values);

    PGRegister(Object, Grid, Grid->AppendIn(Parent, NEW_IN_WXPG14X wxEditEnumProperty(GetPGName(), wxPG_LABEL, PGC, VALUE)));
}

/*! \brief Read a property value.
 *
 * \param Object wxsPropertyContainer*
 * \param Grid wxPropertyGridManager*
 * \param Id wxPGId
 * \param Index long
 * \return bool
 *
 * \date 27/8/10
 * Updated by Cryogen to use the item name rather than the enumerator value under wxPropertyGrid 1.4.
 */
bool wxsEditEnumProperty::PGRead(wxsPropertyContainer *Object, wxPropertyGridManager *Grid, wxPGId Id, long Index)
{
	VALUE = Grid->GetPropertyValueAsString(Id);
    VALUE.Replace(_T("\\n"), _T("\n"));

    return true;
}

/*! \brief Write a property value.
 *
 * \param Object wxsPropertyContainer*
 * \param Grid wxPropertyGridManager*
 * \param Id wxPGId
 * \param Index long
 * \return bool
 *
 */
bool wxsEditEnumProperty::PGWrite(wxsPropertyContainer *Object, wxPropertyGridManager *Grid, wxPGId Id, long Index)
{
    wxString Fixed = VALUE;

    Fixed.Replace(_T("\n"), _T("\\n"));
   if ( UpdateEnteries )
    {
        #if wxCHECK_VERSION(2, 9, 0)
        wxPGChoices choices = Grid->GetGrid()->GetSelection()->GetChoices();
        choices.Set(Names,Values);
        #else
        Grid->GetPropertyChoices(Id).Set(Names,Values);
        #endif
    }
    Grid->SetPropertyValue(Id, Fixed);
    return true;
}

/*! \brief Read XML data.
 *
 * \param Object wxsPropertyContainer*
 * \param Element TiXmlElement*
 * \return bool
 *
 */
bool wxsEditEnumProperty::XmlRead(wxsPropertyContainer *Object, TiXmlElement *Element)
{
    if(!Element)
    {
        VALUE.Clear();
        return false;
    }
    // TODO: Use proper encoding
    wxString Base = cbC2U(Element->GetText());
    wxString Result;
    for(const wxChar *Ch = Base.c_str(); *Ch; Ch++)
    {
        if(*Ch == _T('_'))
        {
            if(*++Ch == _T('_'))
            {
                Result << _T('_');
            }
            else
            {
                Result << _T('&') << *Ch;
            }
        }
        else if(*Ch == _T('\\'))
        {
            switch(*++Ch)
            {
                case _T('n'):
                    Result << _T('\n');
                    break;
                case _T('r'):
                    Result << _T('\r');
                    break;
                case _T('t'):
                    Result << _T('\t');
                    break;
                case _T('\\'):
                    Result << _T('\\');
                    break;
                default:
                    Result << _T('\\') << *Ch;
                    break;
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

/*! \brief Write XML data.
 *
 * \param Object wxsPropertyContainer*
 * \param Element TiXmlElement*
 * \return bool
 *
 */
bool wxsEditEnumProperty::XmlWrite(wxsPropertyContainer *Object, TiXmlElement *Element)
{
    if(XmlStoreEmpty || (VALUE != Default))
    {
        wxString Base = VALUE;
        wxString Result;
        for(const wxChar *Ch = Base.c_str(); *Ch; Ch++)
        {
            switch(*Ch)
            {
                case _T('_'):
                    Result << _T("__");
                    break;       // TODO: This is NOT compatible with xrc file when there's no version entry or version is less than 2.3.0.1
                    //case _T('&'):  Result << _T('_');  break;     // We could leave this to be translated into &amp; but this looks nicer ;)
                case _T('\\'):
                    Result << _T("\\\\");
                    break;
                    // We could handle \n and \r here too but this is not necessary since XRC loading
                    // routines also handle \n and \r chars
                default:
                    Result << *Ch;
            }
        }
        // TODO: Use proper encoding
        Element->InsertEndChild(TiXmlText(cbU2C(Result)));
        return true;
    }
    return false;
}

/*! \brief Read from a property stream.
 *
 * \param Object wxsPropertyContainer*
 * \param Stream wxsPropertyStream*
 * \return bool
 *
 */
bool wxsEditEnumProperty::PropStreamRead(wxsPropertyContainer *Object, wxsPropertyStream *Stream)
{
    return Stream->GetString(GetDataName(), VALUE, Default);
}

/*! \brief Write to a property stream.
 *
 * \param Object wxsPropertyContainer*
 * \param Stream wxsPropertyStream*
 * \return bool
 *
 */
bool wxsEditEnumProperty::PropStreamWrite(wxsPropertyContainer *Object, wxsPropertyStream *Stream)
{
    return Stream->PutString(GetDataName(), VALUE, Default);
}
