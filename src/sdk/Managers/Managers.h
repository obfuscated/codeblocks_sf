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
* @file Managers.h
* @author Muhammad Haggag (MHaggag@optimize-eg.com)
* @brief
*/
#if !defined( __MANAGERS_MANAGERS_H )
#define __MANAGERS_MANAGERS_H

#include <wx/config.h>

#ifdef __WXMSW__
	#ifdef EXPORT_LIB
		// Export
		#define MANAGERS_API __declspec(dllexport)
	#else
		#define MANAGERS_API __declspec(dllimport)
	#endif
#else
	#define MANAGERS_API
#endif

extern "C"
{
	MANAGERS_API Manager* GetManager();
	MANAGERS_API wxConfigBase* GetConfigBase();
	MANAGERS_API TemplateManager* GetTemplateManager();
	MANAGERS_API PluginManager* GetPluginManager();
	MANAGERS_API EditorManager* GetEditorManager();
	MANAGERS_API MacrosManager* GetMacrosManager();
	MANAGERS_API MessageManager* GetMessageManager();
	MANAGERS_API ProjectManager* GetProjectManager();
	MANAGERS_API ToolsManager* GetToolsManager();
	MANAGERS_API PersonalityManager* GetPersonalityManager();

	MANAGERS_API void SetManager( Manager* manager );
	MANAGERS_API void SetConfigBase( wxConfigBase* config );
	MANAGERS_API void SetTemplateManager( TemplateManager* manager );
	MANAGERS_API void SetPluginManager( PluginManager* manager );
	MANAGERS_API void SetEditorManager( EditorManager* manager );
	MANAGERS_API void SetMacrosManager( MacrosManager* manager );
	MANAGERS_API void SetMessageManager( MessageManager* manager );
	MANAGERS_API void SetProjectManager( ProjectManager* manager );
	MANAGERS_API void SetToolsManager( ToolsManager* manager );
	MANAGERS_API void SetPersonalityManager( PersonalityManager* manager );
}

namespace Managers
{
	// Overloaded getter functions. The overloader is useless, it's just there so that the
	// compiler can know which function the caller means
	inline Manager* Get( Manager* overloader ) { return GetManager(); }
	inline void Set( Manager* manager ) { SetManager( manager ); }

	// wxConfigBase Specializations
	inline wxConfigBase* Get(wxConfigBase *overloader) { return GetConfigBase(); }
	inline void Set( wxConfigBase* config ) { SetConfigBase( config ); }

	// TemplateManager specializations
	inline TemplateManager* Get(TemplateManager* overloader) { return GetTemplateManager(); }
	inline void Set( TemplateManager* manager ) { SetTemplateManager( manager ); }

	// Plugin specializations
	inline PluginManager* Get(PluginManager *overloader) { return GetPluginManager(); }
	inline void Set( PluginManager* manager ) { SetPluginManager( manager ); }

	// Editor specializations
	inline EditorManager* Get(EditorManager *overloader) { return GetEditorManager(); }
	inline void Set( EditorManager* manager ) { SetEditorManager( manager ); }

	// Macors specializations
	inline MacrosManager* Get( MacrosManager *overloader ) { return GetMacrosManager(); }
	inline void Set( MacrosManager* manager ) { SetMacrosManager( manager ); }

	// Message specializations
	inline MessageManager* Get(MessageManager *overloader) { return GetMessageManager(); }
	inline void Set( MessageManager* manager ) { SetMessageManager( manager ); }

	// Project specializations
	inline ProjectManager* Get(ProjectManager *overloader) { return GetProjectManager(); }
	inline void Set( ProjectManager* manager ) { SetProjectManager( manager ); }

	// Tools specializations
	inline ToolsManager* Get(ToolsManager *overloader) { return GetToolsManager(); }
	inline void Set( ToolsManager* manager ) { SetToolsManager( manager ); }

	// Personality specializations
	inline PersonalityManager* Get(PersonalityManager *overloader) { return GetPersonalityManager(); }
	inline void Set( PersonalityManager* manager ) { SetPersonalityManager( manager ); }
};

#endif	// if !defined( __MANAGERS_MANAGERS_H )
