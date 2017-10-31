/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WXSAUIMANAGERFLAGSPROPERTY_H
#define WXSAUIMANAGERFLAGSPROPERTY_H

#include <properties/wxsproperties.h>

#include <wx/aui/aui.h>

/** \brief Property responsible for editing auimanager paneinfo options;
 *
 * \note This property uses standard names for property grid / data.
 */
class wxsAuiDockableProperty: public wxsProperty
{
    public:

        // Available docking places
        static const long TopDockable           = 0x0001;
        static const long BottomDockable        = 0x0002;
        static const long LeftDockable          = 0x0004;
        static const long RightDockable         = 0x0008;
        static const long Dockable              = 0x0010;

        /** \brief Ctor
         *  \param Offset   offset to long handling border flags
         *  \param Priority         priority of this property
         */
        wxsAuiDockableProperty(long Offset,int Priority);

        /** \brief Returning type of this property */
        virtual const wxString GetTypeName() { return _T("PaneInfo"); }

        /** \brief Getting string representation of flags */
        static wxString GetString(long Flags);

        /** \brief Getting PaneInfo properties */
        static void GetDockableFlags(wxAuiPaneInfo& PaneInfo, long Flags);

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

    private:

        static long ParseString(const wxString& String);

        long Offset;

        static const long DockableAll = TopDockable | BottomDockable | LeftDockable | RightDockable;
        static const long DockableMask  = DockableAll | Dockable;
};


/** \brief Macro automatically declaring auimanager dockable properties with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of long integer variable inside class used to keep flags
 *  \param Priority priority of this property
 */
#define WXS_AUIMANAGERFLAGS_P(ClassName,VarName,Priority) \
    { static wxsAuiDockableProperty _Property(wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }


/** \brief First time add property */
class wxsFirstAddProperty: public wxsProperty
{
    public:

        /** \brief Ctor
         *  \param PGName   name of property in Property Grid
         *  \param DataName name of property in data stuctures
         *  \param Offset   offset of boolean (taken from wxsOFFSET macro)
         *  \param Default  default value applied on read errors
         */
        wxsFirstAddProperty(const wxString& PGName,const wxString& DataName,long Offset,bool Default=0,int Priority=100);

        /** \brief Returning type name */
        virtual const wxString GetTypeName() { return _T("bool"); }

    protected:

        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

    private:
        long Offset;
        bool Default;
};

/** \brief Macro automatically declaring boolean property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 */
#define WXS_FIRSTADD(ClassName,VarName,DataName,Default) \
    { static wxsFirstAddProperty _Property(wxEmptyString,DataName,wxsOFFSET(ClassName,VarName),Default); \
      Property(_Property); }

#endif
