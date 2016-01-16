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

#ifndef WXSFONTPROPERTY_H
#define WXSFONTPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

/** \brief Structure holding font configuration */
struct wxsFontData
{
    long Size;
    wxFontStyle Style;
    wxFontWeight Weight;
    bool Underlined;
    wxFontFamily Family;
    wxArrayString Faces;
    wxString Encoding;
    wxString SysFont;
    double RelativeSize;

    bool IsDefault : 1;
    bool HasSize : 1;
    bool HasStyle : 1;
    bool HasWeight : 1;
    bool HasUnderlined : 1;
    bool HasFamily : 1;
    bool HasEncoding : 1;
    bool HasSysFont : 1;
    bool HasRelativeSize : 1;

    /** \brief Function building font from font data */
    wxFont BuildFont();

    /** \brief Function generating code which will build font object with specified name
     * \return Code building font or empty string if font is default
     */
    wxString BuildFontCode(const wxString& FontName,wxsCoderContext* Context);

    /** \brief Ctor - initializes all values to default ones */
    wxsFontData():
        Size(12),
        Style(wxFONTSTYLE_NORMAL),
        Weight(wxFONTWEIGHT_NORMAL),
        Underlined(false),
        Family(wxFONTFAMILY_DEFAULT),
        IsDefault(true),
        HasSize(false),
        HasStyle(false),
        HasWeight(false),
        HasUnderlined(false),
        HasFamily(false),
        HasEncoding(false),
        HasSysFont(false),
        HasRelativeSize(false)
    {}

};

/** \brief Property for editing font properties
 */
class wxsFontProperty: public wxsCustomEditorProperty
{
    public:

        /** \brief Ctor
         *  \param PGName       name of property in Property Grid
         *  \param DataName     name of property in data stuctures
         *  \param Offset       offset of wxsFontData structure (returned from wxsOFFSET macro)
         *  \param Priority     priority of this property
         */
        wxsFontProperty(
            const wxString& PGName,
            const wxString& DataName,
            long Offset,
            int Priority=100);

        /** \brief Returning type name */
        virtual const wxString GetTypeName() { return _T("wxFont"); }

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
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring font property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsFontData variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_FONT(ClassName,VarName,PGName,DataName) \
    { static wxsFontProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring font property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsFontData variable inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Priority priority of this property
 */
#define WXS_FONT_P(ClassName,VarName,PGName,DataName,Priority) \
    { static wxsFontProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \} */


#endif
