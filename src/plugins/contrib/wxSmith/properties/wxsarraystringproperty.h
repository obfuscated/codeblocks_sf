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

#ifndef WXSARRAYSTRINGPROPERTY_H
#define WXSARRAYSTRINGPROPERTY_H

#include "wxscustomeditorproperty.h"

/** \brief Property for wxArrayString class
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsArrayStringProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor
         *  \param PGName       name of property in Property Grid
         *  \param DataName     name of property in data stuctures
         *  \param DataSubName  name of name applied for each array element
         *  \param Offset       offset of value (returned from wxsOFFSET macro)
         */
		wxsArrayStringProperty(const wxString& PGName,const wxString& DataName,const wxString& DataSubName,long Offset,int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxArrayString"); }

        /** \brief Showing editor for this property */
        virtual bool ShowEditor(wxsPropertyContainer* Object);

    protected:

        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual wxString GetStr(wxsPropertyContainer* Object);

	private:
        long Offset;
        wxString DataSubName;
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring wxArrayString property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DataSubName name for subelement used in Xml / Data Streams
 */
#define WXS_ARRAYSTRING(ClassName,VarName,PGName,DataName,DataSubName) \
    { static wxsArrayStringProperty _Property(PGName,DataName,DataSubName,wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring wxArrayString property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DataSubName name for subelement used in Xml / Data Streams
 *  \param Priority priority of property
 */
#define WXS_ARRAYSTRING_P(ClassName,VarName,PGName,DataName,DataSubName,Priority) \
    { static wxsArrayStringProperty _Property(PGName,DataName,DataSubName,wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \} */

#endif
