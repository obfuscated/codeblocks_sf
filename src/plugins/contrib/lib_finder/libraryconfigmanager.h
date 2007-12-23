/*
* This file is part of lib_finder plugin for Code::Blocks Studio
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
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#ifndef LIBRARYCONFIGMANAGER_H
#define LIBRARYCONFIGMANAGER_H

#include <wx/dynarray.h>

#include "libraryconfig.h"

class wxArrayString;
class wxString;
class TiXmlElement;

class LibraryConfigManager
{
    public:

        /** \brief Ctor */
        LibraryConfigManager();

        /** \brief Dctor */
        ~LibraryConfigManager();

        /** \brief Function loading xml configuration files from specified directory */
        void LoadXmlConfig(const wxString& Dir);

        /** \brief Function returning number of loaded library configurations */
        inline int GetLibraryCount() const { return (int)Libraries.Count(); }

        /** \brief Function returning configuration for one library */
        const LibraryConfig* GetLibrary(int Index);

        /** \brief Function clearing current library set */
        void Clear();

    private:

        /** \brief Loading configuration from given filename */
        void LoadXmlFile(const wxString& Name);

        /** \brief Loading configuration from given Xml node
         *
         * \param Elem xml node
         * \param Config storage for configuration (data read from node will be appended to current configuration)
         * \param Filters if true, load settings for filters
         * \param Settings if true, load library settings
         */
        void LoadXml(TiXmlElement* Elem,LibraryConfig* Config,bool Filters=true,bool Settings=true);

        /** \brief Test if configuration is valid */
        bool CheckConfig(const LibraryConfig* Cfg) const;

        /** \brief Testing if there's library configuration in pkg-config database */
        bool IsPkgConfigEntry(const wxString& Name);

        /** \brief Adding configuration, if it's not valid it will be deleted */
        void AddConfig(LibraryConfig* Cfg);

        WX_DEFINE_ARRAY(LibraryConfig*,LCArray);

        LCArray Libraries;
};

#endif
