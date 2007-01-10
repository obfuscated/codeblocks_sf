#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <map>
#include <set>

#include "settings.h"
#include "manager.h"
#include "menuitemsmanager.h"
#include <wx/intl.h>

class SquirrelError;

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

        // needed for SqPlus bindings
        ScriptingManager(const ScriptingManager& rhs) { cbThrow(_T("Can't call ScriptingManager's copy ctor!!!")); }
        void operator=(const ScriptingManager& rhs){ cbThrow(_T("Can't assign an ScriptingManager* !!!")); }

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
          * @return True if the script compiled, false if not.
          */
        bool LoadBuffer(const wxString& buffer, const wxString& debugName = _T("CommandLine"));

        /** @brief Loads a string buffer and captures its output.
          *
          * @param buffer The script buffer to compile and run.
          * @return The script's output (if any).
          */
        wxString LoadBufferRedirectOutput(const wxString& buffer);

        /** @brief Returns an accumulated error string.
          *
          * Returns an error string for the passed exception (if any) plus
          * any accumulated script engine errors (e.g. from failed function calls).
          * @param exception A pointer to the exception object containing the error. Can be NULL (default).
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          * @return The error string. If empty, it means "no errors".
          */
        wxString GetErrorString(SquirrelError* exception = 0, bool clearErrors = true);

        /** @brief Display error dialog.
          *
          * Displays an error dialog containing exception info and any other
          * script errors. Calls GetErrorString() internally.
          * You should normally call this function inside your catch handler for
          * SquirrelFunction<>() calls.
          * @param exception A pointer to the exception object containing the error. Can be NULL (default).
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          */
        void DisplayErrors(SquirrelError* exception = 0, bool clearErrors = true);

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

        /** @brief Registers a script plugin menu IDs with the callback function.
          *
          * @param name The script plugin's name.
          * @param ids The menu IDs to bind.
          * @return True on success, false on failure.
          */
        bool RegisterScriptPlugin(const wxString& name, const wxArrayInt& ids);

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
          * @param script The script's full filename.
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
	private:
        void OnScriptMenu(wxCommandEvent& event);
        void OnScriptPluginMenu(wxCommandEvent& event);
        void RegisterScriptFunctions();

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

        bool m_AttachedToMainWindow;
        wxString m_CurrentlyRunningScriptFile;

        typedef std::set<wxString> IncludeSet;
        IncludeSet m_IncludeSet;

        MenuItemsManager m_MenuItemsManager;

        DECLARE_EVENT_TABLE()
};

#endif // SCRIPTING_H
