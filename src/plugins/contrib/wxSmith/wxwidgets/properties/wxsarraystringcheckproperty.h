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

#ifndef WXSARRAYSTRINGCHECKPROPERTY_H
#define WXSARRAYSTRINGCHECKPROPERTY_H

#include "../../properties/wxsproperties.h"

#if wxCHECK_VERSION(2, 8, 0)
    WX_DEFINE_ARRAY_INT(bool,wxArrayBool);
#else
    WX_DEFINE_ARRAY(bool,wxArrayBool);
#endif

/** \brief Property for editing arrays of strings with checked option
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsArrayStringCheckProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor
         *  \param PGName       name of property in Property Grid
         *  \param DataName     name of property in data stuctures
         *  \param DataSubName  name of name applied for each array element
         *  \param Offset       offset of wxArrayString (returned from wxsOFFSET macro)
         *  \param CheckOffset  offset of wxArrayBool (returned from wxsOFFSET macro)
         *  \param Priority     priority of this property
         */
		wxsArrayStringCheckProperty(const wxString& PGName,const wxString& DataName,const wxString& DataSubName,long Offset,long CheckOffset,int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxArrayStringCheck"); }

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
        long CheckOffset;
        wxString DataSubName;
};

/** \addtogroup ext_properties_macros  Macros automatically dedfining extended properties
 *  \{ */

/** \brief Macro automatically declaring wxArrayStrting combined with wxArrayBool properties
 *  \param ClassName name of class holding this property
 *  \param StringVarName name of wxArrayString variable inside class
 *  \param BoolVarName name of wxArrayBool variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DataSubName name of subnode used in Xml / Data Streams
 */
#define WXS_ARRAYSTRINGCHECK(ClassName,StringVarName,BoolVarName,PGName,DataName,DataSubName) \
    { static wxsArrayStringCheckProperty _Property(PGName,DataName,DataSubName,wxsOFFSET(ClassName,StringVarName),wxsOFFSET(ClassName,BoolVarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring wxArrayStrting combined with wxArrayBool properties with custom priority
 *  \param ClassName name of class holding this property
 *  \param StringVarName name of wxArrayString variable inside class
 *  \param BoolVarName name of wxArrayBool variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DataSubName name of subnode used in Xml / Data Streams
 *  \param Priority priority of property
 */
#define WXS_ARRAYSTRINGCHECK_P(ClassName,StringVarName,BoolVarName,PGName,DataName,DataSubName,Priority) \
    { static wxsArrayStringCheckProperty _Property(PGName,DataName,DataSubName,wxsOFFSET(ClassName,StringVarName),wxsOFFSET(ClassName,BoolVarName),Priority); \
      Property(_Property); }

/** \} */


#endif
