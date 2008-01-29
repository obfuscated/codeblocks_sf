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

#ifndef WXSSTRINGPROPERTY_H
#define WXSSTRINGPROPERTY_H

#include "wxsproperty.h"

/** \brief Base string property (working on wxString class) */
class wxsStringProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName           name of property in Property Grid
         *  \param DataName         name of property in data stuctures
         *  \param Offset           offset of value (returned from wxsOFFSET macro)
         *  \param IsLongString     if true, long string editor will be used (used for multiline strings)
         *  \param XmlStoreEmpty    if true, string will be also stored when it's value is equal to default one
         *  \param Default          default value applied on read errors
         *  \param Priority         priority of this property
         */
		wxsStringProperty(const wxString& PGName,const wxString& DataName,long Offset,bool IsLongString=true,bool XmlStoreEmpty=false,const wxString& Default=wxEmptyString,int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxString"); }

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
        bool IsLongString;
        bool XmlStoreEmpty;
        wxString Default;
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring one-line wxString property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 *  \param XmlStoreEmpty true if strings equal to default value should be written to xml structure, false otherwise
 */
#define WXS_SHORT_STRING(ClassName,VarName,PGName,DataName,Default,XmlStoreEmpty) \
    { static wxsStringProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),false,XmlStoreEmpty,Default); \
      Property(_Property); }

/** \brief Macro automatically declaring multiline wxString property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 *  \param XmlStoreEmpty true if strings equal to default value should be written to xml structure, false otherwise
 */
#define WXS_STRING(ClassName,VarName,PGName,DataName,Default,XmlStoreEmpty) \
    { static wxsStringProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),true,XmlStoreEmpty,Default); \
      Property(_Property); }

/** \brief Macro automatically declaring one-line wxString property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 *  \param XmlStoreEmpty true if strings equal to default value should be written to xml structure, false otherwise
 *  \param Priority priority of this property
 */
#define WXS_SHORT_STRING_P(ClassName,VarName,PGName,DataName,Default,XmlStoreEmpty,Priority) \
    { static wxsStringProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),false,XmlStoreEmpty,Default,Priority); \
      Property(_Property); }

/** \brief Macro automatically declaring multiline wxString property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 *  \param XmlStoreEmpty true if strings equal to default value should be written to xml structure, false otherwise
 *  \param Priority priority of this property
 */
#define WXS_STRING_P(ClassName,VarName,PGName,DataName,Default,XmlStoreEmpty,Priority) \
    { static wxsStringProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),true,XmlStoreEmpty,Default,Priority); \
      Property(_Property); }

/** \} */


#endif
