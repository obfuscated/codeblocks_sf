#ifndef DEVPLUGIN_H
#define DEVPLUGIN_H

#include <wx/dynarray.h>
#include <wx/event.h>

#include "settings.h" // build settings
#include "globals.h"

#ifdef __WXMSW__
	#ifndef PLUGIN_EXPORT
		#ifdef EXPORT_LIB
			#define PLUGIN_EXPORT __declspec (dllexport)
		#else // !EXPORT_LIB
			#if BUILDING_PLUGIN
				#define PLUGIN_EXPORT __declspec (dllexport)
			#else // !BUILDING_PLUGIN
				#define PLUGIN_EXPORT __declspec (dllimport)
			#endif // BUILDING_PLUGIN
		#endif // EXPORT_LIB
	#endif // PLUGIN_EXPORT
#else
	#define PLUGIN_EXPORT
#endif

// class decls
class ProjectBuildTarget;
class wxMenuBar;
class wxMenu;
class wxToolBar;
class cbProject;

/** Information about the plugin */
struct PluginInfo
{
    wxString name;
    wxString title;
    wxString version;
    wxString description;
    wxString author;
    wxString authorEmail;
    wxString authorWebsite;
    wxString thanksTo;
    wxString license;
	bool hasConfigure;
};

/**
  * Base class for plugins. This is the minimum class a plugin must descend
  * from.
  * cbPlugin descends from wxEvtHandler, so it provides its methods as well...
  * \n \n
  * If a plugin has PluginInfo::hasConfigure set to true, this means that
  * the plugin provides a configuration dialog of some sort. In this case,
  * Code::Blocks creates a menu entry for the plugin, under the Settings
  * menu, to configure it. See Configure() for more information.
  */
class cbPlugin : public wxEvtHandler
{
    public:
		/** In default cbPlugin's constructor the associated PluginInfo structure
		  * is filled with default values. If you inherit from cbPlugin, you
		  * should fill the m_PluginInfo members with the appropriate values.
		  */
        cbPlugin();
		/** cbPlugin destructor. */
        virtual ~cbPlugin();
		/** Attach is <u>not</u> a virtual function, so you can't override it.
		  * The default implementation hooks the plugin to Code::Block's
		  * event handling system, so that the plugin can receive (and process)
		  * events from Code::Blocks core library. Use OnAttach() for any
		  * initialization specific tasks.
		  * @see OnAttach()
		  */
        void Attach();
		/** Release is <u>not</u> a virtual function, so you can't override it.
		  * The default implementation un-hooks the plugin from Code::Blocks's
		  * event handling system. Use OnRelease() for any clean-up specific
		  * tasks.
		  * @see OnRelease()
		  */
        void Release();
		/** The plugin must return its type on request. */
        virtual PluginType GetType(){ return m_Type; }
		
		/** The plugin must return its info on request. */
        virtual PluginInfo const* GetInfo(){ return &m_PluginInfo; }
		/** This is a pure virtual method that should be overriden by all
		  * plugins. If a plugin provides some sort of configuration dialog,
		  * this is the place to invoke it. If it does not support/allow
		  * configuration, just return 0.
		  */
        virtual int Configure() = 0;
		/** This method is called by Code::Blocks and is used by the plugin
		  * to add any menu items it needs on Code::Blocks's menu bar.\n
		  * It is a pure virtual method that needs to be implemented by all
		  * plugins. If the plugin does not need to add items on the menu,
		  * just do nothing ;)
		  * @param menuBar the wxMenuBar to create items in
		  */
        virtual void BuildMenu(wxMenuBar* menuBar) = 0;
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
		  * @param arg a wxString argument. In the example above, it would contain the selected project file
		  */
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg) = 0;
		/** This method is called by Code::Blocks and is used by the plugin
		  * to add any toolbar items it needs on Code::Blocks's toolbar.\n
		  * It is a pure virtual method that needs to be implemented by all
		  * plugins. If the plugin does not need to add items on the toolbar,
		  * just do nothing ;)
		  * @param toolBar the wxToolBar to create items on
		  */
        virtual void BuildToolBar(wxToolBar* toolBar) = 0;
        /** See whether this plugin is attached or not. A plugin should not perform
		  * any of its tasks, if not attached...
		  */
		bool IsAttached(){ return m_IsAttached; }
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
        virtual void OnAttach(){}
		
		/** Any descendent plugin should override this virtual method and
		  * perform any necessary de-initialization. This method is called by
		  * Code::Blocks (PluginManager actually) when the plugin has been
		  * loaded, attached and should de-attach from Code::Blocks.\n
		  * Think of this method as the actual destructor...
		  */
        virtual void OnRelease(){}
		
		/** This method logs a "Not implemented" message and is provided for
		  * convenience only. For example, if the plugin *will* provide a
		  * configuration dialog, but it's not implemented yet, the author
		  * should create the Configure() method and put in there something
		  * like: NotImplemented("myCustomPlugin::OnConfigure"). This would
		  * log this message: "myCustomPlugin::OnConfigure : Not implemented"
		  * in the Code::Blocks debug log.
		  */
        virtual void NotImplemented(const wxString& log);
		/** Holds the PluginInfo structure that describes the plugin. */
        PluginInfo m_PluginInfo;
		/** Holds the plugin's type. Set in the default constructor. You shouldn't change it. */
        PluginType m_Type;
		/** Holds the "attached" state. */
        bool m_IsAttached;
	private:
};

class cbCompilerPlugin: public cbPlugin
{
	public:
		cbCompilerPlugin();
        virtual int Run(ProjectBuildTarget* target = 0L) = 0;
        virtual int Clean(ProjectBuildTarget* target = 0L) = 0;
        virtual int Compile(ProjectBuildTarget* target = 0L) = 0;
        virtual int CompileAll() = 0;
        virtual int RebuildAll() = 0;
        virtual int Rebuild(ProjectBuildTarget* target = 0L) = 0;
        virtual int CompileFile(const wxString& file) = 0;
        virtual int KillProcess() = 0;
		virtual bool IsRunning() = 0;
		virtual int GetExitCode() = 0;
		virtual int Configure(){ return Configure(0L, 0L); }
		virtual int Configure(cbProject* project, ProjectBuildTarget* target = 0L) = 0;
	private:
};

class  cbDebuggerPlugin: public cbPlugin
{
	public:
		cbDebuggerPlugin();
		virtual int Debug() = 0;
		virtual void CmdContinue() = 0;
		virtual void CmdNext() = 0;
		virtual void CmdStep() = 0;
		virtual void CmdStop() = 0;
		virtual bool IsRunning() = 0;
		virtual int GetExitCode() = 0;
};

class cbToolPlugin : public cbPlugin
{
    public:
        cbToolPlugin();
        virtual int Execute() = 0;
    private:
        // "Hide" some virtual members, that are not needed in cbToolPlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){}
        void BuildToolBar(wxToolBar* toolBar){}
};

class cbMimePlugin : public cbPlugin
{
    public:
        cbMimePlugin();
        virtual bool CanOpenFile(const wxString& filename) = 0;
        virtual int OpenFile(const wxString& filename) = 0;
    private:
        // "Hide" some virtual members, that are not needed in cbMimePlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){}
        void BuildToolBar(wxToolBar* toolBar){}
};

class  cbCodeCompletionPlugin : public cbPlugin
{
    public:
        cbCodeCompletionPlugin();
		virtual wxArrayString GetCallTips() = 0;
		virtual int CodeComplete() = 0;
		virtual void ShowCallTip() = 0;
};

typedef cbPlugin*(*GetPluginProc)(void);

#endif // DEVPLUGIN_H

