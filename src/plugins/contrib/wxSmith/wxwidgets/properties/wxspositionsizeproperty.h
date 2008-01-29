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

#ifndef WXSPOSITIONSIZEPROPERTY_H
#define WXSPOSITIONSIZEPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

/** \brief Structure holding informations about size or position and functions operating on them */
struct wxsPositionSizeData
{
    bool IsDefault;
    long X;
    long Y;
    bool DialogUnits;

    inline wxsPositionSizeData(): IsDefault(true), X(0), Y(0), DialogUnits(false) {}

    /** \brief Function returning position from data */
    inline wxPoint GetPosition(wxWindow* Parent)
    {
        return IsDefault ?
            wxDefaultPosition:
            DialogUnits ? wxDLG_UNIT(Parent,wxPoint(X,Y)) : wxPoint(X,Y);
    }

    /** \brief Function fetching data from given position
     *
     * This function may be used to set proper values for X and Y coordinates
     * when we have wxPoint value. Thsi function takes care of converting
     * to valid units and check if value is default.
     */
    inline void SetPosition(const wxPoint& Point,wxWindow* Parent)
    {
        if ( Point == wxDefaultPosition )
        {
            IsDefault = true;
        }
        else
        {
            IsDefault = false;
            if ( DialogUnits )
            {
                wxPoint Temp = Parent->ConvertPixelsToDialog(Point);
                X = Temp.x;
                Y = Temp.y;
            }
            else
            {
                X = Point.x;
                Y = Point.y;
            }
        }
    }

    /** \brief Function returning string representation of position
     *
     * This representatin may be used in generated code.
     */
    wxString GetPositionCode(wxsCoderContext* Context);

    /** \brief Function returnign size from data */
    inline wxSize GetSize(wxWindow* Parent)
    {
        return IsDefault ?
            wxDefaultSize :
            DialogUnits ? wxDLG_UNIT(Parent,wxSize(X,Y)) : wxSize(X,Y);
    }

    /** \brief Function fetching data from given size
     *
     * This function may be used to set proper values for Width and Height
     * when we have wxSize value. Thsi function takes care of converting
     * to valid units and check if value is default.
     */
    inline void SetSize(const wxSize& Size,wxWindow* Parent)
    {
        if ( Size == wxDefaultSize )
        {
            IsDefault = true;
        }
        else
        {
            IsDefault = false;
            if ( DialogUnits )
            {
                wxSize Temp = Parent->ConvertPixelsToDialog(Size);
                X = Temp.GetWidth();
                Y = Temp.GetHeight();
            }
            else
            {
                X = Size.GetWidth();
                Y = Size.GetHeight();
            }
        }
    }

    /** \brief Function returning string representation of size
     *
     * This representatin may be used in generated code.
     */
    wxString GetSizeCode(wxsCoderContext* Context);

};

typedef wxsPositionSizeData wxsPositionData;
typedef wxsPositionSizeData wxsSizeData;

/** \brief Property handling Position and Size properties
 *
 * To declare this property, four variables are required - two long integers
 * for coordinates, one boolean for switching between dialog / pixel units
 * and one boolean for switching between default / user defined value.
 * There are no default values for this property. By default
 * "default" flag is turned on, dimensions are both -1, using dialog units
 * is set to false.
 */
class wxsPositionSizeProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGUseDefName name of "use default value" property
         *  \param PGXName      name of x/width property
         *  \param PGYName      name of y/height property
         *  \param PGDUName     name of "use dialog units" property
         *  \param DataName     name of property in data stuctures
         *  \param Offset       offset to wxsPositionSizeData structure
         *  \param Priority     priority of this property
         */
		wxsPositionSizeProperty(
            const wxString& PGUseDefName,
            const wxString& PGXName,
            const wxString& PGYName,
            const wxString& PGDUName,
            const wxString& DataName,
            long Offset,
            int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxPosition wxSize"); }

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        wxString PGXName;
        wxString PGYName;
        wxString PGDUName;
        wxString DataName;
        long Offset;
};

typedef wxsPositionSizeProperty wxsPositionProperty;
typedef wxsPositionSizeProperty wxsSizeProperty;

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring position property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsPositionData structure inside container
 *  \param PGDefName name of "default vlaue" property used in property grid
 *  \param PGXName name of X coordinate used in property grid
 *  \param PGYName name of Y coordinate used in property grid
 *  \param PGDUName name of "dialog units" switch used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_POSITION(ClassName,VarName,PGDefName,PGXName,PGYName,PGDUName,DataName) \
    { static wxsPositionProperty _Property(PGDefName,PGXName,PGYName,PGDUName,DataName,wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }

/** \brief Macro automatically declaring position property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsPositionData structure inside container
 *  \param PGDefName name of "default vlaue" property used in property grid
 *  \param PGXName name of X coordinate used in property grid
 *  \param PGYName name of Y coordinate used in property grid
 *  \param PGDUName name of "dialog units" switch used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Priority priority of this property
 */
#define WXS_POSITION_P(ClassName,VarName,PGDefName,PGXName,PGYName,PGDUName,DataName,Priority) \
    { static wxsPositionProperty _Property(PGDefName,PGXName,PGYName,PGDUName,DataName,wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \brief Macro automatically declaring size property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsSizeData structure inside container
 *  \param PGDefName name of "default vlaue" property used in property grid
 *  \param PGWidthName name of Width used in property grid
 *  \param PGHeightName name of Height coordinate used in property grid
 *  \param PGDUName name of "dialog units" switch used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_SIZE(ClassName,VarName,PGDefName,PGWidthName,PGHeightName,PGDUName,DataName) \
    { static wxsSizeProperty _Property(PGDefName,PGWidthName,PGHeightName,PGDUName,DataName,wxsOFFSET(ClassName,VarName)); \
      Property(_Property); }


/** \brief Macro automatically declaring size property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsSizeData structure inside container
 *  \param PGDefName name of "default vlaue" property used in property grid
 *  \param PGWidthName name of Width used in property grid
 *  \param PGHeightName name of Height coordinate used in property grid
 *  \param PGDUName name of "dialog units" switch used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Priority priority of this property
 */
#define WXS_SIZE_P(ClassName,VarName,PGDefName,PGWidthName,PGHeightName,PGDUName,DataName,Priority) \
    { static wxsSizeProperty _Property(PGDefName,PGWidthName,PGHeightName,PGDUName,DataName,wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }/** \} */


#endif
