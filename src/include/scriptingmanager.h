/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <map>
#include <set>

#ifndef CB_PRECOMP
    #include "cbexception.h" // cbThrow
    #include "globals.h" // cbC2U
    #include <wx/timer.h>
#endif

#include "settings.h"
#include "manager.h"
#include "menuitemsmanager.h"
#include <wx/intl.h>
#include <scripting/bindings/sc_cb_vm.h>
#include <scripting/sqrat/sqratUtil.h>
#include <scripting/bindings/sc_plugin.h>
#include "scripting/squirrel/sqrdbg.h"
#include "scripting/squirrel/sqdbgserver.h"

#define SCRIPT_BINDING_VERSION_MAJOR 2
#define SCRIPT_BINDING_VERSION_MINOR 0
#define SCRIPT_BINDING_VERSION_RELEASE 0

namespace ScriptBindings
{
    class cbScriptPlugin;
}


void PrintSquirrelToWxString(wxString &msg,const SQChar *s, va_list& vl);

/** @brief Provides scripting in Code::Blocks.
  *
  * The scripting engine used is Squirrel (http:://www.squirrel-lang.org).
  *
  * Here's an example to load and execute a script:
  *
  * @code
  * Manager::Get()->GetScriptingManager()->LoadScript(_T("some.script"));
  * @endcode
  *
  * The following is outdated ;) soon:
  * And here's an example to call a script function:
  *
  * @code
  * // int return value
  * // C++ equivalent: int retValue = FunctionName("str_arg", 5, 1.0);
  * SqPlus::SquirrelFunction<int> myfunc("FunctionName");
  * int retValue = myfunc(_T("str_arg"), 5, 1.0);
  * // void return
  * // C++ equivalent: FunctionName("str_arg", 5, 1.0);
  * SqPlus::SquirrelFunction<void> myfunc("FunctionName");
  * myfunc(_T("str_arg"), 5, 1.0);
  * @endcode
  *
  * The templated type denotes the function's return type. Also note that the
  * function name is not unicode (we 're not using Squirrel in unicode mode).
  */

using namespace ScriptBindings;

class DLLIMPORT ScriptingManager : public Mgr<ScriptingManager>, public wxEvtHandler
{
        friend class Mgr<ScriptingManager>;
        wxCriticalSection cs;
    public:
        /// Script trusts container struct
        struct TrustedScriptProps
        {
            bool permanent; // store trust in config (permanent trust)
            wxUint32 crc; // script's contents crc32
        };

        // script filename -> props
        /// Script trusts container struct
        typedef std::map<wxString, TrustedScriptProps> TrustedScripts;

        /** @brief Loads a script.
          *
          * @param filename The filename of the script to run.
          * @return True if the script loaded and compiled, false if not.
          */
        bool LoadScript(const wxString& filename);

        /** @brief Loads a string buffer.
          *
          * @param buffer The script buffer to compile and run.
          * @param debugName A debug name. This will appear in any errors displayed.
          * @param real_path If the debug name is not a path to a real file this has to be set on false. (This is needed to create a temporary file to allow the debugger to open also memory files)
          * @return True if the script compiled, false if not.
          */
        bool LoadBuffer(const wxString& buffer,wxString debugName/* = _T("CommandLine")*/,bool real_path = false);

        /** @brief Loads a string buffer and captures its output.
          *
          * @param buffer The script buffer to compile and run.
          * @return The script's output (if any).
          */
        wxString LoadBufferRedirectOutput(const wxString& buffer,const wxString& name);

        // TODO (bluehazzard#1#): Delete this if the Sqrat API doesn't change
        /** @brief Returns an accumulated error string.
          *
          * Returns an error string for the passed vm (if any) plus
          * any accumulated script engine errors (e.g. from failed function calls).
          * @param The vm from which the error should come
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          * @return The error string. If empty, it means "no errors".
          */
        //wxString GetErrorString(Sqrat::Exception* exception = nullptr, bool clearErrors = true);
        wxString GetErrorString(bool clearErrors = true);

        /** @brief Display error dialog.
          *
          * Displays an error dialog containing exception info and any other
          * script errors. If error_msg isempty the GetErrorString String is called
          * internally and if no error is found no error is displayed and the function returns _false_
          * @param error_msg The error message displayed with the error dialog
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          * @return _true_ if a error occurred, _false_ otherwise
          */
        bool DisplayErrors(wxString error_msg = wxEmptyString, bool clearErrors = true);

        /** @brief Display error dialog.
          *
          * Displays an error dialog containing exception info and any other
          * script errors. If error_msg isempty the GetErrorString String is called
          * internally and if no error is found no error is displayed and the function returns _false_
          * @param pre_error Text printed previous the error
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          * @return _true_ if a error occurred, _false_ otherwise
          */
        bool DisplayErrorsAndText(wxString pre_error, bool clearErrors = true);

        /** @brief Injects script output.
          *
          * This function is for advanced uses. It's used when some code sets a different
          * print function for the scripting engine. When this happens, ScriptingManager
          * no longer receives engine output. If you do something like that,
          * use this function to "forward" all script output to ScriptingManager.
          * @param output The engine's output to inject.
          */
        void InjectScriptOutput(const wxString& output);

        /** @brief Configure scripting in Code::Blocks.
          *
          * @return 0 on success or a negative value on error.
          */
        int Configure();

        /** @brief Registers a script plugin
          *
          * @param name The script plugin's name.
          * @param ids The menu IDs to bind.
          * @return True on success, false on failure.
          */
        bool RegisterScriptPlugin(const wxString& name, ScriptBindings::cbScriptPlugin* plugin);

        /** @brief Unregisters a script plugin
          *
          * @param name The script plugin's name.
          * @param ids The menu IDs to bind.
          * @return True on success, false on failure.
          */
        bool UnRegisterScriptPlugin(const wxString& name);

        /** @brief Script-bound function to register a script with a menu item.
          *
          * @param menuPath The full menu path. This can be separated by slashes (/)
          *                 to create submenus (e.g. "MyScripts/ASubMenu/MyItem").
          *                 If the last part of the string ("MyItem" in the example)
          *                 starts with a dash (-) (e.g. "-MyItem") then a menu
          *                 separator is prepended before the actual menu item.
          * @param scriptOrFunc The script's filename or a script's function name.
          * @param isFunction If true, the @c scriptOrFunc parameter is considered
          *       to be a script filename. If false, it is considered to be a
          *       script's function name.
          * @return True on success, false on failure.
          */
        bool RegisterScriptMenu(const wxString& menuPath, const wxString& scriptOrFunc, bool isFunction);

        /** @brief Script-bound function to unregister a script's menu item.
          *
          * @param menuPath The full menu path to unregister.
          * @return True on success, false on failure.
          */
        bool UnRegisterScriptMenu(const wxString& menuPath);

        /** @brief Unregister all scripts' menu items.
          *
          * @return True on success, false on failure.
          */
        bool UnRegisterAllScriptMenus();

        /** @brief Security function.
          *
          * @param script The script's full filename.
          * @return True if the script is trusted, false if not.
          *
          * @see TrustScript(), TrustCurrentlyRunningScript(), IsCurrentlyRunningScriptTrusted().
          */
        bool IsScriptTrusted(const wxString& script);

        /** @brief Security function.
          *
          * @return True if the script is trusted, false if not.
          *
          * @see TrustScript(), TrustCurrentlyRunningScript(), IsScriptTrusted().
          */
        bool IsCurrentlyRunningScriptTrusted();

        /** @brief Security function to trust a script.
          *
          * @param script The script's full filename.
          * @param permanently If true, this script will be trusted on a permanent basis.
          *                    If false, it will be trusted for this session only.
          *
          * @note When a script is marked as trusted, a CRC key is generated from its contents
          *       and stored for reference. When the IsScriptTrusted() function is called
          *       later on, besides checking the script's filename against the trusted
          *       scripts database, the CRC is checked too. If the CRC doesn't match, the
          *       script is not trusted anymore (the user is notified too).
          * @see TrustCurrentlyRunningScript()
          */
        void TrustScript(const wxString& script, bool permanently);

        /** @brief Security function to trust a script.
          *
          * @param permanently If true, this script will be trusted on a permanent basis.
          *                    If false, it will be trusted for this session only.
          * @see TrustScript()
          */
        void TrustCurrentlyRunningScript(bool permanently);

        /** @brief Remove a script trust.
          *
          * @return True if the trust existed and was removed, false if not.
          */
        bool RemoveTrust(const wxString& script);

        /** @brief Force refresh of script trusts. */
        void RefreshTrusts();

        /** @brief Access the script trusts container (const).
          *
          * @return The script trusts container.
          */
        const TrustedScripts& GetTrustedScripts();

        // needed for SqPlus bindings
        ScriptingManager& operator=(cb_unused const ScriptingManager& rhs) // prevent assignment operator
        {
        	cbThrow(_T("Can't assign a ScriptingManager* !!!"));
        	return *this;
		}

		ScriptBindings::CBSquirrelThread * CreateSandbox();

		ScriptBindings::CBsquirrelVM* GetVM()       {return m_vm;};


        /** \brief Search the plugin _Name_ and run its _Execute_ function
         *
         * \param Name wxString
         * \return int  1 On success
         *             -1 On script error. The error was reported by the script
         *             -2 "Execute" could not be found in the script
         *             -3 The script _Name_ could not be found
         *
         */
		int ExecutePlugin(wxString Name);

        /** \brief Search and return the Plugin with the _Name_
         *
         * \param Name wxString
         * \return cbScriptPlugin* nullptr it not found
         *
         */
		cbScriptPlugin* GetPlugin(wxString Name);

        /** \brief Ask all script plugins to create the "right click" menu
         *
         * \param type const ModuleType
         * \param menu wxMenu*
         * \param data const FileTreeData*
         * \return void
         *
         */
		void CreateModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data);

		unsigned int GetPluginCount()       {return m_registered_plugins.size();};

		cbScriptPlugin* GetPlugin(unsigned int index);

		bool UnInstallScriptPlugin(const wxString& name, bool del_files = true);
        bool InstallScriptPlugin(const wxString& file);


        int LoadFileFromZip(wxString path,wxString file,wxString& contents);

        void ParseDebuggerCMDLine(wxString cmd_line);

    private:
        // needed for SqPlus bindings
        ScriptingManager(cb_unused const ScriptingManager& rhs); // prevent copy construction

        void OnScriptMenu(wxCommandEvent& event);
        void OnScriptPluginMenu(wxCommandEvent& event);
        void RegisterScriptFunctions();
        void OnDebugTimer(wxTimerEvent& event);

        wxTimer m_DebugerUpdateTimer;

        ScriptingManager();
        ~ScriptingManager();

        TrustedScripts m_TrustedScripts;

        // container for script menus
        // script menuitem_ID -> script_filename
        struct MenuBoundScript
        {
            wxString scriptOrFunc;
            bool isFunc;
        };
        typedef std::map<int, MenuBoundScript> MenuIDToScript;
        MenuIDToScript m_MenuIDToScript;

        typedef std::map<wxString,cbScriptPlugin*> scripted_plugin_map;
        scripted_plugin_map m_registered_plugins;

        bool m_AttachedToMainWindow;
        wxString m_CurrentlyRunningScriptFile;

        typedef std::set<wxString> IncludeSet;
        IncludeSet m_IncludeSet;

        MenuItemsManager m_MenuItemsManager;

        ScriptBindings::CBsquirrelVM* m_vm;

        bool m_enable_debugger;
        HSQREMOTEDBG m_rdbg;
        IncludeSet m_debugger_created_temp_files;

        DECLARE_EVENT_TABLE()
};

typedef char SQChar; // HACK, MUST match with the type as defined for the dedicated platform in squirrel.h
void PrintSquirrelToWxString(wxString& msg, const SQChar* s, va_list& vl);

#endif // SCRIPTING_H
