/***************************************************************
 * Name:      EditorTweaks
 * Purpose:   Code::Blocks plugin
 * Author:     ()
 * Created:   2009-12-08
 * Copyright:
 * License:   GPL
 **************************************************************/

#ifndef EDITORTWEAKS_H_INCLUDED
#define EDITORTWEAKS_H_INCLUDED

#include <cbplugin.h> // for "class cbPlugin"
#include <vector>
#include <wx/string.h>

class AlignerMenuEntry
{
    public:
        int UsageCount;
        int id;
        wxString MenuName;
        wxString ArgumentString;
};

class EditorTweaks : public cbPlugin
{
    public:
        /** Constructor. */
        EditorTweaks();
        /** Destructor. */
        virtual ~EditorTweaks();
        int GetConfigurationPriority() const {return 50;}
        int GetConfigurationGroup() const {return cgEditor;}
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        //cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow*  /*parent*/, cbProject* /*project*/){return 0;}


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
        virtual bool BuildToolBar(wxToolBar* /*toolBar*/){ return false; }

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

        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);
        void OnEditorUpdateUI(CodeBlocksEvent& event);
        void OnEditorActivate(CodeBlocksEvent& event);
        void OnEditorDeactivate(CodeBlocksEvent& event);

        void OnKeyPress(wxKeyEvent& event);
        void OnChar(wxKeyEvent& event);
        void OnWordWrap(wxCommandEvent &event);
        void OnShowLineNumbers(wxCommandEvent &event);
        void OnTabChar(wxCommandEvent &event);
        void OnTabIndent(wxCommandEvent &event);
        void OnTabSize2(wxCommandEvent &event);
        void OnTabSize4(wxCommandEvent &event);
        void OnTabSize6(wxCommandEvent &event);
        void OnTabSize8(wxCommandEvent &event);
        void OnShowEOL(wxCommandEvent &event);
        void OnStripTrailingBlanks(wxCommandEvent &event);
        void OnEnsureConsistentEOL(wxCommandEvent &event);
        void OnEOLCRLF(wxCommandEvent &event);
        void OnEOLCR(wxCommandEvent &event);
        void OnEOLLF(wxCommandEvent &event);

        void OnFold(wxCommandEvent &event);
        void OnUnfold(wxCommandEvent &event);
        void DoFoldAboveLevel(int level, int fold);

        void OnUpdateUI(wxUpdateUIEvent &event);
        void UpdateUI();

//        void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);
    private:
		void OnAlign(wxCommandEvent& event);
		void OnAlignOthers(wxCommandEvent& event);
        void OnAlignAuto(wxCommandEvent& event);
		void OnSuppressInsert(wxCommandEvent& event);
		void OnConvertBraces(wxCommandEvent& event);
		void AlignToString(const wxString AlignmentString);
		wxString GetPadding(const wxString& Padding, const int Count);
		bool GetSelectionLines(int& LineStart, int& LineEnd);
        void DoBufferEditorPos(int delta = 0, bool isScrollTimer = false);
        void OnScrollTimer(wxTimerEvent& event);

		std::vector<AlignerMenuEntry> AlignerMenuEntries;

    private:
        int  m_EditorHookId;
        bool m_suppress_insert;
        bool m_convert_braces;
        int  m_buffer_caret;
        wxMenu *m_tweakmenu;
        wxMenuItem *m_tweakmenuitem;
        wxTimer m_scrollTimer;

        DECLARE_EVENT_TABLE();
};

#endif // EDITORTWEAKS_H_INCLUDED
