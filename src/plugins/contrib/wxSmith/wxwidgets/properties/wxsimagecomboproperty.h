/** \file wxsimagecomboproperty.h
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

#ifndef WXSIMAGECOMBOPROPERTY_H
#define WXSIMAGECOMBOPROPERTY_H

#include "../../properties/wxscustomeditorproperty.h"
#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

/** \brief Property for wxBitmapComboBox class
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsImageComboProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor
         *  \param PGName       name of property in Property Grid
         *  \param DataName     name of property in data structures
         *  \param DataSubName  name of name applied for each array element
         *  \param Offset       offset of value (returned from wxsOFFSET macro)
         */
		wxsImageComboProperty(const wxString& PGName,const wxString& DataName,const wxString& DataSubName,long Offset,int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxArrayString"); }

        /** \brief Showing editor for this property */
        virtual bool ShowEditor(wxsPropertyContainer* Object);

		/** \brief Function giving string representation of property
		* \param Object wxsPropertyContainer*
		* \return wxString
		*/
        virtual wxString GetStr(wxsPropertyContainer* Object);

    protected:

        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        long Offset;							//!< The offset of a variable in this class.
        wxString DataSubName;	//!< The name to use for sub-item entries in XML.
        wxString DataName;			//!< The name to use for item entries in XML.
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring wxsImageComboProperty property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_IMAGECOMBO(ClassName,VarName,PGName,DataName) \
    { static wxsImageComboProperty _Property(PGName,DataName,_("item"),wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring wxsImageComboProperty property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Priority priority of property
 */
#define WXS_IMAGECOMBO_P(ClassName,VarName,PGName,DataName,Priority) \
    { static wxsImageComboProperty _Property(PGName,DataName,_("item"),wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \} */

#endif
