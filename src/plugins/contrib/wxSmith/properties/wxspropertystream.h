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

#ifndef WXSPROPERTYSTREAM_H
#define WXSPROPERTYSTREAM_H

#include <wx/string.h>

/** \brief Interface for property stream
 *
 * Using property stream one can define any type of data reader/writer.
 * Basic data types are processed using functions with names simillar to data
 * types. More complex types can be processed using SubCategory() and
 * PopCategory() functions which groups data.
 *
 * Each reading function returns True or False. True is returned if value read
 * correctly, false if read error. Error will usually mean - no more data with
 * this name found.
 * It's permitted to use same names for different values.
 * Calling read functions with same name will find next data with given name
 * located in stream after previous read. This could be usefull to store arrays
 * of data.
 *
 * Currently the functionality is focused on reading/writing. But it may be
 * extended to additional data checking. Because of that fact, arguments
 * passed to PutXXX functions (functions storing data) are using references
 * instead of values.
 *
 * Most Get / Put functions are defined by dedfault. Default behaviour is to
 * convert from / to string value. It's obvious that it can not be done for
 * string itself. So the minimum to make property stream usable is to write
 * own GetString / PutString functions and fuuncitons processing sub categories:
 * SubCategory() and PopCategory().
 */
class wxsPropertyStream
{
    public:

        /** \brief Ctor */
        wxsPropertyStream() {}

        /** \brief Dctor */
        virtual ~wxsPropertyStream() {}

        /** \brief Reading wxChar value from steram */
        virtual bool GetChar(const wxString &Name, wxChar& Value, wxChar Default = _T('\0'));

        /** \brief Writing wxChar value to stream */
        virtual bool PutChar(const wxString &Name, wxChar& Value, wxChar Default = _T('\0'));

        /** \brief Getting double value */
        virtual bool GetDouble(const wxString& Name, double& value, double Default = 0.0);

        /** \brief Writting double value */
        virtual bool PutDouble(const wxString& Name, double& value, double Default = 0.0);

        /** \brief Getting long value */
        virtual bool GetLong(const wxString &Name, long& Value, long Default = 0);

        /** \brief Writing long value */
        virtual bool PutLong(const wxString &Name, long& Value, long Default = 0);

        /** \brief Getting unsigned long value */
        virtual bool GetULong(const wxString &Name, unsigned long& Value, unsigned long Default = 0);

        /** \brief Writing unsigned long value */
        virtual bool PutULong(const wxString &Name, unsigned long& Value, unsigned long Default = 0);

        /** \brief Getting bool value */
        virtual bool GetBool(const wxString &Name, bool& Value, bool Default = false);

        /** \brief Writing bool value */
        virtual bool PutBool(const wxString &Name, bool& Value, bool Default = false);

        /** \brief Getting wxString value */
        virtual bool GetString(const wxString &Name, wxString& Value, wxString Default = wxEmptyString)=0;

        /** \brief Writting wxString value */
        virtual bool PutString(const wxString &Name, wxString& Value, wxString Default = wxEmptyString)=0;

        /** \brief Function creating new data sub-group */
        virtual void SubCategory(const wxString &Name)=0;

        /** \brief Function restoring previous data group */
        virtual void PopCategory()=0;
};

#endif
