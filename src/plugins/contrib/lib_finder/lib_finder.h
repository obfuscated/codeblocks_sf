/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2008  Bartlomiej Swiecki
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

#ifndef LIB_AUTO_CONF_H
#define LIB_AUTO_CONF_H

#include <cbplugin.h>
#include <settings.h>
#include <sdk_events.h>
#include <tinyxml/tinyxml.h>

#include "libraryconfigmanager.h"
#include "resultmap.h"
#include "projectconfiguration.h"
#include "libraryresult.h"
#include "pkgconfigmanager.h"

class LibraryResult;

class lib_finder: public cbToolPlugin
{
	public:

		lib_finder();
		~lib_finder();
		int Configure(){ return 0; }
		int Execute();
		void OnAttach();
		void OnRelease(bool appShutDown);

    private:

        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);

        void SetGlobalVar(const LibraryResult* Result);
        void ReadDetectedResults();
        void ReadPkgConfigResults();
        void ReadPredefinedResults();
        void WriteDetectedResults();
        void OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading);
        void OnProjectClose(CodeBlocksEvent& event);
        void OnCompilerSetBuildOptions(CodeBlocksEvent& event);
        void SetupTarget(CompileTargetBase* Target,const wxArrayString& Libs);
        bool TryAddLibrary(CompileTargetBase* Target,LibraryResult* Result);
        bool LoadSearchFilters(LibraryConfigManager* CfgManager);
        void RegisterScripting();
        void UnregisterScripting();
        void LoadPredefinedResultFromFile(const wxString& FileName);

        bool SameResults(LibraryResult* First, LibraryResult* Second);

        // These functions are used in scripting bindings
		static bool AddLibraryToProject(const wxString& LibName,cbProject* Project,const wxString& TargetName);
		static bool RemoveLibraryFromProject(const wxString& LibName,cbProject* Project,const wxString& TargetName);
		static bool IsLibraryInProject(const wxString& LibName,cbProject* Project,const wxString& TargetName);


        ProjectConfiguration* GetProject(cbProject* Project);

        WX_DECLARE_HASH_MAP(cbProject*,ProjectConfiguration*,wxPointerHash,wxPointerEqual,ProjectMapT);


        ResultMap m_KnownLibraries[rtCount];

        PkgConfigManager m_PkgConfig;
        ProjectMapT m_Projects;
        int m_HookId;

        static lib_finder* m_Singleton;
};

#endif
