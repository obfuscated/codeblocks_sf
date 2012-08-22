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

#ifndef WXSWIDGET_H
#define WXSWIDGET_H

#include "wxsitem.h"
#include "wxsbaseproperties.h"
#include "wxsstyle.h"
#include "wxsflags.h"

using namespace wxsFlags;

/** \brief Class used as a base class for standard widgets without any children
 *
 * Few steps to create new widgets:
 *  \li Create widget info structure
 *  \li Create new class deriving from wxsWidget
 *  \li Create set of styles using macros defined in wxsstyle.h
 *  \li Create set of events using macros defined in wxsevent.h
 *  \li Override EnumWidgetProperties function
 *  \li Override BuildCreatingCode function (it's declared in wxsItem)
 *  \li Override BuildPreview function (also declared in wxsItem)
 *
 */
class wxsWidget: public wxsItem
{
    public:

        /** \brief Ctor
         *  \param Data data managment object handling this item
         *  \param PropertiesFlags flags filtering sed base properties
         *         (see wxsBaseProperties for details)
         *  \param Info pointer to static widget info
         *  \param EventArray pointer to static set of events
         *  \param StyleSet set of used styles, if 0, this widget won't
         *         provide styles by default
         */
        wxsWidget(
            wxsItemResData*     Data,
            const wxsItemInfo*  Info,
            const wxsEventDesc* EventArray = 0,
            const wxsStyleSet*  StyleSet = 0,
            long                PropertiesFlags = flWidget);

    protected:

        /** \brief Function enumerating properties for this widget only
         *
         * This function should enumerate all extra properties
         * required by item (extra means not enumerated in base properties
         * not id or variable name).
         * These properties will be placed at the beginning, right after
         * there will be Variable name and identifier and at the end, all
         * required base properties.
         */
        virtual void OnEnumWidgetProperties(long Flags) = 0;

        /** \brief Function which adds new items to QPP
         *
         * This function may be used to add special quick properties for
         * this item.
         *
         * All QPPChild panels will be added before additional panels
         * added by widget.
         */
        virtual void OnAddWidgetQPP(wxsAdvQPP* QPP) { }

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
