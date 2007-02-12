/*
* This file is part of Code::Blocks Studio, and licensed under the terms of the
* GNU General Public License version 2, or (at your option) any later version.
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*/

#ifndef MANAGER_H
#define MANAGER_H

#include <wx/event.h>
#include "settings.h"

// forward decls
class wxFrame;
class ProjectManager;
class EditorManager;
class MessageManager;
class PluginManager;
class ToolsManager;
class MacrosManager;
class PersonalityManager;
class wxMenu;
class wxMenuBar;
class wxToolBar;
class UserVariableManager;
class ScriptingManager;
class ConfigManager;
class FileManager;
class CodeBlocksEvent;



class DLLIMPORT Manager
{
    bool SendEventTo(wxEvtHandler* handler, CodeBlocksEvent& event);
    void OnMenu(wxCommandEvent& event);
    wxFrame* m_pAppWindow;
    static bool appShuttingDown;
    static bool blockYields;
    static bool isBatch;

    Manager();
    ~Manager();

public:
    static void SetBatchBuild(bool isBatch);
    static bool IsBatchBuild(){ return isBatch; }
    /// Blocks/unblocks Manager::Yield(). Be carefull when using it. Actually, do *not* use it ;)
    static void BlockYields(bool block);
    /// Whenever you need to call wxYield(), call Manager::Yield(). It's safer.
    static void Yield();
    static void ProcessPendingEvents();
    static void Shutdown();
    bool ProcessEvent(CodeBlocksEvent& event);


    /** Use Manager::Get() to get a pointer to its instance
     * Manager::Get() is guaranteed to never return an invalid pointer.
     */
    static Manager* Get();

    wxFrame* GetAppWindow() const;

    static bool IsAppShuttingDown();
    static bool isappShuttingDown(){return Manager::IsAppShuttingDown();};

    /** Functions returning pointers to the respective sub-manager instances.
     * During application startup as well as during runtime, these functions will always return a valid pointer.
     * During application shutdown, these functions will continue to return a valid pointer until the requested manager shuts down.
     * From that point, the below functions will return null. If there is any chance that your code might execute
     * during application shutdown, you MUST check for a null pointer.
     * The one notable exception to this rule is ConfigManager, which has the same lifetime as Manager itself.
     *
     * The order of destruction is:
     * ----------------------------
     *   ToolsManager,       TemplateManager, PluginManager,
	 *   ScriptingManager,   ProjectManager,  EditorManager,
	 *   PersonalityManager, MacrosManager,   UserVariableManager,
	 *   MessageManager
	 *   The ConfigManager is destroyed immediately before the applicaton terminates, so it can be
	 *   considered being omnipresent.
     *
     * For plugin developers, this means that most managers (except for the ones you probably don't use anyway)
     * will be available throughout the entire lifetime of your plugins.
     */

    ProjectManager* GetProjectManager() const;
    EditorManager* GetEditorManager() const;
    MessageManager* GetMessageManager() const;
    PluginManager* GetPluginManager() const;
    ToolsManager* GetToolsManager() const;
    MacrosManager* GetMacrosManager() const;
    PersonalityManager* GetPersonalityManager() const;
    UserVariableManager* GetUserVariableManager() const;
    ScriptingManager* GetScriptingManager() const;
    ConfigManager* GetConfigManager(const wxString& name_space) const;
    FileManager* GetFileManager() const;



    /////// XML Resource functions ///////
    /// Inits XML Resource system
    static void Initxrc(bool force=false);
    /// Loads XRC file(s) using data_path
    static void Loadxrc(wxString relpath);
    static bool LoadResource(const wxString& file);

    /// Loads Menubar from XRC
    static wxMenuBar* LoadMenuBar(wxString resid,bool createonfailure=false);
    /// Loads Menu from XRC
    static wxMenu* LoadMenu(wxString menu_id,bool createonfailure=false);
    /// Loads ToolBar from XRC
    static wxToolBar *LoadToolBar(wxFrame *parent,wxString resid,bool defaultsmall=true);
    /// Loads ToolBarAddOn from XRC into existing Toolbar

    // Do not use this, use Get()
    static Manager* Get(wxFrame* appWindow);

    static void AddonToolBar(wxToolBar* toolBar,wxString resid);
    static bool isToolBar16x16(wxToolBar* toolBar);
};

template <class MgrT> class Mgr
{
    static MgrT *instance;
    static bool isShutdown;
    explicit Mgr(const Mgr<MgrT>&){};
    Mgr<MgrT>& operator=(Mgr<MgrT> const&){};

protected:

    Mgr(){assert(Mgr<MgrT>::instance == 0);}
    virtual ~Mgr(){Mgr<MgrT>::instance = 0;}

public:

    static inline bool Valid(){return instance;}

    static inline MgrT* Get()
    {
        if(instance == 0 && isShutdown == false)
            instance = new MgrT();

        return instance;
    }

    static void Free()
    {
        isShutdown = true;
        delete instance;
        instance = 0;
    }
};
template<class MgrT>MgrT* Mgr<MgrT>::instance = 0;
template<class MgrT>bool  Mgr<MgrT>::isShutdown = false;

#endif // MANAGER_H

