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

#ifndef WXSGUIFACTORY_H
#define WXSGUIFACTORY_H

#include <wx/string.h>
#include <wx/hashmap.h>

class wxsGUI;
class wxsProject;

/** \brief Class representing GUI factory
 *
 * This class can be used to enumerate and produce any GUI item
 * managed by wxSmith. This is done using static members of this class.
 * To add more GUI, only instance of class derived from wxsGUIFactory need
 * to be created and it will automatically register new GUI modules.
 */
class wxsGUIFactory
{
    public:

        /** \brief Building GUI item from given name */
        static wxsGUI* Build(const wxString& Name,wxsProject* Project);

        /** \brief Creating GUI after selecting it from list */
        static wxsGUI* SelectNew(const wxString& Message,wxsProject* Project);

        /** \brief Getting name of created gui */
        virtual wxString GetName() { return m_Name; }

    protected:

        /** \brief Ctor */
        wxsGUIFactory(const wxString& Name);

        /** \brief Dctor */
        virtual ~wxsGUIFactory();

        /** \brief Creating GUI module */
        virtual wxsGUI* OnCreate(wxsProject* Project) = 0;

    private:

        WX_DECLARE_STRING_HASH_MAP(wxsGUIFactory*,GUIItemHashT);

        wxString m_Name;                        ///< \brief GUI name
        static inline GUIItemHashT& GetHash();  ///< \brief Getting hash table
};

/** \brief template for easy GUI registering
 *
 * To register GUI in wxSmith, put following code somewhere in GUI cpp file:
 * \code static wxsRegisterGUI<GUIClassName> Reg(_T("GUIClassName")); \endcode
 * Where GUIClassName should be replaced by name of real class
 */
template <class T> class wxsRegisterGUI: public wxsGUIFactory
{
    public:
        wxsRegisterGUI(const wxString& Name): wxsGUIFactory(Name) {}
    protected:
        virtual wxsGUI* OnCreate(wxsProject* Project) { return new T(Project); }
};

#endif
