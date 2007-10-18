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

#ifndef WXSEVENTSEDITOR_H
#define WXSEVENTSEDITOR_H

#include "../properties/wxsproperties.h"
#include "../wxscodinglang.h"

class wxsItem;
class wxsEvents;
class wxsEventDesc;
class wxsItemResData;

/** \brief This class is responsible for showing, managing and editing events in property grid */
class wxsEventsEditor
{
    public:

        /** \brief Ctor */
        wxsEventsEditor();

        /** \brief Dctor */
        ~wxsEventsEditor();

        /** \brief Getting singleton object */
        static wxsEventsEditor& Get();

        /** \brief Function adding events from given item to grid */
        void BuildEvents(wxsItem* Item,wxsPropertyGridManager* Grid);

        /** \brief Function called when content inside grid has been changed */
        void PGChanged(wxsItem* Item,wxsPropertyGridManager* Grid,wxPGId Id);

        /** \brief Function automatically adding new event function / going into function inside item
         * \return true if new event was added, false on failure or on going to old handler's definition
         */
        bool GotoOrBuildEvent(wxsItem* Item,int EventIndex,wxsPropertyGridManager* Grid);

	private:

        WX_DEFINE_ARRAY(wxPGId,wxArrayPGId);

        wxsItemResData* m_Data;
        wxsItem* m_Item;
        wxsEvents* m_Events;
        wxString m_Source;
        wxString m_Header;
        wxString m_Class;
        wxsCodingLang m_Language;
        wxArrayPGId m_Ids;

        /** \brief Finding all handlers with given argument type */
        void FindFunctions(const wxString& ArgType,wxArrayString& Array);

        /** \brief Gerring input from user with new function name */
        wxString GetNewFunction(const wxsEventDesc* Event);

        /** \brief Generating proposition for new function name */
        wxString GetFunctionProposition(const wxsEventDesc* Event);

        /** \brief Creating new function inside source code */
        bool CreateNewFunction(const wxsEventDesc* Event,const wxString& NewFunctionName);

        /** \brief Trying to find function's event handler in source code */
        bool GotoHandler(int EventIndex);
};

#endif
