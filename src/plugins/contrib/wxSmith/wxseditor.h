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

#ifndef WXSEDITOR_H
#define WXSEDITOR_H

#include <editorbase.h>

class wxsResource;

/** \brief Base class for editors used inside wxSmith
 *
 * This class is responsible for proper binding / unbinding editor
 * from resource.
 *
 * Resource is only an information that given resource should exist.
 * Editor does load this resource and allow user to edit it.
 *
 * When editing resource it should always be done in such form:
 *  - Call BeginChange() to put resource into unstable state
 *  - Change data of resource (without updating information on screen)
 *  - Call EndChange() which should update data on screen
 *
 */
class wxsEditor: public EditorBase
{
    public:

        /** \brief Ctor */
        wxsEditor(wxWindow* parent,const wxString& title,wxsResource* Resource);

        /** \brief Dctor */
        virtual ~wxsEditor();

        /** \brief Getting current resource */
        inline wxsResource* GetResource() { return m_Resource; }

    private:

        /** \brief Currently associated resource */
        wxsResource* m_Resource;
};

#endif
