/*
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
* $Id$
* $Date$
*/

#include <wx/string.h>
#include <wx/dir.h>
#include <wx/menu.h>
#include <wx/dynlib.h>

#include "pluginmanager.h" // class's header file
#include "cbplugin.h"
#include "messagemanager.h"
#include "manager.h"
#include "editormanager.h"
#include "pluginsconfigurationdlg.h"
#include "configmanager.h"
#include "managerproxy.h"

PluginManager* PluginManager::Get()
{
    if (!PluginManagerProxy::Get())
	{
        PluginManagerProxy::Set( new PluginManager() );
		Manager::Get()->GetMessageManager()->Log(_("PluginManager initialized"));
	}
    return PluginManagerProxy::Get();
}

void PluginManager::Free()
{
	if (PluginManagerProxy::Get())
	{
		delete PluginManagerProxy::Get();
		PluginManagerProxy::Set( 0L );
	}
}

// class constructor
PluginManager::PluginManager()
{
	ConfigManager::AddConfiguration(_("Plugin Manager"), "/plugins");
}

// class destructor
PluginManager::~PluginManager()
{
	UnloadAllPlugins();
}

void PluginManager::CreateMenu(wxMenuBar* menuBar)
{
}

void PluginManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

int PluginManager::ScanForPlugins(const wxString& path)
{
#ifdef __WXMSW__
	#define PLUGINS_MASK "*.dll"
#else
	#define PLUGINS_MASK "*.so"
#endif

    int count = 0;
    wxDir dir(path);

    if (!dir.IsOpened())
        return count;

    wxString filename;
    bool ok = dir.GetFirst(&filename, PLUGINS_MASK, wxDIR_FILES);
    while (ok)
    {
		//Manager::Get()->GetMessageManager()->AppendDebugLog(_("Trying %s: "), filename.c_str());
        if (LoadPlugin(path + '/' + filename))
            ++count;
        ok = dir.GetNext(&filename);
    }
    return count;

#undef PLUGINS_MASK
}

cbPlugin* PluginManager::LoadPlugin(const wxString& pluginName)
{
    wxLogNull zero; // no need for error messages; we check everything ourselves...
    //MessageManager* msgMan = Manager::Get()->GetMessageManager();

    wxDynamicLibrary* lib = new wxDynamicLibrary();
    lib->Load(pluginName);
    if (!lib->IsLoaded())
    {
        //msgMan->DebugLog(_("not loaded (file exists?)"));
        return 0L;
    }

    GetPluginProc proc = (GetPluginProc)lib->GetSymbol("GetPlugin");
    if (!proc)
    {
        lib->Unload();
        //msgMan->DebugLog(_("not a plugin"));
        return 0L;
    }

    cbPlugin* plug = proc();
    wxString plugName = plug->GetInfo()->name;

    // check if we have already loaded a plugin by that name
    if (FindPluginByName(plugName))
    {
        //msgMan->DebugLog(_("another plugin with name \"%s\" is already loaded..."), plugName.c_str());
        lib->Unload();
        return 0L;
    }
    
    PluginElement* plugElem = new PluginElement;
    plugElem->fileName = pluginName;
    plugElem->name = plugName;
    plugElem->library = lib;
	plugElem->plugin = plug;
    m_Plugins.Add(plugElem);

	//msgMan->DebugLog(_("loaded %s"), plugName.c_str());
    return plug;
}

void PluginManager::LoadAllPlugins()
{
    // check if a plugin crashed the app last time
    wxString probPlugin = ConfigManager::Get()->Read("/plugins/try_to_load", wxEmptyString);
    if (!probPlugin.IsEmpty())
    {
        wxString msg;
        msg.Printf(_("Plugin \"%s\" failed to load last time Code::Blocks was executed.\n"
                    "Do you want to disable this plugin from loading?"), probPlugin.c_str());
        if (wxMessageBox(msg, _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxNO)
            probPlugin = "";
    }

    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;

        // do not load it if the user has explicitly asked not to...
        wxString baseKey;
        baseKey << "/plugins/" << m_Plugins[i]->name;
        bool loadIt = ConfigManager::Get()->Read(baseKey, true);
        
        // if we have a problematic plugin, check if this is it
        if (loadIt && !probPlugin.IsEmpty())
        {
            loadIt = plug->GetInfo()->title != probPlugin;
            // if this is the problematic plugin, don't load it
            if (!loadIt)
                ConfigManager::Get()->Write(baseKey, false);
        }

        if (loadIt && !plug->IsAttached())
		{
            ConfigManager::Get()->Write("/plugins/try_to_load", plug->GetInfo()->title);
			Manager::Get()->GetMessageManager()->AppendLog(_("%s "), m_Plugins[i]->name.c_str());
            plug->Attach();
		}
    }
	Manager::Get()->GetMessageManager()->Log("");

    wxLogNull ln;
    ConfigManager::Get()->DeleteEntry("/plugins/try_to_load");
}

void PluginManager::UnloadAllPlugins()
{
//    Manager::Get()->GetMessageManager()->DebugLog("Count %d", m_Plugins.GetCount());
	unsigned int i = m_Plugins.GetCount();
    while (i > 0)
    {
		--i;
//		Manager::Get()->GetMessageManager()->DebugLog("At %d", i);
        cbPlugin* plug = m_Plugins[i]->plugin;
		if (!plug)
			continue;
//        Manager::Get()->GetMessageManager()->DebugLog("Doing '%s'", m_Plugins[i]->name.c_str());
        plug->Release(true);
        //it->first->library->Unload();
//        Manager::Get()->GetMessageManager()->DebugLog("Plugin '%s' unloaded", m_Plugins[i]->name.c_str());
        // FIXME: find a way to delete the toolbars too...
    }
}

cbPlugin* PluginManager::FindPluginByName(const wxString& pluginName)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->name == pluginName)
            return m_Plugins[i]->plugin;
    }

    return NULL;
}

cbPlugin* PluginManager::FindPluginByFileName(const wxString& pluginFileName)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->fileName == pluginFileName)
            return m_Plugins[i]->plugin;
    }

    return NULL;
}

const PluginInfo* PluginManager::GetPluginInfo(const wxString& pluginName)
{
    cbPlugin* plug = FindPluginByName(pluginName);
    if (plug)
        return plug->GetInfo();
        
    return NULL;
}

int PluginManager::ExecutePlugin(const wxString& pluginName)
{
    cbPlugin* plug = FindPluginByName(pluginName);
    if (plug)
    {
        if (plug->GetType() != ptTool)
        {
            MessageManager* msgMan = Manager::Get()->GetMessageManager();
            msgMan->DebugLog(_("Plugin %s is not a tool to have Execute() method!"), plug->GetInfo()->name.c_str());
        }
        else
            return ((cbToolPlugin*)plug)->Execute();
    }
	
	return 0;
}

int PluginManager::ConfigurePlugin(const wxString& pluginName)
{
    cbPlugin* plug = FindPluginByName(pluginName);
    if (plug)
        return plug->Configure();
	return 0;
}

PluginsArray PluginManager::GetToolOffers()
{
    return DoGetOffersFor(ptTool);
}

PluginsArray PluginManager::GetMimeOffers()
{
    return DoGetOffersFor(ptMime);
}

PluginsArray PluginManager::GetCompilerOffers()
{
    return DoGetOffersFor(ptCompiler);
}

PluginsArray PluginManager::GetCodeCompletionOffers()
{
	return DoGetOffersFor(ptCodeCompletion);
}

PluginsArray PluginManager::DoGetOffersFor(PluginType type)
{
    PluginsArray arr;
        
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        if (plug->GetType() == type)
            arr.Add(plug);
    }
    
    return arr;
}

void PluginManager::AskPluginsForModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        if (plug)
            plug->BuildModuleMenu(type, menu, arg);
    }
}

void PluginManager::NotifyPlugins(CodeBlocksEvent& event)
{
    /*
    A plugin might process the event we 'll send it or not. The event then
    "travels" up the chain of plugins. If one plugin (say in the middle) is
    disabled, it's not processing events and so the event "travelling" stops.
    The rest of the plugins never "hear" about this event.
    The solution is that the plugin manager checks for disabled plugins and
    posts the event not only to the last plugin but also to all plugins that
    are followed by a disabled plugin (skipping the chain-breakers that is)...
    */
    if (!m_Plugins.GetCount())
		return;

    bool sendEvt = true;
    for (unsigned int i = m_Plugins.GetCount() - 1; i > 0; --i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        if (!plug || (plug && !plug->IsAttached()))
            sendEvt = true;
        else if (sendEvt)
        {
            wxPostEvent(plug, event);
            sendEvt = false;
        }
    }
}

int PluginManager::Configure()
{
	PluginsConfigurationDlg dlg(Manager::Get()->GetAppWindow());
    if (dlg.ShowModal() == wxID_CANCEL)
        return wxID_CANCEL;

    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;

        // do not load it if the user has explicitely asked not to...
        wxString baseKey;
        baseKey << "/plugins/" << m_Plugins[i]->name;
        bool loadIt = ConfigManager::Get()->Read(baseKey, true);

        if (!loadIt && plug->IsAttached())
            plug->Release(false);
        else if (loadIt && !plug->IsAttached())
        {
            ConfigManager::Get()->Write("/plugins/try_to_load", plug->GetInfo()->title);
            plug->Attach();
        }
    }
    wxLogNull ln;
    ConfigManager::Get()->DeleteEntry("/plugins/try_to_load");
    return wxID_OK;
}

