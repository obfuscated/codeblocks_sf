/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef CODESNIPPETS_H_INCLUDED
#define CODESNIPPETS_H_INCLUDED

#include "cbplugin.h" // for "class cbPlugin"

class CodeSnippetsWindow;

class CodeSnippets : public cbPlugin
{
	public:
		/** Constructor. */
		CodeSnippets();
		/** Destructor. */
		~CodeSnippets();

		/** Invoke configuration dialog. */
		int Configure();

		/** Return the plugin's configuration priority.
		 * This is a number (default is 50) that is used to sort plugins
		 * in configuration dialogs. Lower numbers mean the plugin's
		 * configuration is put higher in the list.
		*/
		int GetConfigurationPriority() const { return 50; }

		/** Return the configuration group for this plugin. Default is cgUnknown.
		 * Notice that you can logically AND more than one configuration groups,
		 * so you could set it, for example, as "cgCompiler | cgContribPlugin".
		*/
		int GetConfigurationGroup() const { return cgUnknown; }

		/** Return plugin's configuration panel.
		  * @param parent The parent window.
		  * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
		  */
		cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent){ return 0; }

		/** Return plugin's configuration panel for projects.
		 * The panel returned from this function will be added in the project's
		 * configuration dialog.
		 * @param parent The parent window.
		 * @param project The project that is being edited.
		 * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
		*/
		cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0; }

		/** This method is called by Code::Blocks and is used by the plugin
		 * to add any menu items it needs on Code::Blocks's menu bar.\n
		 * It is a pure virtual method that needs to be implemented by all
		 * plugins. If the plugin does not need to add items on the menu,
		 * just do nothing ;)
		 * @param menuBar the wxMenuBar to create items in
		*/
		void BuildMenu(wxMenuBar* menuBar);

		/** This method is called by Code::Blocks core modules (EditorManager,
		 * ProjectManager etc) and is used by the plugin to add any menu
		 * items it needs in the module's popup menu. For example, when
		 * the user right-clicks on a project file in the project tree,
		 * ProjectManager prepares a popup menu to display with context
		 * sensitive options for that file. Before it displays this popup
		 * menu, it asks all attached plugins (by asking PluginManager to call
		 * this method), if they need to add any entries
		 * in that menu. This method is called.\n
		 * If the plugin does not need to add items in the menu,
		 * just do nothing ;)
		 * @param type the module that's preparing a popup menu
		 * @param menu pointer to the popup menu
		 * @param data pointer to FileTreeData object (to access/modify the file tree)
		*/
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}

		/** This method is called by Code::Blocks and is used by the plugin
		 * to add any toolbar items it needs on Code::Blocks's toolbar.\n
		 * It is a pure virtual method that needs to be implemented by all
		 * plugins. If the plugin does not need to add items on the toolbar,
		 * just do nothing ;)
		 * @param toolBar the wxToolBar to create items on
		 * @return The plugin should return true if it needed the toolbar, false if not
		*/
		bool BuildToolBar(wxToolBar* toolBar){ return false; }
	protected:
		/** Any descendent plugin should override this virtual method and
		 * perform any necessary initialization. This method is called by
		 * Code::Blocks (PluginManager actually) when the plugin has been
		 * loaded and should attach in Code::Blocks. When Code::Blocks
		 * starts up, it finds and <em>loads</em> all plugins but <em>does
		 * not</em> activate (attaches) them. It then activates all plugins
		 * that the user has selected to be activated on start-up.\n
		 * This means that a plugin might be loaded but <b>not</b> activated...\n
		 * Think of this method as the actual constructor...
		*/
		void OnAttach();

		/** Any descendent plugin should override this virtual method and
		 * perform any necessary de-initialization. This method is called by
		 * Code::Blocks (PluginManager actually) when the plugin has been
		 * loaded, attached and should de-attach from Code::Blocks.\n
		 * Think of this method as the actual destructor...
		 * @param appShutDown If true, the application is shutting down. In this
		 *         case *don't* use Manager::Get()->Get...() functions or the
		 *         behaviour is undefined...
		*/
		void OnRelease(bool appShutDown);

		// ---
		CodeSnippetsWindow* m_SnippetsWindow;


	private:
		void OnViewSnippets(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		DECLARE_EVENT_TABLE();
};

// Declare the plugin's hooks
CB_DECLARE_PLUGIN();

#endif // CODESNIPPETS_H_INCLUDED
