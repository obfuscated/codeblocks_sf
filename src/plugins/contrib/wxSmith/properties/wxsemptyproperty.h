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

#ifndef WXSEMPTYPROPERTY_H
#define WXSEMPTYPROPERTY_H

#include "wxsproperty.h"

/** \brief Empty property with no meaning
 *
 * This property is used to avoid failures when 0 pointers are supplied.
 * It should only avoid seg faults and currently there's no other purpose.
 * It's given as singleton class, can not be created.
 *
 * To get an instance, use Get() function
 */
class wxsEmptyProperty: public wxsProperty
{
    public:

        /** \brief Getting instance of empty property */
        static inline wxsEmptyProperty& Get() { return Singleton; }

    private:
        wxsEmptyProperty();
        virtual const wxString GetTypeName() { return _T(""); }
        static wxsEmptyProperty Singleton;
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring empty property (totally useless,
 *         currently no purpose)
 */
#define WXS_VOID() \
    Property(wxsEmptyProperty::Get(),0);

/** \} */


#endif
