/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#ifndef PKGCONFIGMANAGER_H
#define PKGCONFIGMANAGER_H

#include "resultmap.h"
#include <compiletargetbase.h>

/** \brief Class responsible for integration with pkg-config utility
 *
 * The functinality of this class is as follows:
 *  - Checking whether the system has pkg-config installed
 *  - List libraries which may be configured through pkg-config
 *  - Generate options which allow to use pkg-config inside project
 */
class PkgConfigManager
{
    public:

        /** \brief Ctor
         *
         * It does not detet the presence or list of libraries
         */
        PkgConfigManager();

        /** \brief Dctor */
        ~PkgConfigManager();

        /** \brief Checking whether there is pkg-config on this system */
        bool IsPkgConfig() { return m_PkgConfigVersion!=-1L; }

        /** \brief Getting the result of scanning for libraries */
        ResultMap& GetLibraries() { return m_Libraries; }

        /** \brief Clear all results */
        void Clear();

        /** \brief Update given target to use library from pkg-config */
        bool UpdateTarget(const wxString& LibName,CompileTargetBase* Target,bool Force=true);

        /** \brief Refreshing pkg-config data */
        void RefreshData();

    private:

        /** \brief Detecting version of pkg-config */
        bool DetectVersion();

        /** \brief Reading list of supported libraries */
        bool LoadLibraries();

        long m_PkgConfigVersion;    ///< \brief Pkg-Config version, -1 if there's no pkg-config
        ResultMap m_Libraries;      ///< \brief Libraries detected by pkg-config
};


#endif
