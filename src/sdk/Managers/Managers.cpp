// Managers.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "../manager.h"
#include "Managers.h"

#ifdef __WXMSW__
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

Manager* g_Manager;
wxConfigBase* g_Config;
TemplateManager* g_TemplateManager;
PluginManager* g_PluginManager;
EditorManager* g_EditorManager;
MacrosManager* g_MacrosManager;
MessageManager* g_MessageManager;
ProjectManager* g_ProjectManager;
ToolsManager* g_ToolsManager;

extern "C"
{
	MANAGERS_API Manager* GetManager() { return g_Manager;	}	
	MANAGERS_API wxConfigBase* GetConfigBase() { return g_Config; }
	MANAGERS_API TemplateManager* GetTemplateManager() { return g_TemplateManager; }	
	MANAGERS_API PluginManager* GetPluginManager() { return g_PluginManager; }	
	MANAGERS_API EditorManager* GetEditorManager() { return g_EditorManager; }	
	MANAGERS_API MacrosManager* GetMacrosManager() { return g_MacrosManager; }	
	MANAGERS_API MessageManager* GetMessageManager() { return g_MessageManager; }	
	MANAGERS_API ProjectManager* GetProjectManager() { return g_ProjectManager; }
	MANAGERS_API ToolsManager* GetToolsManager() { return g_ToolsManager; }
	
	MANAGERS_API void SetManager( Manager* manager ) { g_Manager = manager; }
	MANAGERS_API void SetConfigBase( wxConfigBase* config ) { g_Config = config; }
	MANAGERS_API void SetTemplateManager( TemplateManager* manager ) { g_TemplateManager = manager; }
	MANAGERS_API void SetPluginManager( PluginManager* manager ) { g_PluginManager = manager; }
	MANAGERS_API void SetEditorManager( EditorManager* manager ) { g_EditorManager = manager; }
	MANAGERS_API void SetMacrosManager( MacrosManager* manager ) { g_MacrosManager = manager; }
	MANAGERS_API void SetMessageManager( MessageManager* manager ){ g_MessageManager = manager; }
	MANAGERS_API void SetProjectManager( ProjectManager* manager ) { g_ProjectManager = manager; }
	MANAGERS_API void SetToolsManager( ToolsManager* manager ) { g_ToolsManager = manager; }
}
