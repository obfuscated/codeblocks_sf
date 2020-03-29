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

#ifndef WXSCONTAINER_H
#define WXSCONTAINER_H

#include "wxsparent.h"
#include "wxsstyle.h"
#include "wxsbaseproperties.h"
#include "wxsflags.h"

#include <prep.h>

using namespace wxsFlags;

/** \brief Container is a class which represents widget that can
 *         have child items or one of root items
 */
class wxsContainer: public wxsParent
{
    public:

        /** \brief Ctor
         *  \param Data data management object handling this item
         *  \param Info pointer to static widget info
         *  \param EventArray pointer to static set of events
         *  \param StyleSet set of used styles, if 0, this widget won't
         *         provide styles by default
         *  \param PropertiesFlags flags filtering set base properties
         *         (see wxsBaseProperties for details)
         */
        wxsContainer(
            wxsItemResData* Data,
            const wxsItemInfo* Info,
            const wxsEventDesc* EventArray = 0,
            const wxsStyleSet* StyleSet=0,
            long PropertiesFlags=flContainer);

    protected:

        /** \brief Function enumerating properties for this container only
         *
         * This function should enumerate all extra properties
         * required by item (extra means not enumerated in base properties,
         * not id or variable name).
         * These properties will be placed at the beginning, right after
         * there will be Variable name and identifier and at the end, all
         * required base properties.
         */
        virtual void OnEnumContainerProperties(long Flags) = 0;

        /** \brief Function which adds new items to QPP
         *
         * This function may be used to add special quick properties for
         * this item.
         *
         * All QPPChild panels will be added before additional panels
         * added by widget.
         */
        virtual void OnAddContainerQPP(cb_unused wxsAdvQPP* QPP) { }

        /** \brief Checking if can add child item
         *
         * This function is by default implemented inside wxsContainer.
         * In case of some limitations made for children (like inside
         * wxSplitterWindow), this function should be overridden
         * to avoid invalidating item.
         *
         * Default implementation matches few rules:
         *  - spacer item can be added into sizers only
         *  - sizer can be added into other sizers or into empty container
         *    (cannot put sizer when there's other one inside container
         *     or when there's any item added)
         *  - if non-sizer container already has sizer, nothing else can be
         *    added into it
         */
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);

        /** \brief Function adding children items into preview window */
        void AddChildrenPreview(wxWindow* This,long Flags);

        /** \brief Function adding code generating child items */
        void AddChildrenCode();

    private:

        /** \brief Function enumerating properties with default ones
         *
         * Function enumerating item properties. The implementation
         * does call EnumContainerProperties() and adds all default properties.
         */
        virtual void OnEnumItemProperties(long Flags);

        /** \brief Function Adding QPPChild panels for base properties of this
         *         container.
         *
         * This function calls internally AddContainerQPP to add any additional
         * QPPChild panels.
         */
        virtual void OnAddItemQPP(wxsAdvQPP* QPP);
};

#endif
