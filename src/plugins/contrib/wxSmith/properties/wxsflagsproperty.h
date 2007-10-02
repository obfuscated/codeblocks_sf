/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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

#ifndef WXSFLAGSPROPERTY_H
#define WXSFLAGSPROPERTY_H

#include "wxsproperty.h"

/** \brief Property with flags
 *
 * Enum property works almost the same as long property but it uses
 * predefined values from given set usd as bit fields in this value.
 * Note that this class is abstract and should be used as base class for others.
 */
class wxsFlagsProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName           name of property used in Property Grid
         *  \param DataName         name of property used in data structures
         *  \param Offset           offset of long integer holding value (taken from wxsOFFSET macro)
         *  \param Values           array of long integer with values of flags
         *  \param Names            array of names used for items in Values array, ending with 0 string
         *  \param UpdateEnteries   posting true here notifies, that arrays may change
         *  \param Default          defaut value applied on read errors
         *  \param UseNamesInXml    if true, names will be stored inside xml node instead of values
         *  \param Priority         priority of this property
         */
		wxsFlagsProperty(
            const wxString& PGName,
            const wxString& DataName,
            long Offset,
            const long* Values,
            const wxChar** Names,
            bool UpdateEnteries=false,
            long Default=0,
            bool UseNamesInXml=false,
            int Priority=100);

    protected:

        virtual const wxString GetTypeName() { return _T(""); }
        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id, long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id, long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        long Offset;
        long Default;
        bool UpdateEnteries;
        const long* Values;
        const wxChar** Names;
        bool UseNamesInXml;
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring flags property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable of type long inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Values global array of long values for flags
 *  \param Names global array of names (stored as wxChar*) for flags, ended with 0 entry
 *  \param Default value applied on read errors / validation failures
 */
#define WXS_FLAGS(ClassName,VarName,PGName,DataName,Values,Names,Default) \
    { static wxsFlagsProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Values,Names,false,Default,true); \
      Property(_Property); }

/** \brief Macro automatically declaring flags property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable of type long inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Values global array of long values for flags
 *  \param Names global array of names (stored as wxChar*) for flags, ended with 0 entry
 *  \param Default value applied on read errors / validation failures
 *  \param Priority priority of this property
 */
#define WXS_FLAGS_P(ClassName,VarName,PGName,DataName,Values,Names,Default,Priority) \
    { static wxsFlagsProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Values,Names,false,Default,true,Priority); \
      Property(_Property); }

/** \} */

#endif
