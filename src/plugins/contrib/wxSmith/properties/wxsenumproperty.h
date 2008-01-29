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

#ifndef WXSENUMPROPERTY_H
#define WXSENUMPROPERTY_H

#include "wxsproperty.h"

/** \brief Property with enumerated values
 *
 * Enum property works almost the same as long property but it uses
 * predefined values from given set.
 * Note that this class is abstract and should be used as base class for others.
 */
class wxsEnumProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName           name of property used in Property Grid
         *  \param DataName         name of property used in data structures
         *  \param Offset           offset of long integer holding enumerated value (taken from wxsOFFSET macro)
         *  \param Values           array of long integer values which can be enumerated
         *  \param Names            array of names used for items in Values array, ending with 0
         *  \param UpdateEnteries   posting true here notifies, that arrays may change while property is shown in property grid
         *  \param Default          defaut value applied on read errors
         *  \param UseNamesInXml    if true, names will be stored inside xml node instead of values
         *  \param Priority         priority of this property
         */
		wxsEnumProperty(
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

/** \brief Macro automatically declaring enum property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable of type long inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Values global array of long values for enums
 *  \param Names global array of names (stored as wxChar*) for enums, ended with 0 entry
 *  \param Default value applied on read errors / validation failures
 */
#define WXS_ENUM(ClassName,VarName,PGName,DataName,Values,Names,Default) \
    { static wxsEnumProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Values,Names,false,Default,true); \
      Property(_Property); }

/** \brief Macro automatically declaring enum property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable of type long inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Values global array of long values for enums
 *  \param Names global array of names (stored as wxChar*) for enums, ended with 0 entry
 *  \param Default value applied on read errors / validation failures
 *  \param Priority priority of this property
 */
#define WXS_ENUM_P(ClassName,VarName,PGName,DataName,Values,Names,Default,Priority) \
    { static wxsEnumProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Values,Names,false,Default,true,Priority); \
      Property(_Property); }

/** \} */

#endif
