/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef SPELLCHECKER_H_INCLUDED
#define SPELLCHECKER_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h> // for "class cbPlugin"
class wxSpellCheckEngineInterface;
class SpellCheckHelper;
class OnlineSpellChecker;
class Thesaurus;
class SpellCheckerConfig;
class MySpellingDialog;
class SpellCheckerStatusField;

class SpellCheckerPlugin : public cbPlugin
{
    public:
        /** Constructor. */
        SpellCheckerPlugin();
        /** Destructor. */
        virtual ~SpellCheckerPlugin();

        /** Invoke configuration dialog. */
        virtual int Configure();

        /** Return the plugin's configuration priority.
          * This is a number (default is 50) that is used to sort plugins
          * in configuration dialogs. Lower numbers mean the plugin's
          * configuration is put higher in the list.
          */
        virtual int GetConfigurationPriority() const { return 50; }

        /** Return the configuration group for this plugin. Default is cgUnknown.
          * Notice that you can logically OR more than one configuration groups,
          * so you could set it, for example, as "cgCompiler | cgContribPlugin".
          */
        virtual int GetConfigurationGroup() const { return cgEditor; }

        /** Return plugin's configuration panel.
          * @param parent The parent window.
          * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
          */
        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

        /** Return plugin's configuration panel for projects.
          * The panel returned from this function will be added in the project's
          * configuration dialog.
          * @param parent The parent window.
          * @param project The project that is being edited.
          * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
          */
        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0; }

        /** This method is called by Code::Blocks and is used by the plugin
          * to add any menu items it needs on Code::Blocks's menu bar.\n
          * It is a pure virtual method that needs to be implemented by all
          * plugins. If the plugin does not need to add items on the menu,
          * just do nothing ;)
          * @param menuBar the wxMenuBar to create items in
          */
        virtual void BuildMenu(wxMenuBar* menuBar);

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
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);

        /** This method is called by Code::Blocks and is used by the plugin
          * to add any toolbar items it needs on Code::Blocks's toolbar.\n
          * It is a pure virtual method that needs to be implemented by all
          * plugins. If the plugin does not need to add items on the toolbar,
          * just do nothing ;)
          * @param toolBar the wxToolBar to create items on
          * @return The plugin should return true if it needed the toolbar, false if not
          */
        virtual bool BuildToolBar(wxToolBar* toolBar);

        #ifdef wxUSE_STATUSBAR
        virtual void CreateStatusField(cbStatusBar *bar);
        #endif

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
        virtual void OnAttach();

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnRelease(bool appShutDown);


    public:
        void ReloadSettings();
        static wxString GetOnlineCheckerConfigPath();

    private:
        void OnSpelling(wxCommandEvent &event);
        void OnUpdateSpelling(wxUpdateUIEvent &event);
        void OnUpdateThesaurus(wxUpdateUIEvent &event);
        void OnThesaurus(wxCommandEvent &event);
        void OnReplaceBySuggestion(wxCommandEvent &event);
        void OnMoreSuggestions(wxCommandEvent &event);
        void OnAddToPersonalDictionary(wxCommandEvent &event);

        void ConfigureHunspellSpellCheckEngine();
        void ConfigureThesaurus();
        void ConfigurePersonalDictionary();
        void SavePersonalDictionary();
        void OnEditorSaved(CodeBlocksEvent& event);
        //void OnRightUpOverStatus(wxMouseEvent &event);

    public:
        void EditPersonalDictionary();

    private:
        int m_FunctorId;
        wxSpellCheckEngineInterface* m_pSpellChecker;
        MySpellingDialog *m_pSpellingDialog;
        SpellCheckHelper *m_pSpellHelper;
        OnlineSpellChecker *m_pOnlineChecker;
        Thesaurus *m_pThesaurus;
        SpellCheckerConfig *m_sccfg;

        wxArrayString m_suggestions;
        int m_wordstart, m_wordend;
#ifdef wxUSE_STATUSBAR
    private:
        SpellCheckerStatusField *m_fld;
#endif

    private:
        DECLARE_EVENT_TABLE();
};

#endif // SPELLCHECKER_H_INCLUDED
