/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SCRIPTING_H
#define SCRIPTING_H

#ifndef CB_PRECOMP
    #include "cbexception.h" // cbThrow
    #include "globals.h" // cbC2U
    #include "settings.h"
    #include "manager.h"
    #include "menuitemsmanager.h"

    #include <wx/intl.h>
#endif

/// Forward declare the Squirrel VM type. I'm doing this because including squirrel.h would require
/// changing include paths in a lot of projects. Unfortunately we have to keep this in sync with the
/// one in squirrel.h.
typedef struct SQVM* cbHSQUIRRELVM;

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
  * Caller caller(vm);
  * wxString strArg("str_arg");
  * SQInteger result;
  * if (!caller.CallAndReturn3(_SC("FunctionName"), &strArg, 5, 1.0))
  *     <handleTheErrorAppropriately>
  * // void return
  * // C++ equivalent: FunctionName("str_arg", 5, 1.0);
  * Caller caller(vm);
  * wxString strArg("str_arg");
  * if (!caller.Call3(_SC("FunctionName"), &strArg, 5, 1.0))
  *     <handleTheErrorAppropriately>
  * @endcode
  *
  * The templated type denotes the function's return type. Also note that the
  * function name is not unicode (we 're not using Squirrel in unicode mode).
  */
class DLLIMPORT ScriptingManager : public Mgr<ScriptingManager>
{
        friend class Mgr<ScriptingManager>;
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

        cbHSQUIRRELVM GetVM();

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
          * Returns an error string any accumulated script engine errors (e.g. from failed function
          * calls).
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          * @return The error string. If empty, it means "no errors".
          */
        wxString GetErrorString(bool clearErrors = true);

        /** @brief Display error dialog.
          *
          * Displays an error dialog containing info about script errors. Calls GetErrorString()
          * internally.
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          */
        void DisplayErrors(bool clearErrors = true);

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

        /// Register an integer constant.
        void BindIntConstant(const char *name, int64_t value);
        /// Register a bool constant
        void BindBoolConstant(const char *name, bool value);
        /// Register a string constant.
        void BindWxStringConstant(const char *name, const wxString &value);

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

        /// Return a type tag which can be used to register a class to squirrel.
        /// This is not infinite resource, so you have to call this rarely.
        /// Use TypeTag::Unassigned if you want to reset your type tag.
        uint32_t RequestClassTypeTag();

        ScriptingManager(const ScriptingManager& rhs) = delete;
        ScriptingManager& operator=(const ScriptingManager& rhs) = delete;

    private:
        ScriptingManager();
        ~ScriptingManager() override;

    public:
        struct Data;
    private:

        Data *m_data;
};

#endif // SCRIPTING_H
