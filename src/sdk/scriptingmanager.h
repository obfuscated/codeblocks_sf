#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "settings.h"
#include "manager.h"
#include <wx/intl.h>

class SquirrelError;

/** All about scripting in Code::Blocks.
  *
  * The scripting engine used here is Squirrel (http:://www.squirrel-lang.org).
  *
  * Example to load and execute a script follows:
  * <code>
  * Manager::Get()->GetScriptingManager()->LoadScript(_T("some.script"));
  * </code>
  *
  * Example to call a script function follows:
  * <code>
  * // int return value
  * int retValue = SqPlus::SquirrelFunction<int>("FunctionName")(_T("str_arg"), 5, 1.0);
  * // void return
  * SqPlus::SquirrelFunction<void>("FunctionName")(_T("str_arg"), 5, 1.0);
  * </code>
  * The templated type denotes the function's return type. Also note that the
  * function name is not unicode (we 're not using Squirrel in unicode mode).
  */
class DLLIMPORT ScriptingManager : public Mgr<ScriptingManager>
{
        friend class Mgr<ScriptingManager>;
    public:
        /** Loads a script.
          * @param script The script to run.
          * @return True if the script loaded and compiled, false if not.
          */
        bool LoadScript(const wxString& filename);

        /** Loads a string buffer.
          * @param script The script buffer to compile and run.
          * @param debugName A debug name. This will appear in any errors displayed.
          * @return True if the script compiled, false if not.
          */
        bool LoadBuffer(const wxString& buffer, const wxString& debugName = _T("CommandLine"));

        /** Returns an error string for the passed exception (if any) plus
          * any accumulated script engine errors (e.g. from failed function calls).
          * @param exception A pointer to the exception object containing the error. Can be NULL (default).
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          * @return The error string. If empty, it means "no errors".
          */
        wxString GetErrorString(SquirrelError* exception = 0, bool clearErrors = true);

        /** Displays an error dialog containing exception info and any other
          * script errors. Calls GetErrorString() internally.
          * You should normally call this function inside your catch handler for
          * SquirrelFunction<>() calls.
          * @param exception A pointer to the exception object containing the error. Can be NULL (default).
          * @param clearErrors If true (default), when this function returns all
          *        accumulated error messages are cleared.
          */
        void DisplayErrors(SquirrelError* exception = 0, bool clearErrors = true);

        /** @brief Injects script output.
          * This function is for advanced uses. It's used when some code sets a different
          * print function for the scripting engine. When this happens, ScriptingManager
          * no longer receives engine output. If you do something like that,
          * use this function to "forward" all script output to ScriptingManager.
          * @param output The engine's output to inject.
          */
        void InjectScriptOutput(const wxString& output);
	protected:
	private:
        ScriptingManager();
        ~ScriptingManager();
};

#endif // SCRIPTING_H
