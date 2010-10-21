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

#ifndef WXSCORRECTOR_H
#define WXSCORRECTOR_H

#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/hashset.h>

class wxsItemResData;
class wxsItem;

/** \brief This class is responsible for correcting data in item resource.
 *
 * This class takes care about variable names and identifiers. It may also
 * do some additional checks in future. Base rule is that there can be no items
 * in resource with same variable name nor there may not be two items with same
 * identifier
 */
class wxsCorrector
{
    public:

        /** \brief Ctor */
        wxsCorrector(wxsItemResData* Data);

        /** \brief Correcting data right after resource load
         *
         * This function scans all items searching for duplicates of
         * variable names and identifiers. For duplicates, new values are
         * created. Also empty fields are filled with new items.
         *
         * \return false if all data was correct, false otherwise
         */
        bool GlobalCheck();

        /** \brief Correcting data after change of item properties
         *
         * This function should be called when variable name or identifier
         * may have changed in one item.
         */
        void AfterChange(wxsItem* Item);

        /** \brief Preparing new item before pasting into resource.
         *
         * This function updates all variable names and identifier in
         * given tree subnode.
         */
        void BeforePaste(wxsItem* Item);

        /** \brief Notifying inish of pasting item */
        void AfterPaste(wxsItem* Item);

        /** \brief Clearing current vars/ids cache */
        void ClearCache();

    private:

        /** \brief Rebuilding content of Vars and Ids */
        void RebuildSets();

        /** \brief Generating new variable name for given item */
        void SetNewVarName(wxsItem* Item);

        /** \brief Generating new identifier for given item */
        void SetNewIdName(wxsItem* Item);

        /** \brief Function checking and fixing if necessary variable name */
        bool FixVarName(wxString& Name);

        /** \brief Function checking and fixing if necessary identifier */
        bool FixIdName(wxString& Id);

        bool IsWxWidgetsIdPrefix(const wxString& Id);

        void RebuildSetsReq(wxsItem* Item,wxsItem* Exclude);
        bool FixAfterLoadCheckNames(wxsItem* Item);
        bool FillEmpty(wxsItem* Item);
        void BeforePasteReq(wxsItem* Item);

        WX_DECLARE_HASH_SET(wxString,wxStringHash,wxStringEqual,wxStringSet);

        wxsItemResData* m_Data; ///< \brief Data object using this corrector
        wxStringSet m_Vars;     ///< \brief set of currently used variable names
        wxStringSet m_Ids;      ///< \brief set of currently used identifiers
        bool m_NeedRebuild;     ///< \brief flag indicating when Vars and Ids contain valid data
};

#endif
