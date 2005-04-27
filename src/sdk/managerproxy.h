/**
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* @file managerproxy.h
* @author Muhammad Haggag (MHaggag@optimize-eg.com)
* @brief Implements proxy functionality required for sharing codeblocks managers across DLLs
*/
#if !defined( __SDK_MANAGERPROXY_H )
#define __SDK_MANAGERPROXY_H

// Forward decls must come before including manangers.h
class wxConfigBase;
class Manager;
class TemplateManager;
class PluginManager;
class EditorManager;
class MacrosManager;
class MessageManager;
class ProjectManager;
class ToolsManager;
class PersonalityManager;

#ifdef STATIC_SDK
	#include "Managers/Managers.h"
#endif

/**
  * VC++ can't export/import classes easily, so we build the sdk as a lib. However, singleton
  * managers should be in a DLL to be shared across "src" and all plugins. So, we do the
  * following for static lib SDK builds:
  * Define an instance of every (singleton) manager in a "CodeBlocksManagers.dll". The
  * manager classes use those instances instead of local ones.
  *
  * We define accessor functions (GetManager/SetManager) to the manager instance for both
  * implementations (static and dynamic lib SDK)
  *
  * @todo Perhaps use an overloaded assignment operator instead of "Set"
  */
template <class ManagerT>
class GenericManagerProxy
{
#ifndef STATIC_SDK
	static ManagerT* m_Manager;
#endif

public:
#ifndef STATIC_SDK
	static inline ManagerT* Get() { return m_Manager; }
	static inline void Set( ManagerT* manager ) { m_Manager = manager; }
#else
	static inline ManagerT* Get()
	{
		// We need to pass the null manager pointer so that the compiler can know which
		// overload to user
		return Managers::Get( (ManagerT*)0 );
	}
	static inline void Set( ManagerT* manager ) { Managers::Set( manager ); }
#endif
};

#ifndef STATIC_SDK
template <class ManagerT> ManagerT* GenericManagerProxy<ManagerT>::m_Manager;
#endif

typedef GenericManagerProxy<Manager> ManagerProxy;
typedef GenericManagerProxy<wxConfigBase> ConfigBaseProxy;
typedef GenericManagerProxy<TemplateManager> TemplateManagerProxy;
typedef GenericManagerProxy<PluginManager> PluginManagerProxy;
typedef GenericManagerProxy<EditorManager> EditorManagerProxy;
typedef GenericManagerProxy<MacrosManager> MacrosManagerProxy;
typedef GenericManagerProxy<MessageManager> MessageManagerProxy;
typedef GenericManagerProxy<ProjectManager> ProjectManagerProxy;
typedef GenericManagerProxy<ToolsManager> ToolsManagerProxy;
typedef GenericManagerProxy<PersonalityManager> PersonalityManagerProxy;

#endif	// if !defined( __SDK_MANAGERPROXY_H )
