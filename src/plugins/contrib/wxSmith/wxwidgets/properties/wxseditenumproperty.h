/** \file wxseditenumproperty.h
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

#ifndef WXSEDITENUMPROPERTY_H
#define WXSEDITENUMPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

/** \brief Property with enumerated values and editable field.
 *
 * Enum property works almost the same as long property but it uses
 * predefined values from given set.
 * Note that this class is abstract and should be used as base class for others.
 */
class wxsEditEnumProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName           name of property used in Property Grid
         *  \param DataName         name of property used in data structures
         *  \param Offset           offset of long integer holding enumerated value (taken from wxsOFFSET macro)
         *  \param Values           array of long integer values which can be enumerated
         *  \param Names            array of names used for items in Values array, ending with 0
         *  \param UpdateEntries   posting true here notifies, that arrays may change while property is shown in property grid
         *  \param Default          defaut value applied on read errors
         *  \param UseNamesInXml    if true, names will be stored inside xml node instead of values
         *  \param Priority         priority of this property
         */
		wxsEditEnumProperty(
            const wxString& PGName,
            const wxString& DataName,
            long Offset,
            const wxChar** Names,
            bool UpdateEntries=false,
            const wxString &Default=wxEmptyString,
            bool XmlStoreEmpty=false,
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
        long            		Offset;							//!< The offset of the long integer holding the enumerated value (taken from wxsOFFSET macro).
        wxString        	Default;							//!< The defaut value applied on read errors.
        bool            		XmlStoreEmpty;			//!< Store empty values in XML.
        bool            		UpdateEntries;			//!< If true, arrays may change while the property is shown in the property grid.
        long            		Values[512];					//!< An array of long integer values which can be enumerated.
        const wxChar  **Names;						//!< An array of names used for items in Values array, ending with 0.
        bool            		UseNamesInXml;		//!< If true, names will be stored in XML nodes instead of values.
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring enum property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable of type long inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Names global array of names (stored as wxChar*) for enums, ended with 0 entry
 *  \param Default value applied on read errors / validation failures
 *
 * \date 27/8/10
 * Updated by Cryogen to allow the values to be updated whilst the property is displayed.
 */
	#define WXS_EDITENUM(ClassName,VarName,PGName,DataName,Names,Default) \
    { static wxsEditEnumProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Names,true,Default,true); \
      Property(_Property); }

/** \brief Macro automatically declaring enum property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable of type long inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Names global array of names (stored as wxChar*) for enums, ended with 0 entry
 *  \param Default value applied on read errors / validation failures
 *  \param Priority priority of this property
 *
 * \date 27/8/10
 * Updated by Cryogen to allow the values to be updated whilst the property is displayed.
 */
#define WXS_EDITENUM_P(ClassName,VarName,PGName,DataName,Names,Default,Priority) \
    { static wxsEditEnumProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Names,true,Default,true,Priority); \
      Property(_Property); }

/** \} */

#endif
