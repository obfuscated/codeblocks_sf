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

#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/hashmap.h>
#include <tinyxml/tinyxml.h>
#include <cbproject.h>

/** \brief Configuration of one project */
class ProjectConfiguration
{
    public:

        /** \brief Ctor */
        ProjectConfiguration();

        /** \brief Dctor */
        ~ProjectConfiguration();

        /** \brief Loading configuration from xml node */
        void XmlLoad(TiXmlElement* Node,cbProject* Project);

        /** \brief Writing configuration to xml node */
        void XmlWrite(TiXmlElement* Node,cbProject* Project);

        WX_DECLARE_STRING_HASH_MAP(wxArrayString,wxMultiStringMap);

        wxArrayString    m_GlobalUsedLibs;  ///< \brief List of used libraries for the whole project
        wxMultiStringMap m_TargetsUsedLibs; ///< \brief List of libs used in targets
};

#endif
