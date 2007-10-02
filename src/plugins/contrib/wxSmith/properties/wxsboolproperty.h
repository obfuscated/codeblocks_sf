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

#ifndef WXSBOOLPROPERTY_H
#define WXSBOOLPROPERTY_H

#include "wxsproperty.h"

/** \brief Base boolean property */
class wxsBoolProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName   name of property in Property Grid
         *  \param DataName name of property in data stuctures
         *  \param Offset   offset of boolean (taken from wxsOFFSET macro)
         *  \param Default  default value applied on read errors
         */
		wxsBoolProperty(const wxString& PGName,const wxString& DataName,long Offset,bool Default=0,int Priority=100);

        /** \brief Returning type name */
        virtual const wxString GetTypeName() { return _T("bool"); }

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        long Offset;
        bool Default;
};

/** \addtogroup properties_macros Macros automatically defining standard properties
 *  \{ */

/** \brief Macro automatically declaring boolean property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 */
#define WXS_BOOL(ClassName,VarName,PGName,DataName,Default) \
    { static wxsBoolProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Default); \
      Property(_Property); }

/** \brief Macro automatically declaring boolean property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 *  \param Priority priority of this property
 */
#define WXS_BOOL_P(ClassName,VarName,PGName,DataName,Default,Priority) \
    { static wxsBoolProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Default,Priority); \
      Property(_Property); }

/** \} */

#endif
