/** \file wxsimagelistproperty.h
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

#ifndef WXSIMAGELISTPROPERTY_H
#define WXSIMAGELISTPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

/** \brief Property for wxsImageListProperty class
 *
 * \note This property doesn't take any default value.
 *       By default, the array is cleared.
 */
class wxsImageListProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor
         *  \param PGName       The name of this property in the Property Grid.
         *  \param DataName     The name of this property in data structures.
         *  \param DataSubName  The name applied for each array element.
         *  \param Offset       The offset of the value (returned from wxsOFFSET macro).
		 *  \param Priority int	The item's priority.
         */
		wxsImageListProperty(const wxString& PGName,const wxString& DataName,const wxString& DataSubName,long Offset,int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxArrayString"); }

        /** \brief Showing editor for this property
		 *
		 * \param Object wxsPropertyContainer*	A pointer to a wxsPropertyContainer object.
		 * \return bool	True on succes, otherwise false.
		 *
		 */
        virtual bool ShowEditor(wxsPropertyContainer* Object);

    protected:

        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual wxString GetStr(wxsPropertyContainer* Object);

	private:
        long Offset;				//!< The offset of a variable in this class.
        wxString DataSubName;		//!< The name to use for sub-item entries in XML
        wxString DataName;			//!< The name to use for item entries in XML.
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring wxImageList property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_IMAGELIST(ClassName,VarName,PGName,DataName) \
    { static wxsImageListProperty _Property(PGName,DataName,_("item"),wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring wxImageList property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Priority priority of property
 */
#define WXS_IMAGELIST_P(ClassName,VarName,PGName,DataName,Priority) \
    { static wxsImageListProperty _Property(PGName,DataName,_("item"),wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \} */

#endif
