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

#ifndef WXSCOLOURPROPERTY_H
#define WXSCOLOURPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

#include <wx/dialog.h>
#if wxCHECK_VERSION(3, 0, 0)
#include <wx/propgrid/editors.h>
#else
#include <wx/propgrid/propdev.h>
#endif
#include <wx/propgrid/advprops.h>

#define wxsCOLOUR_DEFAULT   (wxPG_COLOUR_CUSTOM - 1)

/** \brief Class handling colour data for wxSmith */
class wxsColourData: public wxColourPropertyValue
{
    public:

        wxsColourData(wxUint32 type, const wxColour &colour): wxColourPropertyValue(type,colour) {}
        wxsColourData(wxUint32 type = wxsCOLOUR_DEFAULT): wxColourPropertyValue(type) {}
        wxsColourData(const wxColour &colour): wxColourPropertyValue(colour) {}
        wxsColourData(const wxColourPropertyValue& cp): wxColourPropertyValue(cp) {}

        /** \brief Getting wxColour object from wxColourPropertyValue
         *  \return wxColour class, if wxColour.Ok() will return false, default colour was used
         */
        wxColour GetColour();

        /** \brief Getting code building colour
         *  \return code with colour or empty string if there's default colour
         */
        wxString BuildCode(wxsCoderContext* Context);
};

/** \brief Colour property - property used for handling wxColour property
 *
 *  This property uses wxColourPropertyValue to keep data
 *
 */
class wxsColourProperty: public wxsProperty
{
    public:

        /** \brief Ctor
         *  \param PGName               name of property in Property Grid
         *  \param DataName             name of property in data stuctures
         *  \param ValueOffset          offset of wxColourPropertyValue member (taken from wxsOFFSET macro)
         *  \param Priority         priority of this property
         */
        wxsColourProperty(
            const wxString& PGName,
            const wxString& DataName,
            long ValueOffset,
            int Priority=100);

        /** \brief Returning type name */
        virtual const wxString GetTypeName() { return _T("wxsColour"); }

        /** \brief Getting wxColour object from wxColourPropertyValue
         *  \return wxColour class, if wxColour.Ok() will return false, default colour was used
         */
        static wxColour GetColour(const wxColourPropertyValue& value);

        /** \brief Getting code building colour
         *  \return code with colour or empty string if there's default colour
         */
        static wxString GetColourCode(const wxColourPropertyValue& value,wxsCodingLang Language);

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

    private:
        long ValueOffset;
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring colour property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsColourData variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_COLOUR(ClassName,VarName,PGName,DataName) \
    { static wxsColourProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring colour property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsColourData variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Priority priority of this property
 */
#define WXS_COLOUR_P(ClassName,VarName,PGName,DataName,Priority) \
    { static wxsColourProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \} */

#endif
