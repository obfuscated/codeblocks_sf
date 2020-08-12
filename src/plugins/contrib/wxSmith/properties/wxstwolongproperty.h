/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2020
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
* $Revision: $
* $Id: $
* $HeadURL:$
*/

#ifndef WXSTWOLONGPROPERTY_H
#define WXSTWOLONGPROPERTY_H

#include "wxsproperties.h"

struct wxsTwoLongData
{
    long Value1;
    long Value2;
    bool DefValue;

    wxsTwoLongData(): Value1(0),  Value2(0), DefValue(true)  {}

    /** \brief Function to get the first value
     */
    inline long GetValue1()
    {
        return Value1;
    }

    /** \brief Function to get the second value
     */
    inline long GetValue2()
    {
        return Value2;
    }

    /** \brief Return true if the default value is used
     */
    inline bool GetDefValue()
    {
        return DefValue;
    }

    /** \brief Function getting code returning valid value
     *
     * This function may be used when generating code. It will return
     * correct representation of value which may be directly passed
     * to created object.
     *
     */
    wxString GetLongCode(wxsCoderContext* Context);

};

/** \brief Dimension property (long integer value which may be in pixel or dialog units) */
class wxsTwoLongProperty: public wxsProperty
{
    public:
        /** \brief Ctor
         *  \param PGName               name of property in Property Grid
         *  \param PGDUName             name of "use dialog units" property
         *  \param DataName             name of property in data stuctures
         *  \param Offset               offset of wxsTwoLongData value (taken from wxsOFFSET macro)
         *  \param DefaultValue1        default value 1 applied on read errors
         *  \param DefaultValue2        default value 2 applied on read errors
         *  \param Priority             priority of this property
         */
        wxsTwoLongProperty(
            const wxString& PGName,
            const wxString& PGValue1Name,
            const wxString& PGValue2Name,
            const wxString& DataName,
            long Offset,
            long DefaultValue1=0,
            long DefaultValue2=0,
            int Priority=100);

        /** \brief Returning type name */
        virtual const wxString GetTypeName() { return _T("wxsTwoLongProperty"); }

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
        long DefaultValue1;
        long DefaultValue2;
        bool DefValue;

        wxString Value1Name;
        wxString Value2Name;
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring two long property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsTwoLongData variable inside class
 *  \param PGName name used in property grid
 *  \param PGValue1Name name of value 1 property
 *  \param PGValue2Name name of value 2 property
 *  \param DataName name used in Xml / Data Streams
 *  \param Default1 default value 1 (integer)
 *  \param Default2 default value 2 (integer)
 */
#define WXS_TWOLONG(ClassName,VarName,PGName,PGValue1Name,PGValue2Name,DataName,Default1,Default2) \
    { static wxsTwoLongProperty _Property(PGName,PGValue1Name,PGValue2Name,DataName,wxsOFFSET(ClassName,VarName),Default1, Default2); \
      Property(_Property); }

/** \brief Macro automatically declaring two long property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsTwoLongData variable inside class
 *  \param PGName name used in property grid
 *  \param PGValue1Name name of value 1 property
 *  \param PGValue2Name name of value 2 property
 *  \param DataName name used in Xml / Data Streams
 *  \param Default1 default value 1 (integer)
 *  \param Default2 default value 2 (integer)
 *  \param Priority priority of this property
 */
#define WXS_TWOLONG_P(ClassName,VarName,PGName,PGValue1Name,PGValue2Name,DataName,Default1,Default2,Priority) \
    { static wxsTwoLongProperty _Property(PGName,PGValue1Name,PGValue2Name,DataName,wxsOFFSET(ClassName,VarName),Default1, Default2,Priority); \
      Property(_Property); }

/** \} */

#endif
