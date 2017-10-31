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
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef LIB_AUTO_CONF_H
#define LIB_AUTO_CONF_H

#include <cbplugin.h>
#include <settings.h>
#include <sdk_events.h>
#include <tinyxml.h>

#include "librarydetectionmanager.h"
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

		static bool IsExtraEvent();

    private:

        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);

        void OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading);
        void OnProjectClose(CodeBlocksEvent& event);
        void OnCompilerStarted(CodeBlocksEvent& event);
        void OnCompilerFinished(CodeBlocksEvent& event);
        void OnCompilerSetBuildOptions(CodeBlocksEvent& event);
        void SetupTarget(CompileTargetBase* Target,const wxArrayString& Libs);
        bool TryAddLibrary(CompileTargetBase* Target,LibraryResult* Result);
        void RegisterScripting();
        void UnregisterScripting();
        bool TryDownload(const wxString& ShortCode,const wxString& FileName);

        // These functions are used in scripting bindings
		static bool AddLibraryToProject(const wxString& LibName,cbProject* Project,const wxString& TargetName);
		static bool RemoveLibraryFromProject(const wxString& LibName,cbProject* Project,const wxString& TargetName);
		static bool IsLibraryInProject(const wxString& LibName,cbProject* Project,const wxString& TargetName);
		static bool SetupTargetManually(CompileTargetBase* Target);
		static bool EnsureIsDefined(const wxString& ShortCode);

        ProjectConfiguration* GetProject(cbProject* Project);

        WX_DECLARE_HASH_MAP(cbProject*,ProjectConfiguration*,wxPointerHash,wxPointerEqual,ProjectMapT);
        WX_DECLARE_HASH_MAP(CompileTargetBase*,wxArrayString,wxPointerHash,wxPointerEqual,TargetLibsMapT);

        ResultMap m_KnownLibraries[rtCount];

        PkgConfigManager m_PkgConfig;
        ProjectMapT m_Projects;         ///< \brief Extra data for projects
        TargetLibsMapT m_Targets;       ///< \brief Libs for targets in project which is currently being built
        int m_HookId;

        static lib_finder* m_Singleton;
};

#endif
