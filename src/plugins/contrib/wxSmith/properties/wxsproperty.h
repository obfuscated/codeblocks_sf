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

#ifndef WXSPROPERTY_H
#define WXSPROPERTY_H

// Some includes required in propgrid
#include <wx/scrolwin.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/textctrl.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>

#include <tinyxml/tinyxml.h>
#include "wxspropertystream.h"

class wxsPropertyContainer;

#if wxCHECK_VERSION(2, 9, 0)
#define wxCHECK_PROPGRID_VERSION(major,minor,release) (0)
#endif

#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
#define wxPGVariant                     wxVariant
#define wxPGId                          wxPGProperty*
#define wxParentProperty                wxPGProperty
#define wxCustomPropertyClass           wxPGProperty
#define wxEnumPropertyClass             wxEnumProperty
#define wxPG_VALUETYPE(T)               wxT(#T)
#define wxPGVariantToWxObjectPtr(A,B)   wxDynamicCast(A.GetWxObjectPtr(),B)
#define wxPG_PROP_UNSPECIFIED           wxPG_EX_AUTO_UNSPECIFIED_VALUES
#define NEW_IN_WXPG14X                  new
#else
#define NEW_IN_WXPG14X
#endif


/** \brief Class representing one property
 *
 * Property is object around real variable(s) which is responsible
 * for streaming it. Currently there are thre streamings supported:
 * - Streaming to/from PropertyGrid - this will be used in
 *   property editor
 * - Streaming to/from Xml structure - this kind of streaming will be
 *   compatible with Xrc file structure (with some additional data)
 *   Streaming to Xml format could be done in streaming to property stream
 *   but because of some special structures used in XRC format it is
 *   given as separate function set
 * - Streaming to/from property stream (interface modelled in wxsPropertyStream)
 *
 * Property object is build independently from class instance. Each operating
 * function takes pointer to such class as argument.
 *
 * Some properties provide additional functions operating on data. In some cases
 * data is not stored directly but must be stored in modified form. These
 * functions are operating on this modified form which guarantee consistency
 * of data (good example is wxsStyleProperty, where styles are not stored like
 * they are in wxWidgets)
 */
class wxsProperty
{
    public:

        /** \brief Ctor
         *
         * This constructor uses separate names for PGName and DataName
         *
         *  \param PGName name used in property grid
         *  \param DataName name used in data operations (including Xml/Xrc)
         *  \param Priority priority of this property used when arranging properties in property grid
         */
        wxsProperty(const wxString& PGName, const wxString& DataName,int Priority);

        /** \brief Dctor */
        virtual ~wxsProperty() {}

        /** \brief Function creating Property grid enteries
         *
         * This function must create all property grid enteries. All enteries
         * must have correct values set on initialization.
         * Each wxPGId of created entrty must be passed to PGRegister function.
         *
         * \note To add properties, use AppendIn, not Append function.
         *
         * \param Object class from which data will be taken
         * \param Grid grid where properties must be added
         * \param Parent id of parent property which MUST be passed to AppendIn function
         */
        virtual void PGCreate(wxsPropertyContainer* /*Object*/, wxPropertyGridManager* /*Grid*/, wxPGId /*Parent*/) {}

        /** \brief Function reading value from property grid.
         *
         * \param Object class where data should be stored, same as class passed to PGCreate
         * \param Grid property grid, same as grid passed to PGCreate
         * \param Id id of changed property
         * \param Index index of variable - value returned from PGRegister function, usually set to variable offset
         */
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index) { return false; }

        /** \brief Function writing value to property grid.
         *
         * \param Object class from which data should be read from, same as class passed to PGCreate
         * \param Grid property grid, same as grid passed to PGCreate
         * \param Id id of property
         * \param Index index returned from PGRegister(), usually set to variable offset
         */
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index) { return false; }

        /** \brief Function reading value from xml element
         *
         * \param Object class where data should be stored
         * \param Element Xml element for this property, may be 0 - in such case,
         *        default value should be applied
         * \return true on read success, false when applied default value
         */
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element) { return false; }

        /** \brief Function writing value to xml element
         *
         * Returning false means that values were not stored in xml node,
         * this usually happens when value is equal to default one.
         *
         * \param Object class from which data should be read
         * \param Element Xml element for this property, is not 0
         * \return True of write success, false when this element should not be stored
         */
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element) { return false; }

        /** \brief Function reading value from property stream
         *
         * Stream->GetXXX functions should be used inside this function
         *
         *  \return True on read success (when all Stream->GetXXX functions returned true), false otherwise
         */
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream) { return false; }

        /** \brief Function writing value to property stream (or checking it)
         *
         * Stream->PutXXX functions should be used inside this function
         *
         *  \return True on write success (when all stream->PutXXX functions returned true), false otherwise
         */
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream) { return false; }

        /** \brief Getting name of PropertyGrid entry */
        inline const wxString& GetPGName()   { return m_PGName; }

        /** \brief Getting name of data (Xml and others) entry */
        inline const wxString& GetDataName() { return m_DataName; }

        /** \brief Getting priority of this property */
        inline int GetPriority() { return m_Priority; }

        /** \brief Getting unique name of type
         *
         * Type name will be used in group selection to merge properties
         * of different objects into one thing. Returning empty string
         * means that this property won't be used in multiple selection.
         * This function must be declared in property implementations.
         */
        virtual const wxString GetTypeName() = 0;

    protected:

        /** \brief Registering property grid identifier
         *
         * This function must be called in PGCreate for each property created.
         * Identifier will be used to determine which property object must be
         * fired when value of property changes.
         * \param Object owner of properties, should be value passed to PGCreate
         * \param Grid property grid where object is registered. It must be
         *        value of Grid param passed to PGCreate
         * \param ID identifier of property returned from Grid->AddProperty()
         * \param Index any positive integer value which will be used inside
         *        PGRead and PGWrite calls. If <0, this function will try to
         *        create it's own indexes (usually sequence of numbers starting at 0)
         * \return Index of added property (value of Index if Index >= 0 ) or -1 on failue
         */
        long PGRegister(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId ID,long Index = -1);

        /** \brief Function getting properties flags from given container.
         *
         * This operation is prohibited by default because availability flags
         * are protected inside wxsPropertyContainer. wxsProperty class may
         * access it, and this operation is done here. Through this function,
         * classes derived from wxsProperty may check properties flags.
         *
         * \return Object->GetPropertiesFlags()
         */
        long GetPropertiesFlags(wxsPropertyContainer* Object);

        /** \brief Helper function for fetching value from xml element
         *  \param Elem - element holding value, may be 0
         *  \param Value - here string will be stored
         *  \param SubChild - name of sub node, if empty, Elem will be used
         *  \return true when string exist, false if it does not exist
         */
        static bool XmlGetString(TiXmlElement* Elem,wxString& Value,const wxString& SubChild = wxEmptyString);

        /** \brief Helper function for setting value of xml elemet
         *  \param Elem - element which will hold value
         *  \param Value - value
         *  \param SubChild - value of sub node, if empty, Elem will be used
         */
        static void XmlSetString(TiXmlElement* Elem,const wxString& Value,const wxString& SubChild = wxEmptyString);

        /** \brief Helper function for fetching long integer value from xml element
         *  \param Elem - element holding value, may be 0
         *  \param Value - here value will be stored
         *  \param SubChild - name of sub node, if empty, Elem will be used
         *  \return true when value exist and is valid long integer, false otherwie
         */
        inline static bool XmlGetLong(TiXmlElement* Elem,long& Value,const wxString& SubChild = wxEmptyString)
        {
            wxString Tmp;
            return XmlGetString(Elem,Tmp,SubChild) && Tmp.ToLong(&Value);
        }

        /** \brief Helper function for setting long integer value of xml elemet
         *  \param Elem - element which will hold value
         *  \param Value - value
         *  \param SubChild - value of sub node, if empty, Elem will be used
         */
        static void XmlSetLong(TiXmlElement* Elem, long Value, const wxString& SubChild = wxEmptyString)
        {
            XmlSetString(Elem,wxString::Format(_T("%ld"),Value),SubChild);
        }

        /** \brief Helper function for fetching bool value from xml element
         *  \param Elem - element holding value, may be 0
         *  \param Value - here value will be stored
         *  \param SubChild - name of sub node, if empty, Elem will be used
         *  \return true when value exist and is valid bool, false otherwie
         */
        inline static bool XmlGetBool(TiXmlElement* Elem,bool& Value,const wxString& SubChild = wxEmptyString)
        {
            long Tmp;
            if ( !XmlGetLong(Elem,Tmp,SubChild) ) return false;
            Value = Tmp!=0;
            return true;
        }

        /** \brief Helper function for setting long integer value of xml elemet
         *  \param Elem - element which will hold value
         *  \param Value - value
         *  \param SubChild - value of sub node, if empty, Elem will be used
         */
        static void XmlSetBool(TiXmlElement* Elem,bool Value,const wxString& SubChild = wxEmptyString)
        {
            XmlSetString(Elem,Value?_T("1"):_T("0"),SubChild);
        }

    private:

        wxString m_PGName;        ///< \brief Name used inside property grid
        wxString m_DataName;      ///< \brief Name of data element (xml element)
        int      m_Priority;      ///< \brief Priority of this property
};

/** \brief Macro fetching offset of variable from given object
 *
 * \param Class type of class containing variable
 * \param Variable name of variable inside class
 */
#define wxsOFFSET(Class,Variable) \
    ( (long)(((char*)(&(((Class*)1)->Variable)))) - \
      (long)(((char*)((wxsPropertyContainer*)((Class*)1)))) )

/** \brief Macro converting variable offset to value
 *
 * \param Instance pointer to class instance
 * \param Offset variable offset
 * \param Type type of variable
 */
#define wxsVARIABLE(Instance,Offset,Type) \
    (*((Type*)(((char*)&((wxsPropertyContainer&)*(Instance)))+(Offset))))

#endif
