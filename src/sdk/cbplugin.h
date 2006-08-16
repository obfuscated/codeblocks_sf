#ifndef CBPLUGIN_H
#define CBPLUGIN_H

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/intl.h>
#include <wx/string.h>

#include "settings.h" // build settings
#include "globals.h"

#ifdef __WXMSW__
	#ifndef PLUGIN_EXPORT
		#ifdef EXPORT_LIB
			#define PLUGIN_EXPORT __declspec (dllexport)
		#else // !EXPORT_LIB
			#ifdef BUILDING_PLUGIN
				#define PLUGIN_EXPORT __declspec (dllexport)
			#else // !BUILDING_PLUGIN
				#define PLUGIN_EXPORT __declspec (dllimport)
			#endif // BUILDING_PLUGIN
		#endif // EXPORT_LIB
	#endif // PLUGIN_EXPORT
#else
	#define PLUGIN_EXPORT
#endif

// this is the plugins SDK version number
// it will change when the SDK interface breaks
#define PLUGIN_SDK_VERSION_MAJOR 1
#define PLUGIN_SDK_VERSION_MINOR 9
#define PLUGIN_SDK_VERSION_RELEASE 0

// class decls
class wxMenuBar;
class wxMenu;
class wxToolBar;
class wxPanel;
class wxWindow;
class cbEditor;
class cbProject;
class ProjectBuildTarget;
class CompileTargetBase;
class FileTreeData;
class cbConfigurationPanel;

// Define basic groups for plugins' configuration.
static const int cgCompiler         = 0x01; ///< Compiler related.
static const int cgDebugger         = 0x02; ///< Debugger related.
static const int cgEditor           = 0x04; ///< Editor related.
static const int cgCorePlugin       = 0x08; ///< One of the core plugins.
static const int cgContribPlugin    = 0x10; ///< One of the contrib plugins (or any third-party plugin for that matter).
static const int cgUnknown          = 0x20; ///< Unknown. This will be probably grouped with cgContribPlugin.

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
};

/** @brief Base class for plugins
  * This is the most basic class a plugin must descend
  * from.
  * cbPlugin descends from wxEvtHandler, so it provides its methods as well...
  * \n \n
  * If a plugin has PluginInfo::hasConfigure set to true, this means that
  * the plugin provides a configuration dialog of some sort. In this case,
  * Code::Blocks creates a menu entry for the plugin, under the Settings
  * menu, to configure it. See Configure() for more information.
  */
class PLUGIN_EXPORT cbPlugin : public wxEvtHandler
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
		  * @param appShutDown If true, the application is shutting down. In this
		  *         case *don't* use Manager::Get()->Get...() functions or the
		  *         behaviour is undefined...
		  * @see OnRelease()
		  */
        void Release(bool appShutDown);
		/** The plugin must return its type on request. */
        virtual PluginType GetType() const { return m_Type; }

		/** The plugin must return its info on request. */
        virtual PluginInfo const* GetInfo() const { return &m_PluginInfo; }
		/** If a plugin provides some sort of configuration dialog,
		  * this is the place to invoke it.
		  */
        virtual int Configure(){ return 0; }
        /** Return the plugin's configuration priority.
          * This is a number (default is 50) that is used to sort plugins
          * in configuration dialogs. Lower numbers mean the plugin's
          * configuration is put higher in the list.
          */
        virtual int GetConfigurationPriority() const { return 50; }
        /** Return the configuration group for this plugin. Default is cgUnknown.
          * Notice that you can logically AND more than one configuration groups,
          * so you could set it, for example, as "cgCompiler | cgContribPlugin".
          */
        virtual int GetConfigurationGroup() const { return cgUnknown; }
		/** Return plugin's configuration panel.
		  * @param parent The parent window.
		  * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
		  */
        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent){ return 0; }
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
		  * @param data pointer to FileTreeData object (to access/modify the file tree)
		  */
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0) = 0;
		/** This method is called by Code::Blocks and is used by the plugin
		  * to add any toolbar items it needs on Code::Blocks's toolbar.\n
		  * It is a pure virtual method that needs to be implemented by all
		  * plugins. If the plugin does not need to add items on the toolbar,
		  * just do nothing ;)
		  * @param toolBar the wxToolBar to create items on
		  * @return The plugin should return true if it needed the toolbar, false if not
		  */
        virtual bool BuildToolBar(wxToolBar* toolBar) = 0;
        /** See whether this plugin is attached or not. A plugin should not perform
		  * any of its tasks, if not attached...
		  */
		bool IsAttached() const { return m_IsAttached; }
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
		  * @param appShutDown If true, the application is shutting down. In this
		  *         case *don't* use Manager::Get()->Get...() functions or the
		  *         behaviour is undefined...
		  */
        virtual void OnRelease(bool appShutDown){}

		/** This method logs a "Not implemented" message and is provided for
		  * convenience only.
		  */
        virtual void NotImplemented(const wxString& log) const;
		/** Holds the PluginInfo structure that describes the plugin. */
        PluginInfo m_PluginInfo;
		/** Holds the plugin's type. Set in the default constructor. You shouldn't change it. */
        PluginType m_Type;
		/** Holds the "attached" state. */
        bool m_IsAttached;
	private:
};

/** @brief Base class for compiler plugins
  *
  * This plugin type must offer some pre-defined build facilities, on top
  * of the generic plugin's.
  */
class PLUGIN_EXPORT cbCompilerPlugin: public cbPlugin
{
	public:
		cbCompilerPlugin();

		/** @brief Run the project/target.
		  *
		  * Running a project means executing its build output. Of course
		  * this depends on the selected build target and its type.
		  *
		  * @param target The specific build target to "run". If NULL, the plugin
		  * should ask the user which target to "run" (except maybe if there is
		  * only one build target in the project).
		  */
        virtual int Run(ProjectBuildTarget* target = 0L) = 0;
        /** Same as Run(ProjectBuildTarget*) but with a wxString argument. */
        virtual int Run(const wxString& target) = 0;

		/** @brief Clean the project/target.
		  *
		  * Cleaning a project means deleting any files created by building it.
		  * This includes any object files, the binary output file, etc.
		  *
		  * @param target The specific build target to "clean". If NULL, it
		  * cleans all the build targets of the current project.
		  */
        virtual int Clean(ProjectBuildTarget* target = 0L) = 0;
        /** Same as Clean(ProjectBuildTarget*) but with a wxString argument. */
        virtual int Clean(const wxString& target) = 0;

		/** @brief DistClean the project/target.
		  *
		  * DistClean will typically remove any config files
		  * and anything else that got created as part of
		  * building a software package.
		  *
		  * @param target The specific build target to "distclean". If NULL, it
		  * cleans all the build targets of the current project.
		  */
        virtual int DistClean(ProjectBuildTarget* target = 0L) = 0;
        /** Same as DistClean(ProjectBuildTarget*) but with a wxString argument. */
        virtual int DistClean(const wxString& target) = 0;

		/** @brief Build the project/target.
		  *
		  * @param target The specific build target to build. If NULL, it
		  * builds all the targets of the current project.
		  */
        virtual int Build(ProjectBuildTarget* target = 0L) = 0;
        /** Same as Build(ProjectBuildTarget*) but with a wxString argument. */
        virtual int Build(const wxString& target) = 0;

		/** @brief Rebuild the project/target.
		  *
		  * Rebuilding a project is equal to calling Clean() and then Build().
		  * This makes sure that all compilable files in the project will be
		  * compiled again.
		  *
		  * @param target The specific build target to rebuild. If NULL, it
		  * rebuilds all the build targets of the current project.
		  */
        virtual int Rebuild(ProjectBuildTarget* target = 0L) = 0;
        /** Same as Rebuild(ProjectBuildTarget*) but with a wxString argument. */
        virtual int Rebuild(const wxString& target) = 0;

		/** @brief Build all open projects.
		  * @param target If not empty, the target to build in each project. Else all targets.
		  */
        virtual int BuildWorkspace(const wxString& target = wxEmptyString) = 0;

		/** @brief Rebuild all open projects.
		  * @param target If not empty, the target to rebuild in each project. Else all targets.
		  */
        virtual int RebuildWorkspace(const wxString& target = wxEmptyString) = 0;

		/** @brief Clean all open projects.
		  * @param target If not empty, the target to clean in each project. Else all targets.
		  */
        virtual int CleanWorkspace(const wxString& target = wxEmptyString) = 0;

        /** @brief Compile a specific file.
          *
          * @param file The file to compile (must be a project file!)
          */
        virtual int CompileFile(const wxString& file) = 0;

        /** @brief Abort the current build process. */
        virtual int KillProcess() = 0;

        /** @brief Is the plugin currently compiling? */
		virtual bool IsRunning() const = 0;

        /** @brief Get the exit code of the last build process. */
		virtual int GetExitCode() const = 0;

        /** @brief Display configuration dialog.
          *
          * @param project The selected project (can be NULL).
          * @param target The selected target (can be NULL).
          */
		virtual int Configure(cbProject* project, ProjectBuildTarget* target = 0L) = 0;
	private:
};

/** @brief Base class for debugger plugins
  *
  * This plugin type must offer some pre-defined debug facilities, on top
  * of the generic plugin's.
  */
class PLUGIN_EXPORT cbDebuggerPlugin: public cbPlugin
{
	public:
		cbDebuggerPlugin();
		/** @brief Request to add a breakpoint.
		  * @param file The file to add the breakpoint based on a file/line pair.
		  * @param line The line number to put the breakpoint in @c file.
		  * @return True if succeeded, false if not.
		  */
		virtual bool AddBreakpoint(const wxString& file, int line) = 0;
		/** @brief Request to add a breakpoint based on a function signature.
		  * @param functionSignature The function signature to add the breakpoint.
		  * @return True if succeeded, false if not.
		  */
		virtual bool AddBreakpoint(const wxString& functionSignature) = 0;
		/** @brief Request to remove a breakpoint based on a file/line pair.
		  * @param file The file to remove the breakpoint.
		  * @param line The line number the breakpoint is in @c file.
		  * @return True if succeeded, false if not.
		  */
		virtual bool RemoveBreakpoint(const wxString& file, int line) = 0;
		/** @brief Request to remove a breakpoint based on a function signature.
		  * @param functionSignature The function signature to remove the breakpoint.
		  * @return True if succeeded, false if not.
		  */
		virtual bool RemoveBreakpoint(const wxString& functionSignature) = 0;
		/** @brief Request to remove all breakpoints from a file.
		  * @param file The file to remove all breakpoints in. If the argument is empty, all breakpoints are removed from all files.
		  * @return True if succeeded, false if not.
		  */
		virtual bool RemoveAllBreakpoints(const wxString& file = wxEmptyString) = 0;
		/** @brief Notify the debugger that lines were added or removed in an editor.
		  * This causes the debugger to keep the breakpoints list in-sync with the
		  * editors (i.e. what the user sees).
		  * @param editor The editor in question.
		  * @param startline The starting line this change took place.
		  * @param lines The number of lines added or removed. If it's a positive number,
		  *              lines were added. If it's a negative number, lines were removed.
		  */
		virtual void EditorLinesAddedOrRemoved(cbEditor* editor, int startline, int lines) = 0;
		/** @brief Start a new debugging process. */
		virtual int Debug() = 0;
		/** @brief Continue running the debugged program. */
		virtual void Continue() = 0;
		/** @brief Execute the next instruction and return control to the debugger. */
		virtual void Next() = 0;
		/** @brief Execute the next instruction, stepping into function calls if needed, and return control to the debugger. */
		virtual void Step() = 0;
		/** @brief Stop the debugging process. */
		virtual void Stop() = 0;
        /** @brief Is the plugin currently debugging? */
		virtual bool IsRunning() const = 0;
        /** @brief Get the exit code of the last debug process. */
		virtual int GetExitCode() const = 0;
};

/** @brief Base class for tool plugins
  *
  * This plugin is automatically managed by Code::Blocks, so the inherited
  * functions to build menus/toolbars are hidden.
  *
  * Tool plugins are automatically added under the "Plugins" menu.
  */
class PLUGIN_EXPORT cbToolPlugin : public cbPlugin
{
    public:
        cbToolPlugin();
        /** @brief Execute the plugin.
          *
          * This is the only function needed by a cbToolPlugin.
          * This will be called when the user selects the plugin from the "Plugins"
          * menu.
          */
        virtual int Execute() = 0;
    private:
        // "Hide" some virtual members, that are not needed in cbToolPlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void RemoveMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
        void RemoveToolBar(wxToolBar* toolBar){}
};

/** @brief Base class for mime plugins
  *
  * Mime plugins are called by Code::Blocks to operate on files that Code::Blocks
  * wouldn't know how to handle on itself.
  */
class PLUGIN_EXPORT cbMimePlugin : public cbPlugin
{
    public:
        cbMimePlugin();
        /** @brief Can a file be handled by this plugin?
          *
          * @param filename The file in question.
          * @return The plugin should return true if it can handle this file,
          * false if not.
          */
        virtual bool CanHandleFile(const wxString& filename) const = 0;
        /** @brief Open the file.
          *
          * @param filename The file to open.
          * @return The plugin should return zero on success, other value on error.
          */
        virtual int OpenFile(const wxString& filename) = 0;
        /** @brief Is this a default handler?
          *
          * This is a flag notifying the main app that this plugin can handle
          * every file passed to it. Usually you 'll want to return false in
          * this function, because you usually create specialized handler
          * plugins (for specific MIME types)...
          *
          * @return True if this plugin can handle every possible MIME type,
          * false if not.
          */
        virtual bool HandlesEverything() const = 0;
    private:
        // "Hide" some virtual members, that are not needed in cbMimePlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void RemoveMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
        void RemoveToolBar(wxToolBar* toolBar){}
};

/** @brief Base class for code-completion plugins
  *
  * This interface is subject to change, so not much info here...
  */
class PLUGIN_EXPORT cbCodeCompletionPlugin : public cbPlugin
{
    public:
        cbCodeCompletionPlugin();
		virtual wxArrayString GetCallTips() = 0;
		virtual int CodeComplete() = 0;
		virtual void ShowCallTip() = 0;
};

/** @brief Base class for wizard plugins
  *
  * Wizard plugins are called by Code::Blocks when the user selects
  * "File->New...".
  *
  * A plugin of this type can support more than one wizard. Additionally,
  * each wizard can support new workspaces, new projects, new targets or new files.
  * The @c index used as a parameter to most of the functions, denotes 0-based index
  * of the project wizard to run.
  */
class PLUGIN_EXPORT cbWizardPlugin : public cbPlugin
{
    public:
        cbWizardPlugin();

        /** @return the number of template wizards this plugin contains */
        virtual int GetCount() const = 0;
        /** @param index the wizard index.
          * @return the output type of the specified wizard at @c index */
        virtual TemplateOutputType GetOutputType(int index) const = 0;
        /** @param index the wizard index.
          * @return the template's title */
        virtual wxString GetTitle(int index) const = 0;
        /** @param index the wizard index.
          * @return the template's description */
        virtual wxString GetDescription(int index) const = 0;
        /** @param index the wizard index.
          * @return the template's category (GUI, Console, etc; free-form text). Try to adhere to standard category names... */
        virtual wxString GetCategory(int index) const = 0;
        /** @param index the wizard index.
          * @return the template's bitmap */
        virtual const wxBitmap& GetBitmap(int index) const = 0;
        /** @param index the wizard index.
          * @return this wizard's script filename (if this wizard is scripted). */
        virtual wxString GetScriptFilename(int index) const = 0;
        /** When this is called, the wizard must get to work ;).
          * @param index the wizard index.
          * @param createdFilename if provided, on return it should contain the main filename
          *                         this wizard created. If the user created a project, that
          *                         would be the project's filename.
          *                         If the wizard created a build target, that would be an empty string.
          *                         If the wizard created a file, that would be the file's name.
          * @return a pointer to the generated cbProject or ProjectBuildTarget. NULL for everything else (failure too).
          * You should dynamic-cast this to the correct type based on GetOutputType() 's value. */
        virtual CompileTargetBase* Launch(int index, wxString* createdFilename = 0) = 0; // do your work ;)
    private:
        // "Hide" some virtual members, that are not needed in cbCreateWizardPlugin
        void BuildMenu(wxMenuBar* menuBar){}
        void RemoveMenu(wxMenuBar* menuBar){}
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
        void RemoveToolBar(wxToolBar* toolBar){}
};

typedef void(*PluginSDKVersionProc)(int*,int*,int*);
typedef size_t(*GetPluginsCountProc)(void);
typedef wxString(*PluginNameProc)(size_t);
typedef cbPlugin*(*CreatePluginProc)(size_t);
typedef void(*FreePluginProc)(cbPlugin*);

/** This is used to declare the plugin's hooks.
  */
#define CB_DECLARE_PLUGIN() \
    extern "C" \
    { \
        PLUGIN_EXPORT wxString PluginName(size_t index); \
        PLUGIN_EXPORT size_t GetPluginsCount(); \
        PLUGIN_EXPORT cbPlugin* CreatePlugin(size_t index); \
        PLUGIN_EXPORT void FreePlugin(cbPlugin* plugin); \
        PLUGIN_EXPORT void PluginSDKVersion(int* major, int* minor, int* release); \
    }

/** This is used to actually implement the plugin's hooks.
  * Implements and exports one plugin of class @c name.
  * @param name The plugin's name (class).
  * @param title The plugin's title (string).
  */
#define CB_IMPLEMENT_PLUGIN(name,title) \
    wxString PluginName(size_t index){ return _T(title); } \
    size_t GetPluginsCount(){ return 1; } \
    cbPlugin* CreatePlugin(size_t index) { return new name; } \
    void FreePlugin(cbPlugin* plugin){ delete plugin; } \
    void PluginSDKVersion(int* major, int* minor, int* release) \
    { \
        if (major) *major = PLUGIN_SDK_VERSION_MAJOR; \
        if (minor) *minor = PLUGIN_SDK_VERSION_MINOR; \
        if (release) *release = PLUGIN_SDK_VERSION_RELEASE; \
    }

/** Synonym to CB_IMPLEMENT_PLUGIN.
  * Implements and exports ONE plugin of class @c name.
  * @param name The plugin's name (class).
  * @param title The plugin's title (string).
  */
#define CB_IMPLEMENT_PLUGINS_1(name,title) \
    wxString PluginName(size_t index){ return _T(title); } \
    size_t GetPluginsCount(){ return 1; } \
    cbPlugin* CreatePlugin(size_t index) { return new name; } \
    void FreePlugin(cbPlugin* plugin){ delete plugin; } \
    void PluginSDKVersion(int* major, int* minor, int* release) \
    { \
        if (major) *major = PLUGIN_SDK_VERSION_MAJOR; \
        if (minor) *minor = PLUGIN_SDK_VERSION_MINOR; \
        if (release) *release = PLUGIN_SDK_VERSION_RELEASE; \
    }

/** Used to export more than one plugin from the same library.
  * Implements and exports TWO plugins of class @c name1 and @c name2.
  * @param name1 The first plugin's name (class).
  * @param title1 The first plugin's title (string).
  * @param name2 The second plugin's name (class).
  * @param title2 The second plugin's title (string).
  */
#define CB_IMPLEMENT_PLUGINS_2(name1,title1,name2,title2) \
    wxString PluginName(size_t index) \
    { \
        switch (index) \
        { \
            case 0: return _T(title1);  \
            case 1: return _T(title2);  \
            default: cbThrow(_T("Invalid plugin index in PluginName()!")); \
        } \
    } \
    size_t GetPluginsCount(){ return 2; } \
    cbPlugin* CreatePlugin(size_t index) \
    { \
        switch(index) \
        { \
            case 0: return new name1; \
            case 1: return new name2; \
            default: cbThrow(_T("Invalid plugin index in CreatePlugin()!")); \
        } \
    } \
    void FreePlugin(cbPlugin* plugin){ delete plugin; } \
    void PluginSDKVersion(int* major, int* minor, int* release) \
    { \
        if (major) *major = PLUGIN_SDK_VERSION_MAJOR; \
        if (minor) *minor = PLUGIN_SDK_VERSION_MINOR; \
        if (release) *release = PLUGIN_SDK_VERSION_RELEASE; \
    }

/** Used to export more than one plugin from the same library.
  * Implements and exports THREE plugins of class @c name1, @c name2 and @c name3.
  * @param name1 The first plugin's name (class).
  * @param title1 The first plugin's title (string).
  * @param name2 The second plugin's name (class).
  * @param title2 The second plugin's title (string).
  * @param name3 The third plugin's name (class).
  * @param title3 The third plugin's title (string).
  */
#define CB_IMPLEMENT_PLUGINS_3(name1,title1,name2,title2,name3,title3) \
    wxString PluginName(size_t index) \
    { \
        switch (index) \
        { \
            case 0: return _T(title1);  \
            case 1: return _T(title2);  \
            case 2: return _T(title3);  \
            default: cbThrow(_T("Invalid plugin index in PluginName()!")); \
        } \
    } \
    size_t GetPluginsCount(){ return 3; } \
    cbPlugin* CreatePlugin(size_t index) \
    { \
        switch(index) \
        { \
            case 0: return new name1; \
            case 1: return new name2; \
            case 2: return new name3; \
            default: cbThrow(_T("Invalid plugin index in CreatePlugin()!")); \
        } \
    } \
    void FreePlugin(cbPlugin* plugin){ delete plugin; } \
    void PluginSDKVersion(int* major, int* minor, int* release) \
    { \
        if (major) *major = PLUGIN_SDK_VERSION_MAJOR; \
        if (minor) *minor = PLUGIN_SDK_VERSION_MINOR; \
        if (release) *release = PLUGIN_SDK_VERSION_RELEASE; \
    }

#endif // CBPLUGIN_H
