/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef X_DEBUGGER_MANAGER_H
#define X_DEBUGGER_MANAGER_H

#include <map>
#include <vector>

#include <wx/string.h>

#include "prep.h"
#include "settings.h"
#include "manager.h"
#include "configmanager.h"

class wxMenu;
class wxToolBar;
class cbBacktraceDlg;
class cbBreakpointsDlg;
class cbCPURegistersDlg;
class cbDebuggerPlugin;
class cbDebugInterfaceFactory;
class cbDisassemblyDlg;
class cbExamineMemoryDlg;
class cbThreadsDlg;
class cbWatchesDlg;
class cbDebuggerMenuHandler;
class TextCtrlLogger;

class DLLIMPORT cbBreakpoint
{
    public:
        virtual ~cbBreakpoint() {}

        virtual void SetEnabled(bool flag) = 0;
        virtual wxString GetLocation() const = 0;
        virtual int GetLine() const = 0;
        virtual wxString GetLineString() const = 0;
        virtual wxString GetType() const = 0;
        virtual wxString GetInfo() const = 0;
        virtual bool IsEnabled() const = 0;
        virtual bool IsVisibleInEditor() const = 0;
        virtual bool IsTemporary() const = 0;
};

class DLLIMPORT cbWatch
{
        cbWatch& operator =(cbWatch &);
        cbWatch(cbWatch &);

    public:
        cbWatch();
    public:
        virtual void GetSymbol(wxString &symbol) const = 0;
        virtual void GetValue(wxString &value) const = 0;
        virtual bool SetValue(const wxString &value) = 0;
        virtual void GetFullWatchString(wxString &full_watch) const = 0;
        virtual void GetType(wxString &type) const = 0;
        virtual void SetType(const wxString &type) = 0;

        virtual wxString const & GetDebugString() const = 0;
    protected:
        virtual ~cbWatch();
    public:
        static void AddChild(cb::shared_ptr<cbWatch> parent, cb::shared_ptr<cbWatch> watch);
        void RemoveChild(int index);
        void RemoveChildren();
        bool RemoveMarkedChildren();
        int GetChildCount() const;
        cb::shared_ptr<cbWatch> GetChild(int index);
        cb::shared_ptr<const cbWatch> GetChild(int index) const;

        cb::shared_ptr<cbWatch> FindChild(const wxString& name);
        int FindChildIndex(const wxString& symbol) const;

        cb::shared_ptr<const cbWatch> GetParent() const;
        cb::shared_ptr<cbWatch> GetParent();

        bool IsRemoved() const;
        bool IsChanged() const;

        void MarkAsRemoved(bool flag);
        void MarkChildsAsRemoved();
        void MarkAsChanged(bool flag);
        void MarkAsChangedRecursive(bool flag);

        bool IsExpanded() const;
        void Expand(bool expand);

        bool IsAutoUpdateEnabled() const;
        void AutoUpdate(bool enabled);
    private:
        cb::weak_ptr<cbWatch> m_parent;
        std::vector<cb::shared_ptr<cbWatch> >    m_children;
        bool            m_changed;
        bool            m_removed;
        bool            m_expanded;
        bool            m_autoUpdate;
};

cb::shared_ptr<cbWatch> DLLIMPORT cbGetRootWatch(cb::shared_ptr<cbWatch> watch);

class DLLIMPORT cbStackFrame
{
    public:
        cbStackFrame();

        void SetNumber(int number);
        void SetAddress(uint64_t address);
        void SetSymbol(const wxString& symbol);
        void SetFile(const wxString& filename, const wxString &line);
        void MakeValid(bool flag);

        int GetNumber() const;
        uint64_t GetAddress() const;
        wxString GetAddressAsString() const;
        const wxString& GetSymbol() const;
        const wxString& GetFilename() const;
        const wxString& GetLine() const;
        bool IsValid() const;
    private:
        bool m_valid; ///< Is this stack frame valid?
        int m_number; ///< Stack frame's number (used in backtraces).
        uint64_t m_address; ///< Stack frame's address.
        wxString m_symbol; ///< Current function name.
        wxString m_file; ///< Current file.
        wxString m_line; ///< Current line in file.
};

class DLLIMPORT cbThread
{
    public:
        cbThread();
        cbThread(bool active, int number, const wxString& info);

        bool IsActive() const;
        int GetNumber() const;
        const wxString& GetInfo() const;

    private:
        bool m_active;
        int m_number;
        wxString m_info;
};

/**
  *
  */
class DLLIMPORT cbDebuggerConfiguration
{
    protected:
        cbDebuggerConfiguration(const cbDebuggerConfiguration &o);
        cbDebuggerConfiguration& operator =(const cbDebuggerConfiguration &);

    public:
        cbDebuggerConfiguration(const ConfigManagerWrapper &config);
        virtual ~cbDebuggerConfiguration() {}

        virtual cbDebuggerConfiguration* Clone() const = 0;

        virtual wxPanel* MakePanel(wxWindow *parent) = 0;
        virtual bool SaveChanges(wxPanel *panel) = 0;

        void SetName(const wxString &name);
        const wxString& GetName() const;

        const ConfigManagerWrapper& GetConfig() const;
        void SetConfig(const ConfigManagerWrapper &config);

        void SetMenuId(long id);
        long GetMenuId() const;

    protected:
        ConfigManagerWrapper m_config;
    private:
        wxString m_name;
        long m_menuId;
};

/**
  *
  */
struct DLLIMPORT cbDebuggerCommonConfig
{
    enum Flags
    {
        AutoBuild = 0,
        AutoSwitchFrame,
        ShowDebuggersLog,
        JumpOnDoubleClick,
        RequireCtrlForTooltips,
        ShowTemporaryBreakpoints
    };

    enum Perspective
    {
        OnlyOne = 0,
        OnePerDebugger,
        OnePerDebuggerConfig
    };

    static bool GetFlag(Flags flag);
    static void SetFlag(Flags flag, bool value);

    static wxString GetValueTooltipFont();
    static void SetValueTooltipFont(const wxString &font);

    static Perspective GetPerspective();
    static void SetPerspective(int perspective);
};

/**
  * Tries to detect the path to the debugger's executable.
  */
DLLIMPORT wxString cbDetectDebuggerExecutable(const wxString &exeName);

/** Convert a string in hex form to a uint64_t number.
 * \return The uint64_t representation of the string or 0 when the string can't be converted.
 */
DLLIMPORT uint64_t cbDebuggerStringToAddress(const wxString &address);

class DLLIMPORT DebuggerManager : public Mgr<DebuggerManager>
{
    private:
        DebuggerManager();
        ~DebuggerManager();

        friend class Mgr<DebuggerManager>;
        friend class Manager;
    public:
        typedef std::vector<cbDebuggerConfiguration*> ConfigurationVector;
        struct PluginData
        {
            friend class DebuggerManager;

            PluginData() :  m_lastConfigID(-1) {}

            ConfigurationVector& GetConfigurations() { return m_configurations; }
            const ConfigurationVector& GetConfigurations() const { return m_configurations; }

            cbDebuggerConfiguration* GetConfiguration(int index);

            void ClearConfigurations()
            {
                for (ConfigurationVector::iterator it = m_configurations.begin(); it != m_configurations.end(); ++it)
                    delete *it;
                m_configurations.clear();
            }
        private:
            ConfigurationVector m_configurations;
            int m_lastConfigID;
        };
        typedef std::map<cbDebuggerPlugin*, PluginData> RegisteredPlugins;

    public:
        /** Called to register a debugger plugin. It is called by cbDebuggerPlugin::OnAttach and it should not be called
          *  by the debugger plugins explicitly in their OnAttachReal methods. */
        bool RegisterDebugger(cbDebuggerPlugin *plugin);
        /** Called to unregister a debugger plugin. It is called by cbDebuggerPlugin::OnRelease and it should not be
          * called by the debugger plugins explicitly in their OnReleaseReal methods. */
        bool UnregisterDebugger(cbDebuggerPlugin *plugin);

        ConfigManagerWrapper NewConfig(cbDebuggerPlugin *plugin, const wxString &name);
        void RebuildAllConfigs();

        wxMenu* GetMenu();
        bool HasMenu() const;
        void BuildContextMenu(wxMenu &menu, const wxString& word_at_caret, bool is_running);

        TextCtrlLogger* GetLogger(int &index);
        TextCtrlLogger* GetLogger();
        void HideLogger();

    public: // debugger windows
        void SetInterfaceFactory(cbDebugInterfaceFactory *factory);
        cbDebugInterfaceFactory* GetInterfaceFactory();
        void SetMenuHandler(cbDebuggerMenuHandler *handler);
        cbDebuggerMenuHandler* GetMenuHandler();

        cbBacktraceDlg* GetBacktraceDialog();

        /** Returns a pointer to the breakpoints dialog.
          * It will return nullptr if there are no debugger plugins loaded.
          * Debugger plugin writers can treat it as always returning non-null value.
          */
        cbBreakpointsDlg* GetBreakpointDialog();

        /** Returns a pointer to the CPU registers dialog.
          * It will return nullptr if there are no debugger plugins loaded.
          * Debugger plugin writers can treat it as always returning non-null value.
          */
        cbCPURegistersDlg* GetCPURegistersDialog();

        /** Returns a pointer to the disassembly dialog.
          * It will return nullptr if there are no debugger plugins loaded.
          * Debugger plugin writers can treat it as always returning non-null value.
          */
        cbDisassemblyDlg* GetDisassemblyDialog();

        /** Returns a pointer to the memory dialog.
          * It will return nullptr if there are no debugger plugins loaded.
          * Debugger plugin writers can treat it as always returning non-null value.
          */
        cbExamineMemoryDlg* GetExamineMemoryDialog();

        /** Returns a pointer to the threads dialog.
          * It will return nullptr if there are no debugger plugins loaded.
          * Debugger plugin writers can treat it as always returning non-null value.
          */
        cbThreadsDlg* GetThreadsDialog();

        /** Returns a pointer to the watches dialog.
          * It will return nullptr if there are no debugger plugins loaded.
          * Debugger plugin writers can treat it as always returning non-null value.
          */
        cbWatchesDlg* GetWatchesDialog();

        bool ShowBacktraceDialog();

    public: // tests if something should be done
        bool UpdateBacktrace();
        bool UpdateCPURegisters();
        bool UpdateDisassembly();
        bool UpdateExamineMemory();
        bool UpdateThreads();

    public: // watches
        cbDebuggerPlugin* GetDebuggerHavingWatch(cb::shared_ptr<cbWatch> watch);
        bool ShowValueTooltip(const cb::shared_ptr<cbWatch> &watch, const wxRect &rect);

        RegisteredPlugins const & GetAllDebuggers() const;
        RegisteredPlugins & GetAllDebuggers();
        cbDebuggerPlugin* GetActiveDebugger();
        void SetActiveDebugger(cbDebuggerPlugin* activeDebugger, ConfigurationVector::const_iterator config);
        void SetTargetsDefaultAsActiveDebugger();
        bool IsActiveDebuggerTargetsDefault() const;

        bool IsDisassemblyMixedMode();
        void SetDisassemblyMixedMode(bool mixed);

    private:
        void ProcessSettings(RegisteredPlugins::iterator it);
        void FindTargetsDebugger();
        void RefreshUI();
        void CreateWindows();
        void DestoryWindows();

        void OnProjectActivated(CodeBlocksEvent& event);
        void OnTargetSelected(CodeBlocksEvent& event);
        void OnSettingsChanged(CodeBlocksEvent& event);
        void OnPluginLoadingComplete(CodeBlocksEvent& event);
    private:

        cbDebugInterfaceFactory *m_interfaceFactory;

        RegisteredPlugins m_registered;
        cbDebuggerPlugin* m_activeDebugger;
        cbDebuggerMenuHandler* m_menuHandler;

        cbBacktraceDlg* m_backtraceDialog;
        cbBreakpointsDlg* m_breakPointsDialog;
        cbCPURegistersDlg* m_cpuRegistersDialog;
        cbDisassemblyDlg* m_disassemblyDialog;
        cbExamineMemoryDlg* m_examineMemoryDialog;
        cbThreadsDlg* m_threadsDialog;
        cbWatchesDlg* m_watchesDialog;

        TextCtrlLogger* m_logger;
        int m_loggerIndex;
        bool m_isDisassemblyMixedMode;
        bool m_useTargetsDefault;
};

#endif // X_DEBUGGER_MANAGER_H

