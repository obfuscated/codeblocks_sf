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

#ifndef PREDEFINEDIDS_H
#define PREDEFINEDIDS_H

#include <wx/string.h>

/** \brief Class managing predefined identifiers
 *
 * There are two types of predefined ids - first is set of identifiers
 * used internally inside wxWidgets, second is when number is given instead
 * of string for identifier.
 *
 * \note This class contains static functions only - there's no instance of it.
 */
class wxsPredefinedIDs
{
    public:

        /** \brief Function checking if identifier is predefined
         * \return true Id is predefined, false otherwise
         */
        static bool Check(const wxString& Name);

        /** \brief Getting value of predefined id
         * \return id value or wxID_ANY if it's not predefined id
         */
        static wxWindowID Value(const wxString& Name);

};

#endif
