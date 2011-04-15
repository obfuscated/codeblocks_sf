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
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef LIBRARYCONFIGMANAGER_H
#define LIBRARYCONFIGMANAGER_H

#include <wx/dynarray.h>

#include "librarydetectionconfig.h"
#include "pkgconfigmanager.h"

class wxArrayString;
class wxString;
class TiXmlElement;

/** \brief Class managing library configurations used to detect libraries
 *
 * Note that confiugrations managed here are not yet fully functional library
 * settings - they can only be used to automatically detect existing libraries.
 * After successfull detection, LibraryResult is produced which does hold
 * useful configuration of library
 */
class LibraryDetectionManager
{
    public:

        /** \brief Ctor */
        LibraryDetectionManager(TypedResults& Results);

        /** \brief Dctor */
        ~LibraryDetectionManager();

        /** \brief Function returning number of loaded library configurations */
        inline int GetLibraryCount() const { return (int)Libraries.Count(); }

        /** \brief Function returning configuration for one library */
        const LibraryDetectionConfigSet* GetLibrary(int Index);

        /** \brief Getting library settings by name */
        const LibraryDetectionConfigSet* GetLibrary(const wxString& Shortcut);

        /** \brief Load search filters from configuration directories */
        bool LoadSearchFilters();

        /** \brief Function clearing current library set */
        void Clear();

        /** \brief Storing new library settings
         *  \return -1 - invalid data, -2 - couldn't write file, >= 0 - ok
         */
        int StoreNewSettingsFile( const wxString& shortcut, const std::vector< char >& content );

    private:

        /** \brief Function loading xml configuration files from specified directory */
        int LoadXmlConfig(const wxString& Dir);

        /** \brief Loading configuration from given filename */
        int LoadXmlFile(const wxString& Name);

        /** \brief Loading configuration from xml document */
        int LoadXmlDoc(TiXmlDocument& Doc);

        /** \brief Loading configuration from given Xml node
         *
         * \param Elem xml node
         * \param Config storage for configuration (data read from node will be appended to current configuration)
         * \param ConfigSet current configurations set
         * \param Filters if true, load settings for filters
         * \param Settings if true, load library settings
         */
        int LoadXml(TiXmlElement* Elem,LibraryDetectionConfig& Config,LibraryDetectionConfigSet* ConfigSet,bool Filters=true,bool Settings=true);

        /** \brief Test if configuration is valid */
        bool CheckConfig(const LibraryDetectionConfig& Cfg) const;

        /** \brief Testing if there's library configuration in pkg-config database */
        bool IsPkgConfigEntry(const wxString& Name);

        /** \brief Adding configuration, if it's not valid it will be deleted */
        bool AddConfig(LibraryDetectionConfig& Cfg, LibraryDetectionConfigSet* Set);

        WX_DEFINE_ARRAY(LibraryDetectionConfigSet*,LCArray);

        LCArray Libraries;
        TypedResults& m_CurrentResults;
};

#endif
